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

#include "GeometryDisplayer.hpp"
#include <qa/utils/Dynamics.hpp>

GeometryDisplayer::GeometryDisplayer(
	const wykobi::point3d<float> * pCenter,
	const wykobi::point3d<float> * pPoint,
	const wykobi::vector3d<float> * pEulerianAngles):
mCenter(pCenter),
mPoint(pPoint),
mEulerianAngles(pEulerianAngles)
{
	glEnable (GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
}

GeometryDisplayer::~GeometryDisplayer(){

}

void GeometryDisplayer::run(){

	gluLookAt
	(
		0,	0,	0,
		0,	0,	1,
		0,	-1,	0
	);

	wykobi::point3d<float> tCenter = *mCenter;
	wykobi::point3d<float> tPoint = *mPoint;
	wykobi::vector3d<float> tEulerianAngles = *mEulerianAngles;
	glEnable(GL_TEXTURE_2D);

	GLUquadric *qCenter = gluNewQuadric();
	gluQuadricTexture(qCenter,GL_TRUE);

	glPushMatrix();
		glTranslatef((GLfloat)tCenter.x,(GLfloat)tCenter.y,(GLfloat)tCenter.z);
			glColor3f(1.0f,0.0f,0.0f);
			gluSphere(qCenter,0.5,48,48);
		glPopMatrix();
	glPopMatrix();


	GLUquadric *qPoint = gluNewQuadric();
	gluQuadricTexture(qPoint,GL_TRUE);

	glPushMatrix();
		glRotatef(tEulerianAngles.x,1,0,0);
		glRotatef(tEulerianAngles.y,0,1,0);
		glRotatef(tEulerianAngles.z,0,0,1);
		glTranslatef((GLfloat)tPoint.x,(GLfloat)tPoint.y,(GLfloat)tPoint.z);
			glColor3f(0.0f,0.0f,1.0f);
			gluSphere(qPoint,0.5,48,48);
		glPopMatrix();
	glPopMatrix();

}
