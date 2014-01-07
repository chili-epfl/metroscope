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

#ifndef OpenGl3DDisplay_HPP
#define OpenGl3DDisplay_HPP

#include "../Pipeable.hpp"
#include <GL/glut.h>
#include <opencv2/opencv.hpp>
#include <qa/components/vision/Camera.hpp>

class OpenGl3DDisplay: public Pipeable
{
public :
	OpenGl3DDisplay(int pDisplayX = 0, int pDisplayY = 0, int pDisplayWidth = 800, int pDisplayHeight = 600, std::string pWindowName = "OpenGl3DDisplay");
	virtual ~OpenGl3DDisplay();

protected:
	void run();

	float mCameraFovY;
	float mCameraAspect;
	float mNearPlane;
	float mFarPlane;
};

#endif
