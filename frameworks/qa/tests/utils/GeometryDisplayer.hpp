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

#ifndef GeometryDisplayer_HPP
#define GeometryDisplayer_HPP

#include <wykobi/wykobi.hpp>
#include <GL/glut.h>
#include <GL/glu.h>
#include <qa/pipeables/Pipeable.hpp>
#include <vector>

class GeometryDisplayer: public Pipeable
{
public :
	GeometryDisplayer(
		const wykobi::point3d<float> * pCenter,
		const wykobi::point3d<float> * pPoint,
		const wykobi::vector3d<float> * pEulerianAngles
	);
	
	virtual ~GeometryDisplayer();


protected:
	void run();

	const wykobi::point3d<float> * mCenter;
	const wykobi::point3d<float> * mPoint;
	const wykobi::vector3d<float> * mEulerianAngles;

	wykobi::vector3d<float> mCurrentCameraEye;
	wykobi::vector3d<float> mCurrentCameraDestination;

};

#endif
