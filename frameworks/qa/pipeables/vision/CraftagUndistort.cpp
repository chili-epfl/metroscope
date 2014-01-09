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

#include "CraftagUndistort.hpp"
#include <opencv2/opencv.hpp>

//#define DEBUG_CraftagUndistort
#ifdef DEBUG_CraftagUndistort
#include <opencv2/highgui/highgui.hpp>
#include <wykobi/wykobi_utilities.hpp>
#include <iostream>
using std::cout;
using std::endl;
#endif

namespace {
	const int scTagMargin = 2;
	const int scDataSize = 6;
	const int scTagSize = scDataSize+2*scTagMargin;
	const float scClose = scTagMargin/(float) scTagSize;
	const float scFar = 1.0f - scClose;
	const int scTagWarpZoom = 16;
}

CraftagUndistort::CraftagUndistort(
	const IplImage *const *pInputImage,
	const wykobi::quadix2d *pCorners):
mInputImage(pInputImage),
mCorners(pCorners),
mTransformation(cvCreateMat(3, 3, CV_32FC1)),
mUndistortedTag(cvCreateImage(cvSize(scTagWarpZoom*scDataSize,scTagWarpZoom*scDataSize), (*pInputImage)->depth, (*pInputImage)->nChannels))
{
	mDstBoundaries[0].x = 0.0f;
	mDstBoundaries[0].y = 0.0f;
	mDstBoundaries[1].x = (float) mUndistortedTag->width;
	mDstBoundaries[1].y = 0.0f;
	mDstBoundaries[2].x = (float) mUndistortedTag->width;
	mDstBoundaries[2].y = (float) mUndistortedTag->height;
	mDstBoundaries[3].x = 0.0f;
	mDstBoundaries[3].y = (float) mUndistortedTag->height;

#ifdef DEBUG_CraftagUndistort
	cvNamedWindow("CraftagUndistort");
#endif
}

CraftagUndistort::~CraftagUndistort()
{
	cvReleaseImage(&mUndistortedTag);
	cvReleaseMat(&mTransformation);
}


void CraftagUndistort::run()
{
	mSrcBoundaries[2].x = (*mCorners)[0].x*scClose + (*mCorners)[2].x*scFar;
	mSrcBoundaries[2].y = (*mCorners)[0].y*scClose + (*mCorners)[2].y*scFar;
	mSrcBoundaries[3].x = (*mCorners)[1].x*scClose + (*mCorners)[3].x*scFar;
	mSrcBoundaries[3].y = (*mCorners)[1].y*scClose + (*mCorners)[3].y*scFar;
	mSrcBoundaries[0].x = (*mCorners)[2].x*scClose + (*mCorners)[0].x*scFar;
	mSrcBoundaries[0].y = (*mCorners)[2].y*scClose + (*mCorners)[0].y*scFar;
	mSrcBoundaries[1].x = (*mCorners)[3].x*scClose + (*mCorners)[1].x*scFar;
	mSrcBoundaries[1].y = (*mCorners)[3].y*scClose + (*mCorners)[1].y*scFar;

	cvGetPerspectiveTransform(mSrcBoundaries, mDstBoundaries, mTransformation );
	cvWarpPerspective(*mInputImage, mUndistortedTag, mTransformation, CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, cvScalar(0, 0, 0));

#ifdef DEBUG_CraftagUndistort
	cout << *mCorners << endl;
	cvShowImage("CraftagUndistort", mUndistortedTag);
	cvWaitKey(0);
#endif
}
