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

#include "CraftagReplay.hpp"
#include <qa/components/vision/CraftagRegistrar.hpp>
#include <qa/utils/Time.hpp>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>

CraftagReplay::CraftagReplay(
		const char *pFilename,
		CraftagRegistrar &pCraftagRegistrar):
mCraftagRegistrar(pCraftagRegistrar),
mLogIndex(0)
{
	try {
		mLogfile.readFile(pFilename);
	} catch(libconfig::FileIOException &) {
		std::cerr << "CraftagReplay: FileIOException with "<< pFilename << std::endl;
	} catch(libconfig::ParseException &e) {
		std::cerr << pFilename << ", line "<< e.getLine() << ": " << e.getError() << std::endl;
	}
	long long tLogStart = mLogfile.getRoot()[0][mLogIndex]["timestamp"];
	mTimeShift = Time::MillisTimestamp() - tLogStart;
	mNow = tLogStart;

	mCraftagRegistrar.reset();
}

CraftagReplay::~CraftagReplay()
{
}

void CraftagReplay::run()
{
	libconfig::Setting &tLogs = mLogfile.getRoot()[0];
	if (mLogIndex+1 < tLogs.getLength())
	{
		mNow = Time::MillisTimestamp() - mTimeShift;
		libconfig::Setting &tCurrentLog = tLogs[mLogIndex];
		long long tNextTimestamp = tCurrentLog["timestamp"];
		long tTimeToWait = tNextTimestamp - mNow;

		if (tTimeToWait <= 0) std::cerr << -tTimeToWait << "ms delay in CraftagReplay" << std::endl;
		else Time::Sleep(tTimeToWait);

		libconfig::Setting &pMarkers = tCurrentLog["tags"];
		mCraftagRegistrar.reset();
		CvPoint2D32f tCvCorners[4];
		for (int i = 0; i < pMarkers.getLength(); ++i)
		{
			libconfig::Setting &tCorners = pMarkers[i]["corners"];
			for (int j = 0; j < 4; ++j) {
				tCvCorners[j].x = tCorners[2*j];
				tCvCorners[j].y = tCorners[2*j+1];
			}
			int tId = pMarkers[i]["id"];
			mCraftagRegistrar.registerCraftag(tId);
			mCraftagRegistrar.set(tId, tCvCorners);
		}

		++mLogIndex;
	}
	else stop();
}
