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

#ifndef FlyCapture2Grabber_HPP
#define FlyCapture2Grabber_HPP

#include "../Pipeable.hpp"
#include <opencv2/opencv.hpp>
#include <flycapture/FlyCapture2.h>

class FlyCapture2Grabber : public Pipeable{

    public:
        FlyCapture2Grabber(
            int pWidth,
            int pHeight,
            float pFrameRate,
            int pDeviceId = 0);

		const IplImage *const *Image() const {return &mImage;};

        ~FlyCapture2Grabber();



    private:
		int mWidth;
		int mHeight;
		int mDeviceId;
        FlyCapture2::Camera mCamera;
        FlyCapture2::Image mRawImage;
		IplImage	*mImage;

		void run();

};

#endif


