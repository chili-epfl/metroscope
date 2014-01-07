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

#include "OpenCVGrabber.hpp"

#include <opencv2/opencv.hpp>
#include <iostream>


OpenCVGrabber::OpenCVGrabber(
    double pCameraWidth,
    double pCameraHeight,
    double pCameraFrameRate,
	int pCameraIndex)
{
    mCapture=cvCaptureFromCAM(pCameraIndex);
    if (!mCapture)
    {
    	std::cerr << "unable to initialise CVCapture" << std::endl;
    	return;
    }
	cvSetCaptureProperty(mCapture, CV_CAP_PROP_FRAME_WIDTH, pCameraWidth);
	cvSetCaptureProperty(mCapture, CV_CAP_PROP_FRAME_HEIGHT, pCameraHeight);
	cvSetCaptureProperty(mCapture, CV_CAP_PROP_FPS, pCameraFrameRate);
	cvSetCaptureProperty(mCapture, CV_CAP_PROP_MODE, 1);
	mRawImage = cvQueryFrame(mCapture);
}

OpenCVGrabber::~OpenCVGrabber(){
	//Do not release or modify an image from RetrieveFrame
	cvReleaseCapture(&mCapture);
}

void OpenCVGrabber::run()
{
	mRawImage = cvQueryFrame(mCapture);
}
