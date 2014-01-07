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

#ifndef OpenGlCamera_H
#define OpenGlCamera_H

#include <GL/glut.h>
#include <wykobi/wykobi.hpp>
#include "../vision/Camera.hpp"

class OpenGlCamera{
    public:

		OpenGlCamera();
		OpenGlCamera(
			const wykobi::point3d<float> & pLookFrom,
			const wykobi::point3d<float> & pLookTo,
			const wykobi::vector3d<float> & pCameraOrientation = wykobi::vector3d<float>(0.0f,-1.0f,0.0f));

        virtual ~OpenGlCamera();

		void setLookFrom(const wykobi::point3d<float> & pLookFrom){mLookFrom = pLookFrom;}
		void setLookTo(const wykobi::point3d<float> & pLookTo){mLookTo = pLookTo;}
		void setCameraOrientation(const wykobi::vector3d<float> & pCameraOrientation){mCameraOrientation = pCameraOrientation;}
		void update() {
			gluLookAt
			(
				mLookFrom.x,			mLookFrom.y,			mLookFrom.z,
				mLookTo.x,				mLookTo.y,				mLookTo.z,
				mCameraOrientation.x,	mCameraOrientation.y,	mCameraOrientation.z
			);
		}

		void setCameraProjectionMatrix(const Camera * pCamera, float pNearPlane= 1.0f, float pFarPlane = 1000.0f);
		void setCameraProjectionMatrix(float pLeft, float pRight, float pBottom, float pTop, float pNear, float pFar);


    protected:

		wykobi::point3d<float> mLookFrom;
		wykobi::point3d<float> mLookTo;
		wykobi::vector3d<float> mCameraOrientation;

};

#endif


