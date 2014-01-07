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

#include "InterventionLogger.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>

const std::string decorators::InterventionLogger::scDecoratorName("InterventionLogger");
const  DecoratorManager::Registerer decorators::InterventionLogger::mRegisterer(decorators::InterventionLogger::scDecoratorName, &decorators::InterventionLogger::create);

decorators::FiducialDecorator *decorators::InterventionLogger::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new InterventionLogger(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), pSetting["name"]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::InterventionLogger::InterventionLogger(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const std::string &pName):
FiducialDecorator(pDecoratorManager, pMarker),
mIntervening(false)
{
	char tFilename[256];
	time_t tTimestamp = time(0);
    struct tm *tTime = localtime(&tTimestamp);
	sprintf(tFilename, "./%d-%.2d-%.2d_%.2d-%.2d-%.2d.intervention.%s.csv", tTime->tm_year+1900, tTime->tm_mon+1, tTime->tm_mday, tTime->tm_hour, tTime->tm_min, tTime->tm_sec, pName.c_str());
	mLogfile.open(tFilename, std::ios_base::out);

	mLogfile << "Time, ";
	mLogfile << "Event, ";
	mLogfile << std::endl;
}

decorators::InterventionLogger::~InterventionLogger()
{
	mLogfile.close();
}

void decorators::InterventionLogger::update()
{
	if (mMarker->isPresent() != mIntervening)
	{
		mIntervening = !mIntervening;
		char tReadableTimestamp[64];
		time_t tTimestamp = time(0);
		struct tm *tTime = localtime(&tTimestamp);
		sprintf(tReadableTimestamp, "%.2d:%.2d:%.2d", tTime->tm_hour, tTime->tm_min, tTime->tm_sec);
		mLogfile << tReadableTimestamp << ", ";
		mLogfile << (mIntervening?"begin":"end") << ", ";
		mLogfile << std::endl;
	}
}
