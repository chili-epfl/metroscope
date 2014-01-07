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

#ifndef CvShower_H
#define CvShower_H

#include "../Pipeable.hpp"

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

class CvShower: public Pipeable
{
    public:
        CvShower(
            char pSwitchKey,
            const IplImage *const *pImage,
            const char *pKeyPressed,
			int pShow1FrameEvery = 1);
        virtual ~CvShower();

        CvShower *StartingPosition(CvPoint pStartingPosition) {
			cvMoveWindow(mWindowName.c_str(), pStartingPosition.x, pStartingPosition.y);
			return this;};

	protected:
        char mSwitchKey;
        const IplImage *const *mImage;
        const char *mKeyPressed;
		int mShow1FrameEvery;
		int mSkippedFrame;

        std::string mWindowName;
        bool mSwitched;
        virtual void run();
};
#endif
