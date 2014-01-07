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

#include "SnapshotTaker.hpp"
#include "../../utils/Time.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <sstream>
#include <iostream>
#include <stdio.h>

SnapshotTaker::SnapshotTaker(
	const IplImage *const *pImage,
	const char *pPath,
	long pMillisecondInterval,
	const char *pFileSuffix):
mImage(pImage),
mPath(pPath),
mMillisecondInterval(pMillisecondInterval),
mLastSaveTimestamp(Time::MillisTimestamp() - mMillisecondInterval - 1),
mFileSuffix(pFileSuffix)
{
}

void SnapshotTaker::run()
{
	long tNow = Time::MillisTimestamp();
	if (tNow >= mLastSaveTimestamp + mMillisecondInterval)
	{
		time_t tTimestamp = time(0);
#ifdef __STDC_SECURE_LIB__
	    struct tm *tTime; 
		localtime_s(tTime, &tTimestamp);
#else
	    struct tm *tTime = localtime(&tTimestamp);
#endif
	    char tFormattedTimestamp[64];
#ifdef __STDC_SECURE_LIB__
	    sprintf_s(tFormattedTimestamp, "%d-%.2d-%.2d_%.2d-%.2d-%.2d.%.2ld",
	    		tTime->tm_year+1900, tTime->tm_mon+1, tTime->tm_mday,
	    		tTime->tm_hour, tTime->tm_min, tTime->tm_sec, tNow%1000l);
#else
	    sprintf(tFormattedTimestamp, "%d-%.2d-%.2d_%.2d-%.2d-%.2d.%.2ld",
	    		tTime->tm_year+1900, tTime->tm_mon+1, tTime->tm_mday,
	    		tTime->tm_hour, tTime->tm_min, tTime->tm_sec, tNow%1000l);
#endif

		std::stringstream tFilename;
		tFilename << mPath;
		tFilename << tFormattedTimestamp;
		tFilename << mFileSuffix;
		cvSaveImage(tFilename.str().c_str(), *mImage);
		mLastSaveTimestamp = tNow;
	}
}
