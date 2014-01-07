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

#ifndef Camera3DProjections_HPP
#define Camera3DProjections_HPP

#include <wykobi/wykobi.hpp>
#include "qa/components/vision/Camera.hpp"

namespace Camera3DProjections {
    template <class T> inline wykobi::point2d<float> project(const wykobi::point3d<T> & pPoint, const Camera * pCamera){
		wykobi::point2d<float> tPoint;
		T tZ = pPoint.z;
		if (abs(tZ) < 0.02f) tZ = 0.02f;
		tPoint.x = (float) pPoint.x * pCamera->getFocalX() / (float)tZ;
		tPoint.y = (float) pPoint.y * pCamera->getFocalY() / (float)tZ;
		tPoint.x += (float)pCamera->getWidth()/2.0f;
		tPoint.y += (float)pCamera->getHeight()/2.0f;

		return tPoint;
	}

    template <class T> inline wykobi::circle<float> project(const wykobi::sphere<T> & pShere, const Camera * pCamera){
		wykobi::circle<float> tCircle;
		tCircle.x = (float) pShere.x * pCamera->getFocalX() / (float)pShere.z;
		tCircle.y = (float) pShere.y * pCamera->getFocalY() / (float)pShere.z;
		tCircle.x += (float)pCamera->getWidth()/2.0f;
		tCircle.y += (float)pCamera->getHeight()/2.0f;
		tCircle.radius = (float) pShere.radius * pCamera->getFocalX() / (float)pShere.z;
		return tCircle;
	}

    inline wykobi::quadix<float, 2> project(const wykobi::quadix<float, 3> & p3dQuadix, const Camera * pCamera){
		wykobi::quadix<float, 2> t2dQuadix;

		t2dQuadix[0] = project(p3dQuadix[0], pCamera);
		t2dQuadix[1] = project(p3dQuadix[1], pCamera);
		t2dQuadix[2] = project(p3dQuadix[2], pCamera);
		t2dQuadix[3] = project(p3dQuadix[3], pCamera);

		return t2dQuadix;
	}
}

#endif
