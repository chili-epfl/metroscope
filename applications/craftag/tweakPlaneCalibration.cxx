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

#include <iostream>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char *argv[]) {
	if (argc != 1+3)
	{
		std::cerr << "Usage: " << argv[0] << " <mapping file> <x-shift> <y-shift>";
		return -1;
	}

	auto tMap = (IplImage *) cvLoad(argv[1]);
	float tXShift = atof(argv[2]);
	float tYShift = atof(argv[3]);
	cvAddS(tMap, cvScalar(tXShift, tYShift), tMap);
	cvSave(argv[1], tMap);
	std::cerr << "Added (" << tXShift << "," << tYShift << ") to " << argv[1] << std::endl;
}
