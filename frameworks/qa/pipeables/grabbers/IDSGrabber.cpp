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

#include "IDSGrabber.hpp"
#include <iostream>
#include <opencv2/highgui/highgui.hpp>

IDSGrabber::IDSGrabber(const char *pParameterFile, int pDevId, int pGrabMode):
mCameraHandle((HIDS) pDevId),
mGrabMode(pGrabMode)
{
	is_InitCamera( &mCameraHandle, 0 );
	if (is_LoadParameters(mCameraHandle, pParameterFile) != IS_SUCCESS)
	{
		std::cerr << "Unable to load parameters from " << pParameterFile << std::endl;
	}

	SENSORINFO m_sInfo;
	is_GetSensorInfo( mCameraHandle, &m_sInfo );
	int tCameraWidth = m_sInfo.nMaxWidth;
	int tCameraHeight = m_sInfo.nMaxHeight;

	int tNChannels;
	if( m_sInfo.nColorMode == IS_COLORMODE_BAYER )
	{
		tNChannels = 3;
	}
	else
	{
		tNChannels = 1;
	}

	is_AllocImageMem( mCameraHandle, tCameraWidth, tCameraHeight,tNChannels*8, &mCameraData, &mCameraDataId );
	is_SetImageMem( mCameraHandle, mCameraData, mCameraDataId );

	switch(mGrabMode){
		case 1:
#ifdef WIN32		
			mHEventName=TEXT("");
			mHEvent = CreateEvent(NULL, TRUE, FALSE, mHEventName);
			is_InitEvent(mCameraHandle, mHEvent, IS_SET_EVENT_FRAME);
			is_EnableEvent(mCameraHandle, IS_SET_EVENT_FRAME);
			is_FreezeVideo(mCameraHandle, IS_DONT_WAIT);
#endif
			mImage = cvCreateImageHeader(cvSize(tCameraWidth, tCameraHeight), IPL_DEPTH_8U, tNChannels);
			mImage->imageData = mCameraData;
			mOutputImage = cvCreateImage(cvSize(tCameraWidth, tCameraHeight), IPL_DEPTH_8U, tNChannels);

			break;
		default :
			mOutputImage = cvCreateImageHeader(cvSize(tCameraWidth, tCameraHeight), IPL_DEPTH_8U, tNChannels);
			mOutputImage->imageData = mCameraData;
			break;
	}
}

void IDSGrabber::run(){
	//Now IDS camera users you make a long long row and you bring candies and cookies to me!
	switch(mGrabMode){
		case 0:
			runFreezeVideoWAIT();
			break;
		case 1:
			runFreezeVideoDONTWAIT();
			break;
		case 2:
			runCaptureVideoWAIT();
			break;
		case 3:
			runCaptureVideoDONTWAIT();
			break;
	}
}

void IDSGrabber::runFreezeVideoWAIT()
{
	is_FreezeVideo(mCameraHandle, IS_WAIT);
}

void IDSGrabber::runFreezeVideoDONTWAIT()
{

	//Now IDS camera users you make a long long row and you bring candies and cookies to me!
#ifdef WIN32
	if (WaitForSingleObject(mHEvent, 1000) == WAIT_OBJECT_0)
	{
#endif
		cvCopy(mImage,mOutputImage);
#ifdef WIN32
		is_DisableEvent(mCameraHandle, IS_SET_EVENT_FRAME);
		is_ExitEvent(mCameraHandle, IS_SET_EVENT_FRAME);
	}
	CloseHandle(mHEvent);
	mHEvent = CreateEvent(NULL, TRUE, FALSE, mHEventName);
	is_InitEvent(mCameraHandle, mHEvent, IS_SET_EVENT_FRAME);
	is_EnableEvent(mCameraHandle, IS_SET_EVENT_FRAME);
#endif
	is_FreezeVideo(mCameraHandle, IS_DONT_WAIT);
	
}

void IDSGrabber::runCaptureVideoDONTWAIT(){
	is_CaptureVideo(mCameraHandle, IS_DONT_WAIT);
}

void IDSGrabber::runCaptureVideoWAIT(){
	is_CaptureVideo(mCameraHandle, IS_WAIT);
}

IDSGrabber::~IDSGrabber()
{
	switch(mGrabMode){
		case 1:
#ifdef WIN32
			is_DisableEvent(mCameraHandle, IS_SET_EVENT_FRAME);
			is_ExitEvent(mCameraHandle, IS_SET_EVENT_FRAME);
			CloseHandle(mHEvent);
#endif
			cvReleaseImageHeader(&mImage);
			cvReleaseImage(&mOutputImage);
			break;
		default :
			cvReleaseImageHeader(&mOutputImage);
			break;
	}
	is_FreeImageMem( mCameraHandle, mCameraData, mCameraDataId );
	is_ExitCamera( mCameraHandle );
}
