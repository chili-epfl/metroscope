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

#include "Camera.hpp"
#include <math.h>

Camera::Camera (
	int pWidth,
	int pHeight,
	int pChannels,
	float pFrameRate,
	float pAlpha,
	float pCenterX,
	float pCenterY,
	float pFocalX,
	float pFocalY,
	float pK1,
	float pK2,
	float pK3,
	float pK4):
mWidth(pWidth),
mHeight(pHeight),
mChannels(pChannels),
mFrameRate(pFrameRate),
mAlpha(pAlpha),
mCenterX(pCenterX),
mCenterY(pCenterY),
mFocalX(pFocalX),
mFocalY(pFocalY),
mK1(pK1),
mK2(pK2),
mK3(pK3),
mK4(pK4)
{}

