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

#include "Link.hpp"
#include <iostream>

const std::string decorators::Link::scDecoratorName("Link");
const  DecoratorManager::Registerer decorators::Link::mRegisterer(decorators::Link::scDecoratorName, &decorators::Link::create);

decorators::FiducialDecorator *decorators::Link::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tColor = pSetting["color"];
		return new decorators::Link(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), pDecoratorManager.loadMarker(pSetting["target"]), tColor[0], tColor[1], tColor[2]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::Link::Link(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pTarget, float pRed, float pGreen, float pBlue):
FiducialDecorator(pDecoratorManager, pMarker),
mTarget(pTarget),
mRed(pRed),
mGreen(pGreen),
mBlue(pBlue)
{
}

void decorators::Link::update() {
	if (mMarker->isPresent() && mTarget->isPresent())
	{
		wykobi::point2d<float> tMarkerCenter = mMarker->getCenter();
		mDecoratorManager.GetCam2World().InterpolatedMap(tMarkerCenter);
		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tMarkerCenter);
		wykobi::point2d<float> tTargetCenter = mTarget->getCenter();
		mDecoratorManager.GetCam2World().InterpolatedMap(tTargetCenter);
		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tTargetCenter);

		mDecoratorManager.GetDisplay().RenderLine(tMarkerCenter.x, tMarkerCenter.y, tTargetCenter.x, tTargetCenter.y, mRed, mGreen, mBlue);
	}
}
