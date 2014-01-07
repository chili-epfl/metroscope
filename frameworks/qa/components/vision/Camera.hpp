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

#ifndef Camera_HPP
#define Camera_HPP

#include <string>

class Camera
{
	public:

		Camera(
			int pWidth,
			int pHeight,
			int pChannels,
			float pFrameRate,
			float pAlpha,
			float pCx,
			float pCy,
			float pFocalX,
			float pFocalY,
			float pK1,
			float pK2,
			float pK3,
			float pK4);

		int getWidth() const{return mWidth;}
		int getHeight() const{return mHeight;}
		int getChannels() const{return mChannels;}
		float getFrameRate() const{return mFrameRate;}
		float getCenterX() const{return mCenterX;}
		float getCenterY() const{return mCenterY;}
		float getFocalX() const{return mFocalX;}
		float getFocalY() const{return mFocalY;}
		float getAlpha() const{return mAlpha;}
		float getK1() const{return mK1;}
		float getK2() const{return mK2;}
		float getK3() const{return mK3;}
		float getK4() const{return mK4;}

protected:

	int mWidth;
	int mHeight;
	int mChannels;
	float mFrameRate;
	float mAlpha;
	float mCenterX;
	float mCenterY;
	float mFocalX;
	float mFocalY;
	float mK1;
	float mK2;
	float mK3;
	float mK4;
};

#endif
