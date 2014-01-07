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

#ifndef Undistorter_HPP
#define Undistorter_HPP

#include "../Pipeable.hpp"
#include <opencv2/opencv.hpp>

class Undistorter: public Pipeable
{

	public:
		Undistorter(
			float pCameraFx,
			float pCameraFy,
			float pCameraCx,
			float pCameraCy,
			float pCameraAlpha,
			float pCameraK1,
			float pCameraK2,
			float pCameraK3,
			float pCameraK4,
			const IplImage *const *pInputImage

		);

		virtual ~Undistorter();

		const IplImage *const *GetOutputImage() const {return &mUndistortedImage;};

	protected:

        CvMat *mCameraIntrinsicParameters;
        CvMat *mCameraDistortionCoefficients;

		//Intrinsic Parameters
        float mCameraFx;
        float mCameraFy;
        float mCameraCx;
        float mCameraCy;
        float mCameraAlpha;

		//Distortion Parameters
        float mCameraK1;
        float mCameraK2;
        float mCameraK3;
        float mCameraK4;

        CvMat *mCameraMapx;
        CvMat *mCameraMapy;

        const IplImage *const *mInputImage;
        IplImage *mUndistortedImage;

        void run();

};

#endif
