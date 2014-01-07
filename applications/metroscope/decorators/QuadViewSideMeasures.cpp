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

#include "QuadViewSideMeasures.hpp"
#include <iostream>

const std::string decorators::QuadViewSideMeasures::scDecoratorName("QuadViewSideMeasures");
const  DecoratorManager::Registerer decorators::QuadViewSideMeasures::mRegisterer(decorators::QuadViewSideMeasures::scDecoratorName, &decorators::QuadViewSideMeasures::create);

namespace {
	static const float scMM2CM = 10.0f;
	static const float scCM2MM = 1.0f/scMM2CM;
	static const int scRed = 0;
	static const int scYellow = 3;
	static const int scBlue = 6;
	static const int scMagenta = 9;
	static const float scColors[3*4] = {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
}

decorators::FiducialDecorator *decorators::QuadViewSideMeasures::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::QuadViewSideMeasures(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), *(QuadModel *)pDecoratorManager.loadDecorator(pSetting["model"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::QuadViewSideMeasures::QuadViewSideMeasures(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const QuadModel &pQuadModel):
FiducialDecorator(pDecoratorManager, pMarker),
mQuadModel(pQuadModel)
{
}

void decorators::QuadViewSideMeasures::update()
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
		wykobi::point2d<float> tMiddles[4];
		wykobi::vector2d<float> tSides[4];
		int tMeasures[4];
		for (int i = 0; i < 4; ++i) {
			tMiddles[i] = 0.5f*(tQuadCorners[i] + tQuadCorners[(i+1)%4]);
			tSides[i] = tQuadCorners[(i+1)%4] - tQuadCorners[i];
			tMeasures[i] = (int) (0.5f+scCM2MM*wykobi::distance(tQuadCorners[i], tQuadCorners[(i+1)%4]));
		}

		int tColorIndices[4] = { scRed, scYellow, scBlue, scMagenta};
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < i; ++j) {
				if (tMeasures[i] == tMeasures[j])
					tColorIndices[i] = tColorIndices[j];
			}
		}

		for (int i = 0; i < 4; ++i) {
			wykobi::point2d<float> tTextPosition = tMiddles[i]+10.0f*wykobi::normalize(wykobi::perpendicular(tSides[i]));
			mDecoratorManager.GetWorld2Proj().InterpolatedMap(tTextPosition);
			char tText[20];
			sprintf(tText, "%d cm", tMeasures[i]);
			mDecoratorManager.GetDisplay().RenderText(tText, tTextPosition.x, tTextPosition.y, 1.0f, scColors[tColorIndices[i]], scColors[tColorIndices[i]+1], scColors[tColorIndices[i]+2]);
		}
	}
}
