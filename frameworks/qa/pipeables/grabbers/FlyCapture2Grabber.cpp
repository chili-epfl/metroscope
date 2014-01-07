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

#include "FlyCapture2Grabber.hpp"
#include <iostream>

namespace {
	void checkError(const FlyCapture2::Error & pResult)
	{
		if (pResult.GetType() != FlyCapture2::PGRERROR_OK)
		{
			std::cerr << "FlyCapture2Grabber: " << pResult.GetDescription() << std::endl;
		}
	}
}

FlyCapture2Grabber::FlyCapture2Grabber(
	int pWidth,
	int pHeight,
	float pFrameRate,
	int pDeviceId):
mWidth(pWidth),
mHeight(pHeight),
mDeviceId(pDeviceId)
{
	FlyCapture2::Error tResult;
	FlyCapture2::PGRGuid guid;
    FlyCapture2::BusManager busMgr;
    tResult = busMgr.GetCameraFromIndex(mDeviceId, &guid);
    checkError(tResult);
    tResult = mCamera.Connect(&guid);
    checkError(tResult);
    tResult = mCamera.StartCapture();
    checkError(tResult);


	FlyCapture2::VideoMode tVideoMode;
	FlyCapture2::FrameRate tFrameRate;
	mCamera.GetVideoModeAndFrameRate(&tVideoMode,&tFrameRate);
	     if (mWidth ==  640 && mHeight ==  480) tVideoMode = FlyCapture2::VIDEOMODE_640x480Y8;
	else if (mWidth ==  800 && mHeight ==  600) tVideoMode = FlyCapture2::VIDEOMODE_800x600Y8;
	else if (mWidth == 1024 && mHeight ==  768) tVideoMode = FlyCapture2::VIDEOMODE_1024x768Y8;
	else if (mWidth == 1280 && mHeight ==  960) tVideoMode = FlyCapture2::VIDEOMODE_1280x960Y8;
	else if (mWidth == 1600 && mHeight == 1200) tVideoMode = FlyCapture2::VIDEOMODE_1600x1200Y8;
	else std::cerr << "FlyCapture2Grabber: Not implemented/supported resolutions: " << mWidth << "x" << mHeight << std::endl;
	mImage = cvCreateImage(cvSize(mWidth,mHeight),IPL_DEPTH_8U,1);

         if (pFrameRate ==  1.875f) tFrameRate = FlyCapture2::FRAMERATE_1_875;
    else if (pFrameRate ==  3.75f ) tFrameRate = FlyCapture2::FRAMERATE_3_75;
    else if (pFrameRate ==  7.5f  ) tFrameRate = FlyCapture2::FRAMERATE_7_5;
    else if (pFrameRate == 15.0f  ) tFrameRate = FlyCapture2::FRAMERATE_15;
    else if (pFrameRate == 30.0f  ) tFrameRate = FlyCapture2::FRAMERATE_30;
    else if (pFrameRate == 60.0f  ) tFrameRate = FlyCapture2::FRAMERATE_60;
	else std::cerr << "FlyCapture2Grabber: Not implemented/supported framerate: " << pFrameRate << std::endl;

	tResult = mCamera.SetVideoModeAndFrameRate(tVideoMode,tFrameRate);
	checkError(tResult);

/* TODO
	FlyCapture2::Property prop;
	prop.type = FlyCapture2::BRIGHTNESS;
	mCamera.GetProperty(&prop);
	prop.valueA = 0;
	mCamera.SetProperty(&prop);

	prop.type = FlyCapture2::AUTO_EXPOSURE;
	mCamera.GetProperty(&prop);
	prop.valueA = 1023;
	prop.autoManualMode = false;
	prop.onOff = true;
	prop.onePush = false;
	mCamera.SetProperty(&prop);

	prop.type = FlyCapture2::GAMMA;
	mCamera.GetProperty(&prop);
	prop.valueA = 512;
	prop.onOff = false;
	mCamera.SetProperty(&prop);

	prop.type = FlyCapture2::PAN;
	mCamera.GetProperty(&prop);
	prop.valueA = 8;
	prop.autoManualMode = true;
	prop.onOff = true;
	mCamera.SetProperty(&prop);

	prop.type = FlyCapture2::TILT;
	mCamera.GetProperty(&prop);
	prop.valueA = 2;
	prop.autoManualMode = true;
	prop.onOff = true;
	mCamera.SetProperty(&prop);

	prop.type = FlyCapture2::SHUTTER;
	mCamera.GetProperty(&prop);
	prop.valueA = 547;
	prop.autoManualMode = false;
	prop.onePush = false;
	mCamera.SetProperty(&prop);

	prop.type = FlyCapture2::GAIN;
	mCamera.GetProperty(&prop);
	prop.valueA = 160;
	prop.autoManualMode = false;
	prop.onePush = false;
	mCamera.SetProperty(&prop);
*/
}


FlyCapture2Grabber::~FlyCapture2Grabber(){
	FlyCapture2::Error tResult;
    tResult = mCamera.StopCapture();
	checkError(tResult);
    tResult = mCamera.Disconnect();
	checkError(tResult);
	cvReleaseImage(&mImage);
}

void FlyCapture2Grabber::run()
{
	FlyCapture2::Error tResult = mCamera.RetrieveBuffer( &mRawImage );
	checkError(tResult);
    memcpy(mImage->imageData, mRawImage.GetData(), mWidth*mHeight);
}
