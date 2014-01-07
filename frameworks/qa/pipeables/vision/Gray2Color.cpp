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

#include "Gray2Color.hpp"
#include <qa/utils/Image.hpp>
#include <opencv2/opencv.hpp>

Gray2Color::Gray2Color(
    const IplImage *const *pInputImage):
mInputImage(pInputImage),
mConversionCode(CV_GRAY2BGR),
mOutputImage(cvCreateImage(cvGetSize(*mInputImage), (*mInputImage)->depth, 3))
{
}

Gray2Color::~Gray2Color()
{
	cvReleaseImage(&mOutputImage);
}

void Gray2Color::run()
{
	const IplImage *const tInputImage = *mInputImage;
    Image::matchImageFormats(tInputImage, &mOutputImage, true);
    cvCvtColor(tInputImage, mOutputImage, mConversionCode);
}
