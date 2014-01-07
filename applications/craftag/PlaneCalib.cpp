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

#include "PlaneCalib.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <wykobi/wykobi.hpp>

#define DEBUG_PlaneCalibration
#ifdef DEBUG_PlaneCalibration
#include <opencv2/highgui/highgui.hpp>
static const char *scDebugWindowName = "DEBUG_PlaneCalibration";
#endif

PlaneCalibration::PlaneCalibration(
    	int pSquareCols,
    	int pSquareRows,
    	int pMaxSampleCount,
        const IplImage *const *pInputImage):
cCornerArraySize(cvSize(pSquareCols-1, pSquareRows-1)),
cExpectedCornerCount(cCornerArraySize.width*cCornerArraySize.height),
mSampleCount(0),
mMaxSampleCount(pMaxSampleCount),
mInputImage(pInputImage),
mFoundCorners(new CvPoint2D32f[cExpectedCornerCount]),
mSumCorners(new CvPoint2D32f[cExpectedCornerCount])
{
	for (int i = 0; i < cExpectedCornerCount; ++i) {
		mSumCorners[i].x = 0.0f;
		mSumCorners[i].y = 0.0f;
	}
}

PlaneCalibration::~PlaneCalibration()
{
	delete [] mSumCorners;
	delete [] mFoundCorners;
}

void PlaneCalibration::run()
{
	int tFoundCornerCount;
	cvFindChessboardCorners( *mInputImage, cCornerArraySize, mFoundCorners, &tFoundCornerCount, CV_CALIB_CB_ADAPTIVE_THRESH );

#ifdef DEBUG_PlaneCalibration
	cvShowImage(scDebugWindowName, *mInputImage);
	cvWaitKey(5);
#endif

	if (tFoundCornerCount == cExpectedCornerCount && mSampleCount < mMaxSampleCount)
	{
		cvFindCornerSubPix(*mInputImage, mFoundCorners, cExpectedCornerCount, cvSize( 5, 5 ), cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_EPS, 0, 0.01 ));
		if (foundChessboardIsGood()) addFoundChessboard();
	}

	std::cout << mSampleCount << "/" << mMaxSampleCount << " samples taken." << std::endl;
	if (mSampleCount >= mMaxSampleCount) stop();
}

bool PlaneCalibration::foundChessboardIsGood() const
{
	float tXMax = mFoundCorners[0].x;
	float tYMax = mFoundCorners[0].y;
	float tXMin = mFoundCorners[0].x;
	float tYMin = mFoundCorners[0].y;
	for (int i = 1; i < cExpectedCornerCount; ++i) {
		tXMax = MAX(tXMax, mFoundCorners[i].x);
		tYMax = MAX(tYMax, mFoundCorners[i].y);
		tXMin = MIN(tXMin, mFoundCorners[i].x);
		tYMin = MIN(tYMin, mFoundCorners[i].y);
	}

	const float cStandardArea = (tXMax - tXMin)*(tYMax - tYMin)/(cCornerArraySize.width*cCornerArraySize.height);
	const float cMinArea = 0.8f*cStandardArea;
	const float cMaxArea = 1.2f*cStandardArea;

	for (int y = 0; y < cCornerArraySize.height-1; ++y)
	{
		for (int x = 0; x < cCornerArraySize.width-1; ++x)
		{
			CvPoint2D32f tCvCorners[4];
			tCvCorners[0] = mFoundCorners[( y )*cCornerArraySize.width +  x ];
			tCvCorners[1] = mFoundCorners[( y )*cCornerArraySize.width + x+1];
			tCvCorners[2] = mFoundCorners[(y+1)*cCornerArraySize.width + x+1];
			tCvCorners[3] = mFoundCorners[(y+1)*cCornerArraySize.width +  x ];

			wykobi::quadix<float, 2> tCorners;
			for (int i = 0; i < 4; ++i) {
				tCorners[i].x = tCvCorners[i].x;
				tCorners[i].y = tCvCorners[i].y;
			}
			bool tDegenerate = wykobi::is_degenerate(tCorners);
			float tArea = wykobi::area(tCorners);
			bool tAreaOutOfRange = tArea < cMinArea || tArea > cMaxArea;
			if (tDegenerate || tAreaOutOfRange)
			{
#ifdef DEBUG_PlaneCalibration
				if (tDegenerate) std::cout << "[degenerate] ";
				if (tAreaOutOfRange) std::cout << "[area(" << tArea <<" not in [" << cMinArea << ", " << cMaxArea <<"])] ";
				std::cout << x << "," << y << ": ";
				for (int i = 0; i < 4; ++i) {
					std::cout << tCvCorners[i].x << ", " << tCvCorners[i].y << "\t";
				}
				std::cout << std::endl;
#endif
				return false;
			}
		}
	}

	return true;
}

void PlaneCalibration::addFoundChessboard()
{
	bool tHFlip = mFoundCorners[0].x > mFoundCorners[cExpectedCornerCount-1].x;
	bool tVFlip = mFoundCorners[0].y > mFoundCorners[cExpectedCornerCount-1].y;
	if (tHFlip && tVFlip)
	{
		for (int i = 0; i < cExpectedCornerCount; ++i) {
			mSumCorners[i].x += mFoundCorners[cExpectedCornerCount-1-i].x;
			mSumCorners[i].y += mFoundCorners[cExpectedCornerCount-1-i].y;
		}
	}
	else if (!tHFlip && !tVFlip)
	{
		for (int i = 0; i < cExpectedCornerCount; ++i) {
			mSumCorners[i].x += mFoundCorners[i].x;
			mSumCorners[i].y += mFoundCorners[i].y;
		}
	}
	else
	{
		std::cerr << "Chessboard orientation not implemented" << std::endl;
	}
	++mSampleCount;
}

void PlaneCalibration::Write(const char *pFileName, float pSquareSize, float pScale, PlaneCoordinateMapper *pCamPx2WorldMm) const
{
	for (int i = 0; i < cExpectedCornerCount; ++i) {
		mSumCorners[i].x /= mMaxSampleCount;
		mSumCorners[i].y /= mMaxSampleCount;
		if (pCamPx2WorldMm)
		{
			pCamPx2WorldMm->InterpolatedMap(mSumCorners[i]);
		}
	}

	//maximum dimension
	int tWidth = pCamPx2WorldMm?800:(*mInputImage)->width;
	int tHeight = pCamPx2WorldMm?600:(*mInputImage)->height;
	tWidth *= pScale;
	tHeight *= pScale;
	IplImage *tMap = cvCreateImage(cvSize(tWidth, tHeight), IPL_DEPTH_32F, 2);
	cvSet(tMap, cvScalar(-1.0));
	IplImage *tCoordinates = cvCreateImage(cvSize(2,2), IPL_DEPTH_32F, 2);

#ifdef DEBUG_PlaneCalibration
	cvNamedWindow(scDebugWindowName);
	cvMoveWindow(scDebugWindowName, 0, 0);
	IplImage *tXMap = cvCreateImage(cvGetSize(tMap), tMap->depth, 1);
	IplImage *tYMap = cvCloneImage(tXMap);
	IplImage *tDisplay = cvCreateImage(cvGetSize(tXMap), tXMap->depth, 3);
	IplImage *tOnes = cvCloneImage(tXMap);
	cvSet(tOnes, cvScalar(1.0));

	float tXMax = mSumCorners[0].x;
	float tYMax = mSumCorners[0].y;
	float tXMin = mSumCorners[0].x;
	float tYMin = mSumCorners[0].y;
	for (int i = 1; i < cExpectedCornerCount; ++i) {
		tXMax = MAX(tXMax, mSumCorners[i].x);
		tYMax = MAX(tYMax, mSumCorners[i].y);
		tXMin = MIN(tXMin, mSumCorners[i].x);
		tYMin = MIN(tYMin, mSumCorners[i].y);
	}
	std::cout << "XMin=" << tXMin << ", XMax=" << tXMax << ", YMin=" << tYMin << ", YMax=" << tYMax << std::endl;
	tXMax *= 1.2f;
	tYMax *= 1.2f;
	tXMin *= 0.8f;
	tYMin *= 0.8f;
#endif

	CvMat *mTransformation = cvCreateMat(3, 3, CV_32FC1);
	CvPoint2D32f mSrcCorners[4];
	mSrcCorners[0].x = 0; mSrcCorners[0].y = 0;
	mSrcCorners[1].x = 1; mSrcCorners[1].y = 0;
	mSrcCorners[2].x = 1; mSrcCorners[2].y = 1;
	mSrcCorners[3].x = 0; mSrcCorners[3].y = 1;
	CvPoint2D32f tDstCorners[4];

#ifndef DEBUG_PlaneCalibration
	int tProgress = 0;
#endif

	// we extrapolate one step in each direction,
	// mainly for the projection calibration which lacks the borders of the screen
	for (int y = -1; y < cCornerArraySize.height; ++y)
	{
		for (int x = -1; x < cCornerArraySize.width; ++x)
		{
			getDestinationCorners(x, y, tDstCorners, pScale);
			cvGetPerspectiveTransform(mSrcCorners, tDstCorners, mTransformation );

			//everything is shifted by 1, since only the internal corners of the chessboard are detected
			cvSet2D(tCoordinates, 0, 0, cvScalar(pSquareSize*(x+1), pSquareSize*(y+1)));
			cvSet2D(tCoordinates, 0, 1, cvScalar(pSquareSize*(x+2), pSquareSize*(y+1)));
			cvSet2D(tCoordinates, 1, 1, cvScalar(pSquareSize*(x+2), pSquareSize*(y+2)));
			cvSet2D(tCoordinates, 1, 0, cvScalar(pSquareSize*(x+1), pSquareSize*(y+2)));

			cvWarpPerspective(tCoordinates, tMap, mTransformation, CV_INTER_LINEAR);

#ifdef DEBUG_PlaneCalibration
			cvSplit(tMap, tXMap, 0, 0, 0);
			cvMul(tXMap, tOnes, tXMap, 1.0f/tXMax);
			cvSplit(tMap, 0, tYMap, 0, 0);
			cvMul(tYMap, tOnes, tYMap, 1.0f/tYMax);
			cvMerge(tXMap, 0, tYMap, 0, tDisplay);
			cvShowImage(scDebugWindowName, tDisplay);
			cvWaitKey(5);
#else
			std::cout << ++tProgress << "/" << (cCornerArraySize.width+1)*(cCornerArraySize.height+1) << std::endl;
#endif
		}
	}

#ifdef DEBUG_PlaneCalibration
	cvReleaseImage(&tOnes);
	cvReleaseImage(&tDisplay);
	cvReleaseImage(&tYMap);
	cvReleaseImage(&tXMap);
	cvDestroyWindow(scDebugWindowName);
#endif

	removeSaltAndPepper(tMap);

	cvSave(pFileName, tMap);

	cvReleaseMat(&mTransformation);
	cvReleaseImage(&tCoordinates);
	cvReleaseImage(&tMap);
}

void PlaneCalibration::getDestinationCorners(int x, int y, CvPoint2D32f *pDstCorners, float pScale) const
{
	if (x < 0)
	{
		if (y < 0)
		{
			extrapolateCorner(x, y, -1, -1, pDstCorners[0]);
			extrapolateCorner(x+1, y, 0, -1, pDstCorners[1]);
			pDstCorners[2] = mSumCorners[(y+1)*cCornerArraySize.width + x+1];
			extrapolateCorner(x, y+1, -1, 0, pDstCorners[3]);
		}
		else if (y >= cCornerArraySize.height-1)
		{
			extrapolateCorner(x, y, -1, 0, pDstCorners[0]);
			pDstCorners[1] = mSumCorners[( y )*cCornerArraySize.width + x+1];
			extrapolateCorner(x+1, y+1, 0, 1, pDstCorners[2]);
			extrapolateCorner(x, y+1, -1, 1, pDstCorners[3]);
		}
		else
		{
			extrapolateCorner(x, y, -1, 0, pDstCorners[0]);
			pDstCorners[1] = mSumCorners[( y )*cCornerArraySize.width + x+1];
			pDstCorners[2] = mSumCorners[(y+1)*cCornerArraySize.width + x+1];
			extrapolateCorner(x, y+1, -1, 0, pDstCorners[3]);
		}
	}
	else if (x >= cCornerArraySize.width-1)
	{
		if (y < 0)
		{
			extrapolateCorner(x, y, 0, -1, pDstCorners[0]);
			extrapolateCorner(x+1, y, 1, -1, pDstCorners[1]);
			extrapolateCorner(x+1, y+1, 1, 0, pDstCorners[2]);
			pDstCorners[3] = mSumCorners[(y+1)*cCornerArraySize.width +  x ];
		}
		else if (y >= cCornerArraySize.height-1)
		{
			pDstCorners[0] = mSumCorners[( y )*cCornerArraySize.width +  x ];
			extrapolateCorner(x+1, y, 1, 0, pDstCorners[1]);
			extrapolateCorner(x+1, y+1, 1, 1, pDstCorners[2]);
			extrapolateCorner(x, y+1, 0, 1, pDstCorners[3]);
		}
		else
		{
			pDstCorners[0] = mSumCorners[( y )*cCornerArraySize.width +  x ];
			extrapolateCorner(x+1, y, 1, 0, pDstCorners[1]);
			extrapolateCorner(x+1, y+1, 1, 0, pDstCorners[2]);
			pDstCorners[3] = mSumCorners[(y+1)*cCornerArraySize.width +  x ];
		}
	}
	else
	{
		if (y < 0)
		{
			extrapolateCorner(x, y, 0, -1, pDstCorners[0]);
			extrapolateCorner(x+1, y, 0, -1, pDstCorners[1]);
			pDstCorners[2] = mSumCorners[(y+1)*cCornerArraySize.width + x+1];
			pDstCorners[3] = mSumCorners[(y+1)*cCornerArraySize.width +  x ];
		}
		else if (y >= cCornerArraySize.height-1)
		{
			pDstCorners[0] = mSumCorners[( y )*cCornerArraySize.width +  x ];
			pDstCorners[1] = mSumCorners[( y )*cCornerArraySize.width + x+1];
			extrapolateCorner(x+1, y+1, 0, 1, pDstCorners[2]);
			extrapolateCorner(x, y+1, 0, 1, pDstCorners[3]);
		}
		else
		{
			pDstCorners[0] = mSumCorners[( y )*cCornerArraySize.width +  x ];
			pDstCorners[1] = mSumCorners[( y )*cCornerArraySize.width + x+1];
			pDstCorners[2] = mSumCorners[(y+1)*cCornerArraySize.width + x+1];
			pDstCorners[3] = mSumCorners[(y+1)*cCornerArraySize.width +  x ];
		}
	}


	pDstCorners[0].x *= pScale; pDstCorners[0].y *= pScale;
	pDstCorners[1].x *= pScale; pDstCorners[1].y *= pScale;
	pDstCorners[2].x *= pScale; pDstCorners[2].y *= pScale;
	pDstCorners[3].x *= pScale; pDstCorners[3].y *= pScale;
}

void PlaneCalibration::extrapolateCorner(int x, int y, char pXDirection, char pYDirection, CvPoint2D32f &pResult) const
{
	x = MIN(MAX(0,x),cCornerArraySize.width-1);
	y = MIN(MAX(0,y),cCornerArraySize.height-1);
	CvPoint2D32f tBorderPoint = mSumCorners[( y )*cCornerArraySize.width +  x ];
	pResult = tBorderPoint;
	if (pXDirection)
	{
		CvPoint2D32f tInsidePoint = mSumCorners[( y )*cCornerArraySize.width +  x-pXDirection ];
		pResult.x += (tBorderPoint.x-tInsidePoint.x);
		pResult.y += (tBorderPoint.y-tInsidePoint.y);
	}
	if (pYDirection)
	{
		CvPoint2D32f tInsidePoint = mSumCorners[( y-pYDirection )*cCornerArraySize.width +  x ];
		pResult.x += (tBorderPoint.x-tInsidePoint.x);
		pResult.y += (tBorderPoint.y-tInsidePoint.y);
	}
}

void PlaneCalibration::removeSaltAndPepper(IplImage *pMap) const
{
#ifdef DEBUG_PlaneCalibration
	IplImage *tXMap = cvCreateImage(cvGetSize(pMap), pMap->depth, 1);
	IplImage *tYMap = cvCloneImage(tXMap);
	IplImage *tDisplay = cvCreateImage(cvGetSize(tXMap), tXMap->depth, 3);
	IplImage *tOnes = cvCloneImage(tXMap);
	cvSet(tOnes, cvScalar(1.0));

	float tXMax = mSumCorners[0].x;
	float tYMax = mSumCorners[0].y;
	float tXMin = mSumCorners[0].x;
	float tYMin = mSumCorners[0].y;
	for (int i = 1; i < cExpectedCornerCount; ++i) {
		tXMax = MAX(tXMax, mSumCorners[i].x);
		tYMax = MAX(tYMax, mSumCorners[i].y);
		tXMin = MIN(tXMin, mSumCorners[i].x);
		tYMin = MIN(tYMin, mSumCorners[i].y);
	}
	std::cout << "XMin=" << tXMin << ", XMax=" << tXMax << ", YMin=" << tYMin << ", YMax=" << tYMax << std::endl;
	tXMax *= 1.2f;
	tYMax *= 1.2f;
	tXMin *= 0.8f;
	tYMin *= 0.8f;
	cvSplit(pMap, tXMap, 0, 0, 0);
	cvMul(tXMap, tOnes, tXMap, 1.0f/tXMax);
	cvSplit(pMap, 0, tYMap, 0, 0);
	cvMul(tYMap, tOnes, tYMap, 1.0f/tYMax);
	cvMerge(tXMap, 0, tYMap, 0, tDisplay);
	cvShowImage(scDebugWindowName, tDisplay);
	cvWaitKey(0);
#endif

	int tHeight = pMap->height;
	int tWidth = pMap->width;
	int tStep = pMap->widthStep/sizeof(float);
	int tChannels = pMap->nChannels;
	float *tData = (float *) pMap->imageData;

	for(int i=1;i<tHeight-1;i++) for(int j=1;j<tWidth-1;j++) for(int k=0;k<tChannels;k++)
	{
		float tValue = tData[i*tStep+j*tChannels+k];

		float tTop = tData[i*tStep+(j-1)*tChannels+k];
		float tDown = tData[i*tStep+(j+1)*tChannels+k];
		float tRight = tData[(i+1)*tStep+j*tChannels+k];
		float tLeft = tData[(i-1)*tStep+j*tChannels+k];

		if (tTop >= 0.0f && tDown >= 0.0f && abs((tTop-tDown)) < 0.1f*tDown)
		{
			float tExpectation = (tTop+tDown)/2.0f;
			if (abs((tExpectation-tValue)) > 0.1f*tExpectation)
			{
				tData[i*tStep+j*tChannels+k] = tExpectation;
			} else if (tRight >= 0.0f && tLeft >= 0.0f && abs((tRight-tLeft)) < 0.1f*tLeft)
			{
				float tExpectation = (tRight+tLeft)/2.0f;
				if (abs((tExpectation-tValue)) > 0.1f*tExpectation)
				{
					tData[i*tStep+j*tChannels+k] = tExpectation;
				}
			}
		}
		else if (tRight >= 0.0f && tLeft >= 0.0f && abs((tRight-tLeft)) < 0.1f*tLeft)
		{
			float tExpectation = (tRight+tLeft)/2.0f;
			if (abs((tExpectation-tValue)) > 0.1f*tExpectation)
			{
				tData[i*tStep+j*tChannels+k] = tExpectation;
			}
		}
	}

#ifdef DEBUG_PlaneCalibration
	cvSplit(pMap, tXMap, 0, 0, 0);
	cvMul(tXMap, tOnes, tXMap, 1.0f/tXMax);
	cvSplit(pMap, 0, tYMap, 0, 0);
	cvMul(tYMap, tOnes, tYMap, 1.0f/tYMax);
	cvMerge(tXMap, 0, tYMap, 0, tDisplay);
	cvShowImage(scDebugWindowName, tDisplay);
	cvWaitKey(0);
	cvReleaseImage(&tOnes);
	cvReleaseImage(&tDisplay);
	cvReleaseImage(&tYMap);
	cvReleaseImage(&tXMap);
#endif
}
