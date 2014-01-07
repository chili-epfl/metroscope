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

#include "Line.hpp"
#include <iostream>

const std::string decorators::Line::scDecoratorName("Line");
const  DecoratorManager::Registerer decorators::Line::mRegisterer(decorators::Line::scDecoratorName, &decorators::Line::create);

decorators::FiducialDecorator *decorators::Line::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
			libconfig::Setting & tColor = pSetting["color"];
			return new decorators::Line(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), tColor[0], tColor[1], tColor[2], pSetting["length"]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
		}
		return 0;
}

decorators::Line::Line(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, float pRed, float pGreen, float pBlue, float pLength):
FiducialDecorator(pDecoratorManager, pMarker),
mRed(pRed),
mGreen(pGreen),
mBlue(pBlue),
mHalfLength(pLength/2.0f)
{
}

void decorators::Line::update() {
	if (mMarker->isPresent())
	{
		wykobi::quadix<float ,2> tMarkerCorners = mMarker->getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
		wykobi::point2d<float> tOrigin;
		wykobi::point2d<float> tXUnit;
		wykobi::point2d<float> tYUnit;
		FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

		wykobi::point2d<float> tLineStart = tOrigin-mHalfLength*tXUnit;
		wykobi::point2d<float> tLineEnd = tOrigin+mHalfLength*tXUnit;

		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tLineStart);
		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tLineEnd);

		mDecoratorManager.GetDisplay().RenderLine(tLineStart.x, tLineStart.y, tLineEnd.x, tLineEnd.y, mRed, mGreen, mBlue);
	}
}
