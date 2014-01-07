/*******************************************************************************
*   Copyright 2013 EPFL                                                        *
*                                                                              *
*   This file is part of metroscope.                                           *
*                                                                              *
*   Metroscope is free software: you can redistribute it and/or modify         *
*   it under the terms of the GNU General Public License as                    *
*   published by the Free Software Foundation, either version 3 of the         *
*   License, or (at your option) any later version.                            *
*                                                                              *
*   Metroscope is distributed in the hope that it will be useful,              *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of             *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
*   GNU General Public License for more details.                               *
*                                                                              *
*   You should have received a copy of the GNU General Public License          *
*   along with Metroscope.  If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************/

#include "QuadMirror.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>

const std::string decorators::QuadMirror::scDecoratorName("QuadMirror");
const  DecoratorManager::Registerer decorators::QuadMirror::mRegisterer(decorators::QuadMirror::scDecoratorName, &decorators::QuadMirror::create);

decorators::FiducialDecorator *decorators::QuadMirror::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::QuadMirror(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), *(QuadModel *)pDecoratorManager.loadDecorator(pSetting["model"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::QuadMirror::QuadMirror(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const QuadModel &pQuadModel):
FiducialDecorator(pDecoratorManager, pMarker),
mQuadModel(pQuadModel),
mAnimationStart(Time::MillisTimestamp())
{
}

void decorators::QuadMirror::update() {
	if (mMarker->isPresent() && mQuadModel.getMarker().isPresent())
	{

		wykobi::quadix<float ,2> tMarkerCorners = mMarker->getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
		wykobi::point2d<float> tAxis1 = 0.5f*(tMarkerCorners[0]+tMarkerCorners[1]);
		wykobi::point2d<float> tAxis2 = 0.5f*(tMarkerCorners[2]+tMarkerCorners[3]);
		wykobi::line<float, 2> tAxis = wykobi::make_line(tAxis1, tAxis2);

		wykobi::quadix<float, 2> tModelMarkerCorners = mQuadModel.getMarker().getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tModelMarkerCorners);
		wykobi::point2d<float> tOrigin;
		wykobi::point2d<float> tXUnit;
		wykobi::point2d<float> tYUnit;
		FiducialMarker::ComputeBasisFromSquare(tModelMarkerCorners, tOrigin, tXUnit, tYUnit);

		wykobi::quadix<float, 2> tQuadCorners;
		for (int i = 0; i < 4; ++i) {
			tQuadCorners[i] = tOrigin + mQuadModel.getQuad()[i].x*tXUnit + mQuadModel.getQuad()[i].y*tYUnit;
		}
		wykobi::quadix<float, 2> tMirroredQuadCorners = wykobi::mirror(tQuadCorners, tAxis);

		mDecoratorManager.GetWorld2Proj().InterpolatedMapOnQuad(tQuadCorners);
		mDecoratorManager.GetWorld2Proj().InterpolatedMapOnQuad(tMirroredQuadCorners);
		for (int i = 0; i < 4; ++i) {
			if (Time::MillisTimestamp() >= mAnimationStart + scAnimationLength)
			{
				mDecoratorManager.GetDisplay().RenderLine(
						tQuadCorners[i].x, tQuadCorners[i].y,
						tMirroredQuadCorners[i].x, tMirroredQuadCorners[i].y,
						1.0f, 0.0f, 0.0f);
				mDecoratorManager.GetDisplay().RenderLine(
						tMirroredQuadCorners[i].x, tMirroredQuadCorners[i].y,
						tMirroredQuadCorners[(i+1)%4].x, tMirroredQuadCorners[(i+1)%4].y,
						0.0f, 0.0f, 0.0f);
			} else {
				float tProgress = (Time::MillisTimestamp()-mAnimationStart)/(float) (scAnimationLength);
				wykobi::point2d<float> tCenter = 0.5f*(tQuadCorners[i]+tMirroredQuadCorners[i]);
				wykobi::point2d<float> tP1 = tCenter + tProgress*(tQuadCorners[i]-tCenter);
				wykobi::point2d<float> tP2 = tCenter + tProgress*(tMirroredQuadCorners[i]-tCenter);
				mDecoratorManager.GetDisplay().RenderLine(
						tP1.x, tP1.y,
						tP2.x, tP2.y,
						1.0f, 0.0f, 0.0f);
			}
		}
	} else mAnimationStart = Time::MillisTimestamp();
}
