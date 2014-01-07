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

#ifndef VideoGrabber_HPP
#define VideoGrabber_HPP

#include "../Pipeable.hpp"
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

class VideoGrabber : public Pipeable{
    public:
        VideoGrabber(const char *pFileName);

        const IplImage *const *Image() const {return &mReadImage;}
        const double *Fps() const {return &mFps;}
		virtual ~VideoGrabber();

		void run();

    protected:
		CvCapture *mCapture;
        IplImage *mReadImage;
		double mFps;
};
#endif


