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

#ifndef IDSGrabber_HPP
#define IDSGrabber_HPP

#include <qa/pipeables/Pipeable.hpp>
#include <opencv2/opencv.hpp>

#ifndef WIN32
#ifndef __LINUX__
    #define __LINUX__
#endif
#endif
#include <ueye.h>

class IDSGrabber: public Pipeable
{
    public:

		IDSGrabber(const char *pParameterFile, int pDevId = 0, int pGrabMode = scFreezeDontWaitGrabMode);
		virtual ~IDSGrabber();
		const IplImage* const *Image() const {return &mOutputImage;}

		static const int scFreezeWaitGrabMode = 0;
		static const int scFreezeDontWaitGrabMode = 1;
		static const int scCaptureWaitGrabMode = 2;
		static const int scCaptureDontWaitGrabMode = 3;

protected:
		void run();

		HIDS mCameraHandle;
#ifdef WIN32
		HANDLE mHEvent;
		LPCTSTR mHEventName;
#endif

		int mCount;

		char *mCameraData;
		INT	mCameraDataId;
		IplImage* mImage;
		IplImage* mOutputImage;
		int mGrabMode;

    private:
        IDSGrabber();
        IDSGrabber(const IDSGrabber& pIDSGrabber);
        IDSGrabber& operator=(const IDSGrabber& pIDSGrabber);
		void runFreezeVideoDONTWAIT();
		void runFreezeVideoWAIT();
		void runCaptureVideoDONTWAIT();
		void runCaptureVideoWAIT();


};

#endif
