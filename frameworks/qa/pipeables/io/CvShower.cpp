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

#include "CvShower.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

CvShower::CvShower(
    char pSwitchKey,
    const IplImage *const *pImage,
    const char *pKeyPressed,
	int pShow1FrameEvery
):
mSwitchKey(pSwitchKey),
mImage(pImage),
mKeyPressed(pKeyPressed),
mShow1FrameEvery(pShow1FrameEvery),
mSkippedFrame(1),
mSwitched(true)
{

    mWindowName = "CvShower(";
    mWindowName += pSwitchKey;
    mWindowName += ")";

    if (mSwitched)
    {
        cvNamedWindow(mWindowName.c_str(), CV_WINDOW_AUTOSIZE);
    }
}

CvShower::~CvShower()
{
    if (mSwitched)
    {
        cvDestroyWindow(mWindowName.c_str());
    }
}

void CvShower::run()
{
    if (*mKeyPressed == mSwitchKey)
    {
        mSwitched = !mSwitched;
        if (mSwitched)
        {
            cvNamedWindow(mWindowName.c_str(), CV_WINDOW_AUTOSIZE);
        }
        else
        {
            cvDestroyWindow(mWindowName.c_str());
        }
    }

    if (mSwitched)
    {
		if (mSkippedFrame>=mShow1FrameEvery){
			cvShowImage(mWindowName.c_str(), *mImage);
			mSkippedFrame=1;
		}
		else{
			mSkippedFrame++;
		}
    }
}
