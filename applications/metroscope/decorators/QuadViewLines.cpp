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

#include "QuadViewLines.hpp"
#include <iostream>

const std::string decorators::QuadViewLines::scDecoratorName("QuadViewLines");
const  DecoratorManager::Registerer decorators::QuadViewLines::mRegisterer(decorators::QuadViewLines::scDecoratorName, &decorators::QuadViewLines::create);

namespace {
	static const float scMM2CM = 10.0f;
	static const float scCM2MM = 1.0f/scMM2CM;
	static const int scRed = 0;
	static const int scYellow = 3;
	static const int scBlue = 6;
	static const int scMagenta = 9;
	static const float scColors[3*4] = {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
	static const float scLineLength = 100.0f;
	static const wykobi::point2d<float> scZero;
}

decorators::FiducialDecorator *decorators::QuadViewLines::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::QuadViewLines(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), *(QuadModel *)pDecoratorManager.loadDecorator(pSetting["model"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::QuadViewLines::QuadViewLines(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const QuadModel &pQuadModel):
FiducialDecorator(pDecoratorManager, pMarker),
mQuadModel(pQuadModel)
{
}

void decorators::QuadViewLines::update()
{
	if (mMarker->isPresent())
	{
		wykobi::quadix<float ,2> tMarkerCorners = mQuadModel.getMarker().getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
		wykobi::point2d<float> tOrigin;
		wykobi::point2d<float> tXUnit;
		wykobi::point2d<float> tYUnit;
		FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);
		wykobi::quadix<float ,2> tQuadCorners;
		for (int i = 0; i < 4; ++i) {
			tQuadCorners[i] = tOrigin + mQuadModel.getQuad()[i].x*tXUnit + mQuadModel.getQuad()[i].y*tYUnit;
		}

		wykobi::vector2d<float> tSideVectors[4];
		for (int i = 0; i < 4; ++i) {
			tSideVectors[i] = wykobi::normalize(wykobi::make_vector(tQuadCorners[(i+1)%4]-tQuadCorners[i]));
		}


		int tColorIndices[4] = { scRed, scYellow, scBlue, scMagenta};
		if ( wykobi::collinear(mQuadModel.getQuad()[1]-mQuadModel.getQuad()[0], mQuadModel.getQuad()[3]-mQuadModel.getQuad()[2], scZero) )
		{
			tColorIndices[2] = tColorIndices[0];
		}
		if ( wykobi::collinear(mQuadModel.getQuad()[2]-mQuadModel.getQuad()[1], mQuadModel.getQuad()[0]-mQuadModel.getQuad()[3], scZero) )
		{
			tColorIndices[3] = tColorIndices[1];
		}


		for (int i = 0; i < 4; ++i) {
			wykobi::point2d<float> tSideMiddle = 0.5f*(tQuadCorners[(i+1)%4]+tQuadCorners[i]);
			wykobi::point2d<float> tA= tSideMiddle+scLineLength*tSideVectors[i];
			wykobi::point2d<float> tB= tSideMiddle-scLineLength*tSideVectors[i];
			mDecoratorManager.GetWorld2Proj().InterpolatedMap(tA);
			mDecoratorManager.GetWorld2Proj().InterpolatedMap(tB);
			mDecoratorManager.GetDisplay().RenderLine(tA.x, tA.y, tB.x, tB.y, scColors[tColorIndices[i]], scColors[tColorIndices[i]+1], scColors[tColorIndices[i]+2]);
		}
	}
}
