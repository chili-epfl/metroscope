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

#include "TagLogger.hpp"
#include <iostream>
#include <sstream>
#include <qa/utils/Time.hpp>

const std::string decorators::TagLogger::scDecoratorName("TagLogger");
const DecoratorManager::Registerer decorators::TagLogger::mRegisterer(decorators::TagLogger::scDecoratorName, &decorators::TagLogger::create);

decorators::FiducialDecorator *decorators::TagLogger::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::TagLogger(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), pDecoratorManager.GetLogId(), pSetting["marker"]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::TagLogger::TagLogger(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const char *pRunId, const char *pLoggerId):
FiducialDecorator(pDecoratorManager, pMarker),
mStartTime(Time::MillisTimestamp())
{
	std::stringstream tFilename;
	if (pRunId) tFilename << pRunId << ".";
	tFilename << pLoggerId << ".csv";
	mLogfile.open(tFilename.str().c_str(), std::ios_base::out);

	mLogfile << "Time,";
	mLogfile << "x,";
	mLogfile << "y,";
	mLogfile << "Angle,";
	mLogfile << std::endl;
}

decorators::TagLogger::~TagLogger()
{
	mLogfile.close();
}

void decorators::TagLogger::update() {
	if (mMarker->isPresent())
	{
		wykobi::point2d<float> tMarkerCenter = mMarker->getCenter();
		mDecoratorManager.GetCam2World().InterpolatedMap(tMarkerCenter);

		mLogfile << Time::MillisTimestamp()-mStartTime << ",";
		mLogfile << tMarkerCenter.x << ",";
		mLogfile << tMarkerCenter.y << ",";
		mLogfile << mMarker->getAngle() << ",";
		mLogfile << std::endl;
	}
}
