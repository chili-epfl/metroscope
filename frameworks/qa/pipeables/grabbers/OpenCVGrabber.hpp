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

#ifndef OpenCVGrabber_H
#define OpenCVGrabber_H

#include "../Pipeable.hpp"
#include <opencv2/highgui/highgui.hpp>

class OpenCVGrabber : public Pipeable{
public:
    OpenCVGrabber(
			double pCameraWidth,
			double pCameraHeight,
			double pCameraFrameRate,
			int pCameraIndex = 0);
    const IplImage *const *Image() const {return &mRawImage;};

    ~OpenCVGrabber();

	int getFps() const {return (int)cvGetCaptureProperty(mCapture, CV_CAP_PROP_FPS);}

    void run();

private:

	CvCapture *mCapture;
	IplImage *mRawImage;
};
#endif


