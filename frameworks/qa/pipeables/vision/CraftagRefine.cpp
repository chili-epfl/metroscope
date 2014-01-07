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

#include "CraftagRefine.hpp"
#include <qa/utils/Image.hpp>
#include <qa/utils/Math.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

//#define DEBUG_CraftagRefine
#ifdef DEBUG_CraftagRefine
#include <stdio.h>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#endif

namespace {
	const float scClose = 1.0f/10.0f;
	const float scFar = 1.0f-scClose;
	const CvScalar scWhite = CV_RGB(255,255,255);
	const CvScalar scBlack = CV_RGB(0, 0, 0);
	const int scNCornersInAQuad = 4;
	const float scMaxRefinement = 4.0f;

	CvRect findBoundingBox(const CvPoint2D32f *pCorners) {
		float tMinX = pCorners[0].x;
		float tMaxX = pCorners[0].x;
		float tMinY = pCorners[0].y;
		float tMaxY = pCorners[0].y;
		for (int i = 1; i < scNCornersInAQuad; ++i) {
			tMinX = MIN(tMinX, pCorners[i].x);
			tMaxX = MAX(tMaxX, pCorners[i].x);
			tMinY = MIN(tMinY, pCorners[i].y);
			tMaxY = MAX(tMaxY, pCorners[i].y);
		}
		return cvRect((int)tMinX, (int)tMinY, (int)(tMaxX-tMinX), (int)(tMaxY-tMinY));
	}

	CvRect findROI(const CvRect &pMinimalROI) {
		static const float scMarginRatio = scClose;
		int tXMargin = (int)(scMarginRatio*pMinimalROI.width);
		int tYMargin = (int)(scMarginRatio*pMinimalROI.height);
		return cvRect(pMinimalROI.x-tXMargin, pMinimalROI.y-tYMargin, pMinimalROI.width+2*tXMargin, pMinimalROI.width+2*tYMargin); //FIXME: effet de bord
	}
}

CraftagRefine::CraftagRefine(
    const IplImage *const *pInputImage,
    const int *pDecodedTag,
	CraftagRegistrar &pCraftagRegistrar):
mInputImage(pInputImage),
mDecodedTag(pDecodedTag),
mBoundingBox(cvRect(0,0,1,1)),
mROI(findROI(mBoundingBox)),
mInternalCorners(new CvPoint[scNCornersInAQuad]),
mROICopy(cvCreateImage(cvSize(mROI.width, mROI.height), (*mInputImage)->depth, (*mInputImage)->nChannels)),
mMask(cvCloneImage(mROICopy)),
mTempImg(cvCreateImage(cvGetSize(mROICopy), IPL_DEPTH_32F, 1)),
mEigenImg(cvCloneImage(mTempImg)),
mNCorners(scNCornersInAQuad),
mRefinedCorners(new CvPoint2D32f[mNCorners]),
mOrderedCorners(new CvPoint2D32f[mNCorners]),
mCraftagRegistrar(pCraftagRegistrar)
{
#ifdef DEBUG_CraftagRefine
	cvNamedWindow("CraftagRefine");
#endif
}

CraftagRefine::~CraftagRefine()
{
	delete []mOrderedCorners;
	delete []mRefinedCorners;
	cvReleaseImage(&mEigenImg);
	cvReleaseImage(&mTempImg);
	cvReleaseImage(&mMask);
	cvReleaseImage(&mROICopy);
	delete []mInternalCorners;
}

void CraftagRefine::run()
{
	int tDecodedTag = *mDecodedTag;
	if (tDecodedTag > -1)
	{
		IplImage * tInputImage = (IplImage *) *mInputImage; //const cast: just changing and restoring the ROI

		const CvPoint2D32f *tInputCorners = mCraftagRegistrar.getCorners(tDecodedTag);
		mNCorners = scNCornersInAQuad;
		mBoundingBox = findBoundingBox(tInputCorners);
		mROI = findROI(mBoundingBox);
		int tPreviousRoiX = mROI.x;
		int tPreviousRoiY = mROI.y;
		mROI.x = CV_IMAX(mROI.x, 0);
		mROI.y = CV_IMAX(mROI.y, 0);
		mROI.width -= mROI.x - tPreviousRoiX;
		mROI.height -= mROI.y - tPreviousRoiY;
		mROI.width = CV_IMIN(mROI.x+mROI.width, tInputImage->width)-mROI.x;
		mROI.height = CV_IMIN(mROI.y+mROI.height, tInputImage->height)-mROI.y;

		Image::matchImageFormats(mROI.width, mROI.height, tInputImage->depth, tInputImage->nChannels, &mROICopy);
		Image::matchImageFormats(mROICopy, &mMask);
		Image::matchImageFormats(mROICopy->width, mROICopy->height, IPL_DEPTH_32F, 1, &mTempImg);
		Image::matchImageFormats(mTempImg, &mEigenImg);

		//std::cout 
		//	<< tInputCorners[0].x << " - " <<  tInputCorners[0].y << " - " 
		//	<< tInputCorners[1].x << " - " <<  tInputCorners[1].y << " - "
		//	<< tInputCorners[2].x << " - " <<  tInputCorners[2].y << " - "
		//	<< tInputCorners[3].x << " - " <<  tInputCorners[3].y << 
		//std::endl;


		mRefinedCorners[0].x = (float)(tInputCorners[0].x - mROI.x);
		mRefinedCorners[0].y = (float)(tInputCorners[0].y - mROI.y);
		mRefinedCorners[1].x = (float)(tInputCorners[1].x - mROI.x);
		mRefinedCorners[1].y = (float)(tInputCorners[1].y - mROI.y);
		mRefinedCorners[2].x = (float)(tInputCorners[2].x - mROI.x);
		mRefinedCorners[2].y = (float)(tInputCorners[2].y - mROI.y);
		mRefinedCorners[3].x = (float)(tInputCorners[3].x - mROI.x);
		mRefinedCorners[3].y = (float)(tInputCorners[3].y - mROI.y);

		CvRect tOriginalROI = cvGetImageROI(tInputImage);
		cvSetImageROI(tInputImage, mROI);
		cvRectangle(mMask, cvPoint(0,0), cvPoint(mMask->width, mMask->height), scWhite, CV_FILLED);
		mInternalCorners[0].x = (int)(mRefinedCorners[0].x*scClose + mRefinedCorners[2].x*scFar);
		mInternalCorners[0].y = (int)(mRefinedCorners[0].y*scClose + mRefinedCorners[2].y*scFar);
		mInternalCorners[1].x = (int)(mRefinedCorners[1].x*scClose + mRefinedCorners[3].x*scFar);
		mInternalCorners[1].y = (int)(mRefinedCorners[1].y*scClose + mRefinedCorners[3].y*scFar);
		mInternalCorners[2].x = (int)(mRefinedCorners[2].x*scClose + mRefinedCorners[0].x*scFar);
		mInternalCorners[2].y = (int)(mRefinedCorners[2].y*scClose + mRefinedCorners[0].y*scFar);
		mInternalCorners[3].x = (int)(mRefinedCorners[3].x*scClose + mRefinedCorners[1].x*scFar);
		mInternalCorners[3].y = (int)(mRefinedCorners[3].y*scClose + mRefinedCorners[1].y*scFar);
		cvFillPoly(mMask, &mInternalCorners, &scNCornersInAQuad, 1, scBlack);
		cvZero(mROICopy);
		cvCopy(tInputImage, mROICopy, mMask);
		cvSetImageROI(tInputImage, tOriginalROI);

		cvGoodFeaturesToTrack(mROICopy, mEigenImg, mTempImg, mRefinedCorners, &mNCorners, 0.1, 20);
		cvFindCornerSubPix(mROICopy, mRefinedCorners, mNCorners, cvSize(5, 5), cvSize(-1, -1), cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 100, 0.000001));

		mRefinedCorners[0].x += mROI.x;
		mRefinedCorners[0].y += mROI.y;
		mRefinedCorners[1].x += mROI.x;
		mRefinedCorners[1].y += mROI.y;
		mRefinedCorners[2].x += mROI.x;
		mRefinedCorners[2].y += mROI.y;
		mRefinedCorners[3].x += mROI.x;
		mRefinedCorners[3].y += mROI.y;

		for (int i = 0; i < scNCornersInAQuad; ++i) {
			int tClosestCornerIndex = 0;
			float tClosestCornerDist = Math::squaredDist((float)tInputCorners[i].x, (float)tInputCorners[i].y, mRefinedCorners[0].x, mRefinedCorners[0].y);
			for (int j = 1; j < mNCorners; ++j) {
				float tDist = Math::squaredDist((float)tInputCorners[i].x, (float)tInputCorners[i].y, mRefinedCorners[j].x, mRefinedCorners[j].y);
				if (tDist < tClosestCornerDist)
				{
					tClosestCornerIndex = j;
					tClosestCornerDist = tDist;
				}
			}
		mOrderedCorners[i] = (tClosestCornerDist<scMaxRefinement)? mRefinedCorners[tClosestCornerIndex] : tInputCorners[i];
#ifdef DEBUG_CraftagRefine
			std::cout << ((tClosestCornerDist<scMaxRefinement)? "." : "*") ;
		}
		std::cout << std::endl;
#else
		}
#endif

#ifdef DEBUG_CraftagRefine
		for(int i=0; i<mNCorners; ++i)
		{
			cvCircle(tInputImage, cvPointFrom32f(tInputCorners[i]), 3, cvScalarAll(128), 2);
			cvLine(tInputImage, cvPointFrom32f(mOrderedCorners[i]), cvPointFrom32f(mOrderedCorners[i]), scWhite, 5);
			printf("%1.1f  %1.1f        ", mOrderedCorners[i].x, mOrderedCorners[i].y);
		}
		printf("\n");
		//cvShowImage("CraftagRefine", tInputImage);
		//cvMoveWindow("CraftagRefine", 1600, 0);
		//cvWaitKey(5);
#endif

		mCraftagRegistrar.set(tDecodedTag, mOrderedCorners);
	}

}
