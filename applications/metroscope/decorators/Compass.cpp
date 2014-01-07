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

#include "Compass.hpp"
#include <iostream>

const std::string decorators::Compass::scDecoratorName("Compass");
const DecoratorManager::Registerer decorators::Compass::mRegisterer(decorators::Compass::scDecoratorName, &decorators::Compass::create);

decorators::FiducialDecorator *decorators::Compass::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tColor = pSetting["color"];
		std::string tDebugM = pSetting["marker"];
		std::string tDebugT = pSetting["target"];
		return new decorators::Compass(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), pDecoratorManager.loadMarker(pSetting["target"]), tColor[0], tColor[1], tColor[2]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::Compass::Compass(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pTarget, float pRed, float pGreen, float pBlue):
FiducialDecorator(pDecoratorManager, pMarker),
mTarget(pTarget),
mRed(pRed),
mGreen(pGreen),
mBlue(pBlue)
{
}

void decorators::Compass::update() {
	static const float scArrowHalfLength = 2.0f;
	static const float scPointerHeight = 0.5f;
	static const float scPointerWidth = scPointerHeight;

	if (mMarker->isPresent())
	{
		wykobi::quadix<float ,2> tMarkerCorners = mMarker->getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
		wykobi::point2d<float> tOrigin;
		wykobi::point2d<float> tXUnit;
		wykobi::point2d<float> tYUnit;
		FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

		wykobi::point2d<float> tArrowStart = tOrigin - scArrowHalfLength*tXUnit;
		wykobi::point2d<float> tArrowEnd = tOrigin + scArrowHalfLength*tXUnit;
		wykobi::point2d<float> tPointerTop = tArrowEnd - scPointerWidth*tXUnit + scPointerHeight*tYUnit;
		wykobi::point2d<float> tPointerBottom = tArrowEnd - scPointerWidth*tXUnit - scPointerHeight*tYUnit;
		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tArrowStart);
		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tArrowEnd);
		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tPointerTop);
		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tPointerBottom);

		mDecoratorManager.GetDisplay().RenderLine(tArrowStart.x, tArrowStart.y, tArrowEnd.x, tArrowEnd.y, mRed, mGreen, mBlue);
		mDecoratorManager.GetDisplay().RenderLine(tPointerTop.x, tPointerTop.y, tArrowEnd.x, tArrowEnd.y, mRed, mGreen, mBlue);
		mDecoratorManager.GetDisplay().RenderLine(tPointerBottom.x, tPointerBottom.y, tArrowEnd.x, tArrowEnd.y, mRed, mGreen, mBlue);
	}
}
