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

#ifndef FiducialMarker_HPP
#define FiducialMarker_HPP

#include <wykobi/wykobi.hpp>

class FiducialMarker
{
	public:
	virtual ~FiducialMarker(){}
	virtual bool isPresent() const = 0;
	virtual wykobi::quadix<float, 2> getCorners() const = 0;

	static void ComputeBasisFromSquare(const wykobi::quadix<float, 2> pCorners, wykobi::point2d<float> &pOrigin, wykobi::point2d<float> &pXUnit, wykobi::point2d<float> &pYUnit);
	wykobi::point2d<float> getCenter() const;
	wykobi::point2d<float> getXUnit() const;
	wykobi::point2d<float> getYUnit() const;
	float getScale() const;
	float getAngle() const;
	wykobi::point3d<float> WorldCenter(float pMarkerSideLength, float pCameraWidth, float pCameraHeight, float pFocalLengthX, float pFocalLengthY) const;
	wykobi::quadix<float, 3> WorldCorners(float pMarkerSideLength, float pCameraWidth, float pCameraHeight, float pFocalLengthX, float pFocalLengthY) const;
	wykobi::quadix<float, 3> WorldCorners2(float pMarkerSideLength, float pCameraWidth, float pCameraHeight, float pFocalLengthX, float pFocalLengthY) const;
	wykobi::point3d<float> EulerAngles(wykobi::quadix<float, 3> &pWorldCorners) const;
};
#endif
