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

#include "PerspectiveWarper.hpp"

#include <opencv2/opencv.hpp>
#include <qa/utils/CvWykobiBridge.hpp>

PerspectiveWarper::PerspectiveWarper(
	const IplImage *const *pInputImage,
    const wykobi::quadix<float, 2> *pSelection,
    int pWidth, int pHeight):
mInputImage(pInputImage),
mSelection(pSelection),
mDistortionMatrix(cvCreateMat(3,3,CV_32FC1)),
mWarpedImage(cvCreateImage(cvSize(pWidth,pHeight),(*pInputImage)->depth, (*pInputImage)->nChannels))
{
	mDst[0].x = 0.0f; mDst[0].y = 0.0f;
	mDst[1].x = pWidth; mDst[1].y = 0.0f;
	mDst[2].x = pWidth; mDst[2].y = pHeight;
	mDst[3].x = 0.0f; mDst[3].y = pHeight;
}

PerspectiveWarper::~PerspectiveWarper()
{
	cvReleaseMat(&mDistortionMatrix);
	cvReleaseImage(&mWarpedImage);
}

void PerspectiveWarper::run()
{
	CvWykobiBridge::convertQuad(*mSelection, mSrc);
    cvGetPerspectiveTransform(mSrc, mDst, mDistortionMatrix);
    cvWarpPerspective(*mInputImage, mWarpedImage, mDistortionMatrix);
}
