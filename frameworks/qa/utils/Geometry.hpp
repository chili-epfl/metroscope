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

#ifndef Geometry_HPP
#define Geometry_HPP

#include <wykobi/wykobi.hpp>
#include <wykobi/wykobi_algorithm.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace Geometry {
	template <class T> wykobi::point3d<T> rotateCorrect(const wykobi::vector3d<T> & pEulerAngles, const wykobi::point3d<T> & pPoint);
	template <class T> wykobi::point3d<T> rotateCorrect(const wykobi::vector3d<T> & pEulerAngles, const wykobi::point3d<T> & pPoint, const wykobi::point3d<T> & pCenter);
	template <class T> wykobi::point3d<T> rotateCorrect(const CvMat * pRotationMatrix, const wykobi::point3d<T> & pPoint);
	template <class T> wykobi::point3d<T> rotateCorrect(const CvMat * pRotationMatrix, const wykobi::point3d<T> & pPoint, const wykobi::point3d<T> & pCenter);
	template <class T> wykobi::point2d<T> rotateCorrect(T pAngle, const wykobi::point2d<T> & pPoint, const wykobi::point2d<T> & pCenter);
	template <class T> wykobi::point3d<T> rotateCorrect(const wykobi::vectornd<T,4> & pQuaternion, const wykobi::point3d<T> & pPoint);
	template <class T> wykobi::point3d<T> rotateCorrect(const wykobi::vectornd<T,4> & pQuaternion, const wykobi::point3d<T> & pPoint, const wykobi::point3d<T> & pCenter);
	template <class T> wykobi::vector3d<T> rotateCorrect(const wykobi::vectornd<T,4> & pQuaternion, const wykobi::vector3d<T> & pVector);


	template <class T> void getRotationMatrix(const wykobi::vector3d<T> & pEulerAngles, CvMat * pRotationMatrix);
	template <class T> void getRotationMatrix(const wykobi::vector3d<T> * pNormalizedXAxis,const wykobi::vector3d<T> * pNormalizedYAxis,const wykobi::vector3d<T> * pNormalizedZAxis,CvMat * pRotationMatrix);
	template <class T> void getRotationMatrix(const wykobi::vector3d<T> & pNormalizedXOAxis,const wykobi::vector3d<T> & pNormalizedYOAxis,const wykobi::vector3d<T> & pNormalizedZOAxis,const wykobi::vector3d<T> & pNormalizedXRAxis,const wykobi::vector3d<T> & pNormalizedYRAxis,const wykobi::vector3d<T> & pNormalizedZRAxis,CvMat * pRotationMatrix);
	template <class T> void getRotationMatrix(const wykobi::vectornd<T,4> & pQuaternion, CvMat * pRotationMatrix);

	template <class T> wykobi::vector3d<T> getEulerAngles(const	CvMat * pRotationMatrix);
	template <class T> wykobi::vector3d<T> getEulerAngles(const wykobi::point3d<T> pCenter, const wykobi::point3d<T> pPoint);
	template <class T> wykobi::vector3d<T> getEulerAngles(const wykobi::vectornd<T,4> & pQuaternion);

	template <class T> wykobi::vectornd<T,4> getQuaternion(const CvMat * pRotationMatrix);
	template <class T> wykobi::vectornd<T,4> getQuaternion(const wykobi::vector3d<T> & pVector1,const wykobi::vector3d<T> & pVector2);
	template <class T> wykobi::vectornd<T,4> getQuaternion(const wykobi::vector3d<T> & pEuler);
	template <class T> wykobi::vectornd<T,4> sumQuaternions(const wykobi::vectornd<T,4> & pQuaternion1, const wykobi::vectornd<T,4> & pQuaternion2);
	template <class T> wykobi::vectornd<T,4> multiplyQuaternions(const wykobi::vectornd<T,4> & pQuaternion1, const wykobi::vectornd<T,4> & pQuaternion2);

	template <class T> void split(const wykobi::polygon<T,2> &pSrc, const wykobi::line<T, 2> &pLine,wykobi::polygon<T,2> &pDst1, wykobi::polygon<T,2> &pDst2);
	template <class T> wykobi::rectangle<T> bounding_box(const wykobi::polygon<T,2> &pPolygon);
	template <class T> void normalize(const wykobi::rectangle<T> pBoundingBox,wykobi::polygon<T,2> &pPolygon);
	template <class T> wykobi::vectornd<T,4> normalize(const wykobi::vectornd<T,4> & pQuaternion);
	template <class T> wykobi::vector3d<T> getNormalVector(const wykobi::point3d<T> pPoint1, const wykobi::point3d<T> pPoint2, const wykobi::point3d<T> pPoint3);
	template <class T> wykobi::vector3d<T> getVector(const wykobi::point3d<T> &pPoint1, const wykobi::point3d<T> &pPoint2);
	template <class T> wykobi::vector3d<T> crossProduct(const wykobi::vector3d<T> & pVector1,const wykobi::vector3d<T> & pVector2);
	template <class T> T dotProduct(const wykobi::vector3d<T> & pVector1,const wykobi::vector3d<T> & pVector2);
	template <class T> T getVectorLength(const wykobi::vector3d<T> & pVector);
	template <class T> T getVectorLengthSqrd(const wykobi::vector3d<T> & pVector);
	template <class T> wykobi::point3d<T> getPoint(const wykobi::point3d<T> pPoint, const wykobi::vector3d<T> pVector, T pDistance);
	template <class T> wykobi::point2d<T> getPoint(const wykobi::point2d<T> pPoint, const wykobi::vector2d<T> pVector, T pDistance);
}




#endif //Geometry_HPP
