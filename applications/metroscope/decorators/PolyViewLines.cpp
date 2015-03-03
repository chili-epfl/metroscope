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

#include "PolyViewLines.hpp"
#include <iostream>

const std::string decorators::PolyViewLines::scDecoratorName("PolyViewLines");
const  DecoratorManager::Registerer decorators::PolyViewLines::mRegisterer(decorators::PolyViewLines::scDecoratorName, &decorators::PolyViewLines::create);

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
	//TODO: We only do until 6 different colors! Should be enough for now...
	static const float scLineLength = 100.0f;
	static const wykobi::point2d<float> scZero;
}

decorators::FiducialDecorator *decorators::PolyViewLines::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::PolyViewLines(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), *(PolyModel *)pDecoratorManager.loadDecorator(pSetting["model"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::PolyViewLines::PolyViewLines(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const PolyModel &pPolyModel):
FiducialDecorator(pDecoratorManager, pMarker),
mPolyModel(pPolyModel)
{
}

void decorators::PolyViewLines::update()
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
			tPolyCorners.push_back(tOrigin + mPolyModel.getPolygon()[i].x*tXUnit + mPolyModel.getPolygon()[i].y*tYUnit);
		}

		wykobi::vector2d<float> tSideVectors[tNumSides];
		for (int i = 0; i < tNumSides; ++i) {
			tSideVectors[i] = wykobi::normalize(wykobi::make_vector(tPolyCorners[(i+1)%tNumSides]-tPolyCorners[i]));
		}


		int tInitialColorIndices[6] = { scRed, scYellow, scBlue, scMagenta, scGreen, scCyan};
		int* tColorIndices = new int[tNumSides];
		for (int i = 0; i < tNumSides; ++i) tColorIndices[i] = tInitialColorIndices[i%6];

		for (int i = 0; i < tNumSides; ++i) {
			for (int j = 0; j < i; ++j) {
				if (wykobi::collinear(mPolyModel.getPolygon()[(i+1)%tNumSides]-mPolyModel.getPolygon()[i%tNumSides], mPolyModel.getPolygon()[(j+1)%tNumSides]-mPolyModel.getPolygon()[j%tNumSides], scZero))
				{
					tColorIndices[i] = tColorIndices[j];
				}
			}
		}


		for (int i = 0; i < tNumSides; ++i) {
			wykobi::point2d<float> tSideMiddle = 0.5f*(tPolyCorners[(i+1)%tNumSides]+tPolyCorners[i]);
			wykobi::point2d<float> tA= tSideMiddle+scLineLength*tSideVectors[i];
			wykobi::point2d<float> tB= tSideMiddle-scLineLength*tSideVectors[i];
			mDecoratorManager.GetWorld2Proj().InterpolatedMap(tA);
			mDecoratorManager.GetWorld2Proj().InterpolatedMap(tB);
			mDecoratorManager.GetDisplay().RenderLine(tA.x, tA.y, tB.x, tB.y, scColors[tColorIndices[i]], scColors[tColorIndices[i]+1], scColors[tColorIndices[i]+2]);
		}
	}
}
