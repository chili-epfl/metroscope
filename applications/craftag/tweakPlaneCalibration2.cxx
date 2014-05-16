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
#include <opencv2/imgproc/imgproc_c.h>

int main(int argc, char *argv[]) {
	if (argc != 1+1+2*4)
	{
		std::cerr << "Usage: " << argv[0] << " <mapping file> <x-shift> <y-shift>\n";
		return -1;
	}

	auto tMap = (IplImage *) cvLoad(argv[1]);
    CvPoint2D32f from[8] = {
        {(float) 0        , (float)0},
        {(float) tMap->width, (float)0},
        {(float) tMap->width, (float)tMap->height},
        {(float) 0        , (float)tMap->height},
    };
    CvPoint2D32f to[8] = {
        {(float) 0        , (float)0},
        {(float) tMap->width, (float)0},
        {(float) tMap->width, (float)tMap->height},
        {(float) 0        , (float)tMap->height},
    };
    from[0].x += atof(argv[2]);
    from[0].y += atof(argv[3]);
    from[1].x += atof(argv[4]);
    from[1].y += atof(argv[5]);
    from[2].x += atof(argv[6]);
    from[2].y += atof(argv[7]);
    from[3].x += atof(argv[8]);
    from[3].y += atof(argv[9]);
    CvMat *transformation = cvCreateMat(3,3,CV_32F);
    cvGetPerspectiveTransform(from, to, transformation); 
    cvWarpPerspective(tMap, tMap, transformation);
    cvSave(argv[1], tMap);
}
