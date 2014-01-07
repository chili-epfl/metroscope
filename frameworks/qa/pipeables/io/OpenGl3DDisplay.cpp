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

#include "OpenGl3DDisplay.hpp"
#include <wykobi/wykobi.hpp>

OpenGl3DDisplay::OpenGl3DDisplay(int pDisplayX, int pDisplayY, int pDisplayWidth, int pDisplayHeight, std::string pWindowName):
mCameraFovY(42.0f),
mCameraAspect((float)pDisplayWidth/(float)pDisplayHeight),
mNearPlane(0.1f),
mFarPlane(1000.0f)
{
	
	int tArgs = 0;
	glutInit(&tArgs, 0);
	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(pDisplayWidth, pDisplayHeight);
	glutInitWindowPosition(pDisplayX, pDisplayY);
	glutCreateWindow(pWindowName.c_str());
	
	GLfloat light_ambient_0[] =	{0.65f, 0.65f, 0.65f, 1.0f};
	GLfloat light_diffuse_0[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light_specular_0[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light_position_0[] = {5.0f, 5.0f, 0.0f, 0.0f};
	
	glLightfv (GL_LIGHT0, GL_AMBIENT,	light_ambient_0);
	glLightfv (GL_LIGHT0, GL_DIFFUSE,	light_diffuse_0);
	glLightfv (GL_LIGHT0, GL_SPECULAR,	light_specular_0);
	glLightfv (GL_LIGHT0, GL_POSITION,	light_position_0);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(mCameraFovY, mCameraAspect, mNearPlane, mFarPlane);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFlush();
	glutSwapBuffers();
}

OpenGl3DDisplay::~OpenGl3DDisplay(){}

void OpenGl3DDisplay::run(){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glFlush();
	glutSwapBuffers();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


