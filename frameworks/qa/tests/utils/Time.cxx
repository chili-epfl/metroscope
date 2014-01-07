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

#include <qa/utils/Tests.hpp>
#include <qa/utils/Time.hpp>
#include <iostream>

int main()
{
    Tests::TestSection("Time");
    const long tSleepTime = 25l;
    const int tTests = 1000;
    int tFails = 0;
    for (int i = 0; i < tTests; ++i) {
		long tBefore = Time::MillisTimestamp();
		Time::Sleep(tSleepTime);
		long tAfter = Time::MillisTimestamp();
		if (tAfter-tBefore != tSleepTime) ++tFails;
	}
	Tests::verifyLessThan(tFails, tTests/5);
}
