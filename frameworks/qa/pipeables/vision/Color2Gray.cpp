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

#include "Color2Gray.hpp"
#include <qa/utils/Image.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

Color2Gray::Color2Gray(
    const IplImage *const *pInputImage):
mInputImage(pInputImage),
mConversionCode(CV_BGR2GRAY),
mOutputImage(cvCreateImage(cvGetSize(*mInputImage), (*mInputImage)->depth, 1))
{
}

Color2Gray::~Color2Gray()
{
	cvReleaseImage(&mOutputImage);
}

void Color2Gray::run()
{
	const IplImage *const tInputImage = *mInputImage;
	Image::matchImageFormats(tInputImage, &mOutputImage, true);
    cvCvtColor(tInputImage, mOutputImage, mConversionCode);
}
