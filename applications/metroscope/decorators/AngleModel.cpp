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

#include "AngleModel.hpp"
#include <iostream>

const std::string decorators::AngleModel::scDecoratorName("AngleModel");
const DecoratorManager::Registerer decorators::AngleModel::mRegisterer(decorators::AngleModel::scDecoratorName, &decorators::AngleModel::create);

decorators::FiducialDecorator *decorators::AngleModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		if (pSetting.exists("start"))
			return new decorators::AngleModel(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), pDecoratorManager.loadMarker(pSetting["start"]), pDecoratorManager.loadMarker(pSetting["end"]), pSetting["clockwise"]);
		return new decorators::AngleModel(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), pDecoratorManager.loadMarker(pSetting["end"]), pSetting["clockwise"]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::AngleModel::AngleModel(DecoratorManager &pDecoratorManager, FiducialMarker *pOrigin, FiducialMarker *pStart, FiducialMarker *pEnd, bool pClockwise):
FiducialDecorator(pDecoratorManager, pOrigin),
mStart(pStart),
mEnd(pEnd),
mOrientation(pClockwise?wykobi::Clockwise:wykobi::CounterClockwise)
{
}

decorators::AngleModel::AngleModel(DecoratorManager &pDecoratorManager, FiducialMarker *pOrigin, FiducialMarker *pEnd, bool pClockwise):
FiducialDecorator(pDecoratorManager, pOrigin),
mStart(0),
mEnd(pEnd),
mOrientation(pClockwise?wykobi::Clockwise:wykobi::CounterClockwise)
{
}

void decorators::AngleModel::NormalizeAngle(float &pAngle)
{
	while (pAngle < 0.0f) pAngle += 360.0f;
	while (pAngle >= 360.0f) pAngle -= 360.0f;
}

float decorators::AngleModel::AbsoluteDifference(float pAngle1, float pAngle2)
{
	return 180.0f - wykobi::abs(180.0f - wykobi::abs(pAngle1 - pAngle2));
}

void decorators::AngleModel::update() {
	if (isPresent())
	{
		mOriginPoint = mMarker->getCenter();
		mStartPoint = mStart?mStart->getCenter():mOriginPoint+mMarker->getXUnit();
		mEndPoint = mEnd->getCenter();

		wykobi::point2d<float> tWorldOriginPoint = mOriginPoint;
		wykobi::point2d<float> tWorldStartPoint = mStartPoint;
		wykobi::point2d<float> tWorldEndPoint = mEndPoint;
		mDecoratorManager.GetCam2World().InterpolatedMap(tWorldOriginPoint);
		mDecoratorManager.GetCam2World().InterpolatedMap(tWorldStartPoint);
		mDecoratorManager.GetCam2World().InterpolatedMap(tWorldEndPoint);

		mAngle = wykobi::oriented_vertex_angle(tWorldStartPoint, tWorldOriginPoint, tWorldEndPoint, mOrientation);
	}
}
