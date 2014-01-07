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

#include "Undistorter.hpp"
#include <qa/utils/Image.hpp>
#include <iostream>

Undistorter::Undistorter(
	float pCameraFx,
	float pCameraFy,
	float pCameraCx,
	float pCameraCy,
	float pCameraAlpha,
	float pCameraK1,
	float pCameraK2,
	float pCameraK3,
	float pCameraK4,
	const IplImage *const *pInputImage
	):
mCameraFx(pCameraFx),
mCameraFy(pCameraFy),
mCameraCx(pCameraCx),
mCameraCy(pCameraCy),
mCameraAlpha(pCameraAlpha),
mCameraK1(pCameraK1),
mCameraK2(pCameraK2),
mCameraK3(pCameraK3),
mCameraK4(pCameraK4),
mInputImage(pInputImage)
{
    int tWidth = 1;
	int tHeight = 1;

	mCameraMapx = cvCreateMat(tHeight, tWidth, CV_32FC1);
	mCameraMapy = cvCreateMat(tHeight, tWidth, CV_32FC1);

	mCameraIntrinsicParameters = cvCreateMat(3, 3, CV_32F);
	mCameraDistortionCoefficients = cvCreateMat(1, 4, CV_32F);

	mUndistortedImage = cvCreateImage(cvSize(tWidth,tHeight),IPL_DEPTH_8U,1);

	CV_MAT_ELEM(*mCameraIntrinsicParameters, float, 0, 0) = mCameraFx;
	CV_MAT_ELEM(*mCameraIntrinsicParameters, float, 0, 1) = mCameraFx*mCameraAlpha;
	CV_MAT_ELEM(*mCameraIntrinsicParameters, float, 0, 2) = mCameraCx;
	CV_MAT_ELEM(*mCameraIntrinsicParameters, float, 1, 0) = 0.0f;
	CV_MAT_ELEM(*mCameraIntrinsicParameters, float, 1, 1) = mCameraFy;
	CV_MAT_ELEM(*mCameraIntrinsicParameters, float, 1, 2) = mCameraCy;
	CV_MAT_ELEM(*mCameraIntrinsicParameters, float, 2, 0) = 0.0f;
	CV_MAT_ELEM(*mCameraIntrinsicParameters, float, 2, 1) = 0.0f;
	CV_MAT_ELEM(*mCameraIntrinsicParameters, float, 2, 2) = 1.0f;

	CV_MAT_ELEM(*mCameraDistortionCoefficients, float, 0, 0) = mCameraK1;
	CV_MAT_ELEM(*mCameraDistortionCoefficients, float, 0, 1) = mCameraK2;
	CV_MAT_ELEM(*mCameraDistortionCoefficients, float, 0, 2) = mCameraK3;
	CV_MAT_ELEM(*mCameraDistortionCoefficients, float, 0, 3) = mCameraK4;

	cvInitUndistortMap(mCameraIntrinsicParameters,mCameraDistortionCoefficients, mCameraMapx, mCameraMapy);
}

Undistorter::~Undistorter(){

	cvReleaseMat(&mCameraIntrinsicParameters);
	cvReleaseMat(&mCameraDistortionCoefficients);
	cvReleaseMat(&mCameraMapx);
	cvReleaseMat(&mCameraMapy);
	cvReleaseImage(&mUndistortedImage);
}

void Undistorter::run(){

	const IplImage *tInputImage = *mInputImage;

	if (Image::matchImageFormats(tInputImage, &mUndistortedImage, false)){
		cvReleaseMat(&mCameraMapx);
		mCameraMapx = cvCreateMat(mUndistortedImage->height, mUndistortedImage->width, CV_32FC1);
		cvReleaseMat(&mCameraMapy);
		mCameraMapy = cvCreateMat(mUndistortedImage->height, mUndistortedImage->width, CV_32FC1);
		cvInitUndistortMap(mCameraIntrinsicParameters,mCameraDistortionCoefficients, mCameraMapx, mCameraMapy);
	}

	cvRemap(tInputImage, mUndistortedImage, mCameraMapx, mCameraMapy, CV_INTER_LINEAR+CV_WARP_FILL_OUTLIERS, cvScalarAll(255));
}
