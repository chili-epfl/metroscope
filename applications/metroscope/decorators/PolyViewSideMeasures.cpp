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

#include "PolyViewSideMeasures.hpp"
#include <iostream>

const std::string decorators::PolyViewSideMeasures::scDecoratorName("PolyViewSideMeasures");
const  DecoratorManager::Registerer decorators::PolyViewSideMeasures::mRegisterer(decorators::PolyViewSideMeasures::scDecoratorName, &decorators::PolyViewSideMeasures::create);

namespace {
	static const float scMM2CM = 10.0f;
	static const float scCM2MM = 1.0f/scMM2CM;
	static const int scRed = 0;
	static const int scYellow = 3;
	static const int scBlue = 6;
	static const int scMagenta = 9;
	static const int scGreen = 12;
	static const int scCyan = 15;
	static const float scColors[3*6] = {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f};
}

decorators::FiducialDecorator *decorators::PolyViewSideMeasures::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::PolyViewSideMeasures(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), *(PolyModel *)pDecoratorManager.loadDecorator(pSetting["model"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::PolyViewSideMeasures::PolyViewSideMeasures(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const PolyModel &pPolyModel):
FiducialDecorator(pDecoratorManager, pMarker),
mPolyModel(pPolyModel)
{
}

void decorators::PolyViewSideMeasures::update()
{
	if (mMarker->isPresent())
	{
		wykobi::quadix<float ,2> tMarkerCorners = mPolyModel.getMarker().getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
		wykobi::point2d<float> tOrigin;
		wykobi::point2d<float> tXUnit;
		wykobi::point2d<float> tYUnit;
		FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

		int tNumSides = mPolyModel.getSize();

		wykobi::polygon<float ,2> tPolyCorners;
		for (int i = 0; i < tNumSides; ++i) {
			tPolyCorners.push_back( tOrigin + mPolyModel.getPolygon()[i].x*tXUnit + mPolyModel.getPolygon()[i].y*tYUnit );
		}
		wykobi::point2d<float> tMiddles[tNumSides];
		wykobi::vector2d<float> tSides[tNumSides];

		float* tMeasures = new float[tNumSides];
		for (int i = 0; i < tNumSides; ++i) {
			tMiddles[i] = 0.5f*(tPolyCorners[i] + tPolyCorners[(i+1)%tNumSides]);
			tSides[i] = tPolyCorners[(i+1)%tNumSides] - tPolyCorners[i];
			//tMeasures[i] = (int) (0.5f+scCM2MM*wykobi::distance(tPolyCorners[i], tPolyCorners[(i+1)%tNumSides]));
			//TODO: Modify the scale to fit the board game dimensions
			//tMeasures[i] = (int) (0.5f+scCM2MM*wykobi::distance(tPolyCorners[i], tPolyCorners[(i+1)%tNumSides]));
			tMeasures[i] = wykobi::distance(mPolyModel.getCoords()[i], mPolyModel.getCoords()[(i+1)%tNumSides]);
		}

		int tInitialColorIndices[6] = { scRed, scYellow, scBlue, scMagenta, scGreen, scCyan};
		int* tColorIndices = new int[tNumSides];
		for (int i = 0; i < tNumSides; ++i) tColorIndices[i] = tInitialColorIndices[i%6];

		for (int i = 0; i < tNumSides; ++i) {
			for (int j = 0; j < i; ++j) {
				//if (tMeasures[i] == tMeasures[j])	tColorIndices[i] = tColorIndices[j];
				if (wykobi::abs(tMeasures[i] - tMeasures[j]) < wykobi::Epsilon)
					tColorIndices[i] = tColorIndices[j];
			}
		}

		for (int i = 0; i < tNumSides; ++i) {
			wykobi::point2d<float> tTextPosition = tMiddles[i]+10.0f*wykobi::normalize(wykobi::perpendicular(tSides[i]));
			mDecoratorManager.GetWorld2Proj().InterpolatedMap(tTextPosition);
			char tText[20];
			sprintf(tText, "%1.3f", tMeasures[i]);
			mDecoratorManager.GetDisplay().RenderText(tText, tTextPosition.x, tTextPosition.y, 1.0f, scColors[tColorIndices[i]], scColors[tColorIndices[i]+1], scColors[tColorIndices[i]+2]);
		}
	}
}
