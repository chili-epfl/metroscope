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

#include "Pipeable.hpp"
#include <iostream>
#include "../utils/Time.hpp"
#include <vector>

void Pipeable::start()
{
    for (Pipeable *tCurrent = this; tCurrent != 0; tCurrent = tCurrent->mNext)
    {
        tCurrent->run();
    }
}

void Pipeable::startDebug()
{
	int i = 0;
    for (Pipeable *tCurrent = this; tCurrent != 0; tCurrent = tCurrent->mNext)
    {
    	std::cout << i << std::endl;
        tCurrent->run();
		std::cout << i << std::endl;
		i=(tCurrent->mNext == this)?0:i+1;
    }
}

void Pipeable::startProfile()
{
	std::vector<long> tTimes;
    for (Pipeable *tCurrent = this; tCurrent != 0; tCurrent = tCurrent->mNext)
    {
		long tMillisTimestampBefore = Time::MillisTimestamp();
        tCurrent->run();
    	long tMillisTimestampAfter = Time::MillisTimestamp();
		tTimes.push_back(tMillisTimestampAfter-tMillisTimestampBefore);
		if (tCurrent->mNext == this)
		{
			for (std::vector<long>::iterator it = tTimes.begin(); it != tTimes.end(); ++it)
				std::cout << *it << " ";
			std::cout << std::endl;
			tTimes.clear();
		}
    }

	for (std::vector<long>::iterator it = tTimes.begin(); it != tTimes.end(); ++it)
		std::cout << *it << " ";
	std::cout << std::endl;
}

void Pipeable::startNoWait()
{
#ifdef WIN32
	mThread = boost::thread( boost::bind( &Pipeable::start, this ) );
#else
	pthread_create(&mThread, 0, &Pipeable::startThread, this);
#endif
}

void Pipeable::join()
{
#ifdef WIN32
	mThread.join();
#else
	pthread_join(mThread, 0);
#endif
}
