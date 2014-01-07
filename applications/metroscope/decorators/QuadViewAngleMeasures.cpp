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

#include "QuadViewAngleMeasures.hpp"
#include <iostream>

const std::string decorators::QuadViewAngleMeasures::scDecoratorName("QuadViewAngleMeasures");
const  DecoratorManager::Registerer decorators::QuadViewAngleMeasures::mRegisterer(decorators::QuadViewAngleMeasures::scDecoratorName, &decorators::QuadViewAngleMeasures::create);

namespace {
	static const int scRed = 0;
	static const int scYellow = 3;
	static const int scBlue = 6;
	static const int scMagenta = 9;
	static const float scColors[3*4] = {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
}

decorators::FiducialDecorator *decorators::QuadViewAngleMeasures::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::QuadViewAngleMeasures(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), *(QuadModel *)pDecoratorManager.loadDecorator(pSetting["model"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::QuadViewAngleMeasures::QuadViewAngleMeasures(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const QuadModel &pQuadModel):
FiducialDecorator(pDecoratorManager, pMarker),
mQuadModel(pQuadModel)
{
}

void decorators::QuadViewAngleMeasures::update() {

	if (mMarker->isPresent())
	{
		wykobi::quadix<float ,2> tMarkerCorners = mQuadModel.getMarker().getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
		wykobi::point2d<float> tOrigin;
		wykobi::point2d<float> tXUnit;
		wykobi::point2d<float> tYUnit;
		FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

		float tMeasures[4];
		for (int i = 0; i < 4; ++i) {
			tMeasures[i] = wykobi::oriented_vertex_angle(mQuadModel.getQuad()[(i+3)%4], mQuadModel.getQuad()[i], mQuadModel.getQuad()[(i+1)%4]);
		}

		wykobi::quadix<float ,2> tQuadCorners;
		for (int i = 0; i < 4; ++i) {
			tQuadCorners[i] = tOrigin + mQuadModel.getQuad()[i].x*tXUnit + mQuadModel.getQuad()[i].y*tYUnit;
		}

		mDecoratorManager.GetWorld2Proj().InterpolatedMapOnQuad(tQuadCorners);


		int tColorIndices[4] = { scRed, scYellow, scBlue, scMagenta};
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < i; ++j) {
				if (wykobi::abs(tMeasures[i] - tMeasures[j]) < wykobi::Epsilon)
					tColorIndices[i] = tColorIndices[j];
			}
		}

		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);
		for (int i = 0; i < 4; ++i) {
			if (wykobi::abs(tMeasures[i] - 90.0f) < wykobi::Epsilon)
			{
				wykobi::point2d<float> tSide1 = 10.0f*wykobi::normalize(tQuadCorners[(i+3)%4]-tQuadCorners[i]);
				wykobi::point2d<float> tSide2 = 10.0f*wykobi::normalize(tQuadCorners[(i+1)%4]-tQuadCorners[i]);
				wykobi::point2d<float> tOppositeCorner = tSide1+tSide2;
				tSide1 = tQuadCorners[i]+tSide1;
				tSide2 = tQuadCorners[i]+tSide2;
				tOppositeCorner = tQuadCorners[i]+tOppositeCorner;
				mDecoratorManager.GetDisplay().RenderLine(tSide1.x, tSide1.y, tOppositeCorner.x, tOppositeCorner.y, scColors[tColorIndices[i]], scColors[tColorIndices[i]+1], scColors[tColorIndices[i]+2]);
				mDecoratorManager.GetDisplay().RenderLine(tSide2.x, tSide2.y, tOppositeCorner.x, tOppositeCorner.y, scColors[tColorIndices[i]], scColors[tColorIndices[i]+1], scColors[tColorIndices[i]+2]);
			}
			else
			{
				float tOrientation1 = wykobi::cartesian_angle(tQuadCorners[(i+3)%4]-tQuadCorners[i]);
				float tOrientation2 = wykobi::cartesian_angle(tQuadCorners[(i+1)%4]-tQuadCorners[i]);
				mDecoratorManager.GetDisplay().RenderArc(
						tQuadCorners[i].x, tQuadCorners[i].y,
						20.0f, 20.0f,
						tOrientation2, tOrientation1,
						scColors[tColorIndices[i]], scColors[tColorIndices[i]+1], scColors[tColorIndices[i]+2]);
			}

			wykobi::point2d<float> tCorner = tQuadCorners[i]-tOrigin;
			wykobi::point2d<float> tTextPosition = tQuadCorners[i]+50.0f*wykobi::normalize(wykobi::make_vector(tCorner));
			char tText[20];
			sprintf(tText, "%.0f°", tMeasures[i]);
			mDecoratorManager.GetDisplay().RenderText(tText, tTextPosition.x, tTextPosition.y, 1.0f, scColors[tColorIndices[i]], scColors[tColorIndices[i]+1], scColors[tColorIndices[i]+2]);
		}
	}


}
