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

#include "CvWaiter.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>


CvWaiter::CvWaiter():
mBlockingTimeMillis(10),
mKeyPressed(-1)
{
    //TODO use singleton
    if (!mInstancesCount) cvNamedWindow(cWindowName, CV_WINDOW_AUTOSIZE);
    mInstancesCount++;

}

CvWaiter::~CvWaiter()
{
    mInstancesCount--;
    if (!mInstancesCount) cvDestroyWindow(cWindowName);
}

void CvWaiter::run()
{
    mKeyPressed = (char) cvWaitKey(mBlockingTimeMillis);
}

const char *const CvWaiter::cWindowName = "CvWaiter";
int CvWaiter::mInstancesCount = 0;
