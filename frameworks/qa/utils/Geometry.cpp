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

#include "Geometry.hpp"
#include <iostream>

namespace Geometry {

	template <class T> wykobi::point3d<T> rotateCorrect(const wykobi::vector3d<T> & pEulerAngles, const wykobi::point3d<T> & pPoint) {
		return wykobi::rotate<T>(
			(T)((float)pEulerAngles[0] / (float)wykobi::PIDiv180),
			(T)((float)pEulerAngles[1] / (float)wykobi::PIDiv180),
			(T)((float)pEulerAngles[2] /(float) wykobi::PIDiv180),
			pPoint);
	}

	template <class T> wykobi::point3d<T> rotateCorrect(const wykobi::vector3d<T> & pEulerAngles, const wykobi::point3d<T> & pPoint, const wykobi::point3d<T> & pCenter) {
		return wykobi::rotate<T>(
			(T)((float)pEulerAngles[0] / (float)wykobi::PIDiv180),
			(T)((float)pEulerAngles[1] / (float)wykobi::PIDiv180),
			(T)((float)pEulerAngles[2] / (float)wykobi::PIDiv180),
			pPoint,
			pCenter);
	}

	template <class T> wykobi::point3d<T> rotateCorrect(const CvMat * pRotationMatrix, const wykobi::point3d<T> & pPoint) {
		wykobi::point3d<T> tRetPoint;

		tRetPoint.x = pRotationMatrix->data.fl[0] * pPoint.x + pRotationMatrix->data.fl[1] * pPoint.y + pRotationMatrix->data.fl[2] * pPoint.z;
		tRetPoint.y = pRotationMatrix->data.fl[3] * pPoint.x + pRotationMatrix->data.fl[4] * pPoint.y + pRotationMatrix->data.fl[5] * pPoint.z;
		tRetPoint.z = pRotationMatrix->data.fl[6] * pPoint.x + pRotationMatrix->data.fl[7] * pPoint.y + pRotationMatrix->data.fl[8] * pPoint.z;

		return tRetPoint;
	}

	template <class T> wykobi::point3d<T> rotateCorrect(const CvMat * pRotationMatrix, const wykobi::point3d<T> & pPoint, const wykobi::point3d<T> & pCenter) {
		wykobi::point3d<T> tRetPoint = pPoint - pCenter;

		tRetPoint = rotateCorrect(pRotationMatrix,tRetPoint);

		tRetPoint=tRetPoint+pCenter;

		return tRetPoint;
	}

	template <class T> wykobi::vector3d<T> rotateCorrect(const wykobi::vectornd<T,4> & pQuaternion, const wykobi::vector3d<T> & pVector){

		float q0 = pQuaternion[0];
		float q1 = pQuaternion[1];
		float q2 = pQuaternion[2];
		float q3 = pQuaternion[3];

		float tX = pVector.x;
		float tY = pVector.y;
		float tZ = pVector.z;

		wykobi::vector3d<T> tRet;
		tRet[0]= (1.0f - 2.0f * q2 * q2 - 2.0f * q3 * q3) * tX		+ 2.0f * (q1 * q2 + q0 * q3) * tY +					2.0f * (q1 * q3 - q0 * q2) * tZ;
		tRet[1]= 2.0f * (q1 * q2 - q0 * q3) * tX					+ (1.0f - 2.0f * q1 * q1 - 2.0f * q3 *q3) * tY + 	2.0f * (q2 * q3 + q0 * q1) * tZ;
		tRet[2]= 2.0f * (q1 * q3 + q0 * q2) * tX					+ 2.0f * (q2 * q3 - q0 * q1) * tY + 				(1.0f - 2.0f * q1 * q1 - 2.0f * q2 * q2) * tZ;

		return tRet;

	}

	template <class T> wykobi::point3d<T> rotateCorrect(const wykobi::vectornd<T,4> & pQuaternion, const wykobi::point3d<T> & pPoint){

		T q0 = pQuaternion[0];
		T q1 = pQuaternion[1];
		T q2 = pQuaternion[2];
		T q3 = pQuaternion[3];

		wykobi::point3d<T> tRet;
		tRet.x= (1.0f - 2.0f * q2 * q2 - 2.0f * q3 * q3) * pPoint.x	+	2.0f * (q1 * q2 - q0 * q3) * pPoint.y +					2.0f * (q1 * q3 + q0 * q2) * pPoint.z;
		tRet.y= 2.0f * (q1 * q2 + q0 * q3) * pPoint.x +					(1.0f - 2.0f * q1 * q1 - 2.0f * q3 *q3) * pPoint.y + 	2.0f * (q2 * q3 - q0 * q1) * pPoint.z;
		tRet.z= 2.0f * (q1 * q3 - q0 * q2) * pPoint.x +					2.0f * (q2 * q3 + q0 * q1) * pPoint.y + 				(1.0f - 2.0f * q1 * q1 - 2.0f * q2 * q2) * pPoint.z;

		return tRet;

	}

	template <class T> wykobi::point3d<T> rotateCorrect(const wykobi::vectornd<T,4> & pQuaternion, const wykobi::point3d<T> & pPoint, const wykobi::point3d<T> & pCenter){
		
		wykobi::point3d<T> tRet;
		tRet.x = pPoint.x - pCenter.x;
		tRet.y = pPoint.y - pCenter.y;
		tRet.z = pPoint.z - pCenter.z;

		tRet = rotateCorrect(pQuaternion,tRet);

		tRet.x+=pCenter.x;
		tRet.y+=pCenter.y;
		tRet.z+=pCenter.z;

		return tRet;

	}

	template <class T> void getRotationMatrix(const wykobi::vector3d<T> & pEulerAngles, CvMat * pRotationMatrix) {

	float tPhi = pEulerAngles.x;
	float tTheta = pEulerAngles.y;
	float tGamma = pEulerAngles.z;

	pRotationMatrix->data.fl[0] = cos(tTheta) * cos(tPhi);
	pRotationMatrix->data.fl[1] = sin(tGamma) * sin(tTheta) * cos(tPhi) - cos(tGamma) * sin(tPhi) ;
	pRotationMatrix->data.fl[2] = cos(tGamma) * sin(tTheta) * cos(tPhi)  + sin(tGamma) * sin(tPhi) ;

	pRotationMatrix->data.fl[3] = cos(tTheta) * sin(tPhi);
	pRotationMatrix->data.fl[4] = sin(tGamma) * sin(tTheta) * sin (tPhi) + cos(tGamma) * cos(tPhi);
	pRotationMatrix->data.fl[5] = cos(tGamma) * sin(tTheta) * sin(tPhi) - sin(tGamma) * cos(tPhi);

	pRotationMatrix->data.fl[6] = - sin(tTheta);
	pRotationMatrix->data.fl[7] = sin(tGamma) *cos(tTheta);
	pRotationMatrix->data.fl[8] = cos(tGamma) * cos(tTheta);
}

	template <class T> void getRotationMatrix(const wykobi::vector3d<T> * pNormalizedXAxis,const wykobi::vector3d<T> * pNormalizedYAxis,const wykobi::vector3d<T> * pNormalizedZAxis,CvMat * pRotationMatrix){

	pRotationMatrix->data.fl[0] = pNormalizedXAxis->x;
	pRotationMatrix->data.fl[3] = pNormalizedXAxis->y;
	pRotationMatrix->data.fl[6] = pNormalizedXAxis->z;
	pRotationMatrix->data.fl[1] = pNormalizedYAxis->x;
	pRotationMatrix->data.fl[4] = pNormalizedYAxis->y;
	pRotationMatrix->data.fl[7] = pNormalizedYAxis->z;
	pRotationMatrix->data.fl[2] = pNormalizedZAxis->x;
	pRotationMatrix->data.fl[5] = pNormalizedZAxis->y;
	pRotationMatrix->data.fl[8] = pNormalizedZAxis->z;

}

	template <class T> void getRotationMatrix(const wykobi::vector3d<T> & pNormalizedXOAxis,const wykobi::vector3d<T> & pNormalizedYOAxis,const wykobi::vector3d<T> & pNormalizedZOAxis,const wykobi::vector3d<T> & pNormalizedXRAxis,const wykobi::vector3d<T> & pNormalizedYRAxis,const wykobi::vector3d<T> & pNormalizedZRAxis,CvMat * pRotationMatrix){


		float tIxO = pNormalizedXOAxis.x;
		float tIyO = pNormalizedXOAxis.y;
		float tIzO = pNormalizedXOAxis.z;

		float tJxO = pNormalizedYOAxis.x;
		float tJyO = pNormalizedYOAxis.y;
		float tJzO = pNormalizedYOAxis.z;

		float tKxO = pNormalizedZOAxis.x;
		float tKyO = pNormalizedZOAxis.y;
		float tKzO = pNormalizedZOAxis.z;

		float tIxR = pNormalizedXRAxis.x;
		float tIyR = pNormalizedXRAxis.y;
		float tIzR = pNormalizedXRAxis.z;

		float tJxR = pNormalizedYRAxis.x;
		float tJyR = pNormalizedYRAxis.y;
		float tJzR = pNormalizedYRAxis.z;

		float tKxR = pNormalizedZRAxis.x;
		float tKyR = pNormalizedZRAxis.y;
		float tKzR = pNormalizedZRAxis.z;


		pRotationMatrix->data.fl[0] = tIxO * tIxR + tIyO * tIyR + tIzO * tIzR;
		pRotationMatrix->data.fl[1] = tIxO * tJxR + tIyO * tJyR + tIzO * tJzR;
		pRotationMatrix->data.fl[2] = tIxO * tKxR + tIyO * tKyR + tIzO * tKzR;

		pRotationMatrix->data.fl[3] = tJxO * tIxR + tJyO * tIyR + tJzO * tIzR;
		pRotationMatrix->data.fl[4] = tJxO * tJxR + tJyO * tJyR + tJzO * tJzR;
		pRotationMatrix->data.fl[5] = tJxO * tKxR + tJyO * tKyR + tJzO * tKzR;

		pRotationMatrix->data.fl[6] = tKxO * tIxR + tKyO * tIyR + tKzO * tIzR;
		pRotationMatrix->data.fl[7] = tKxO * tJxR + tKyO * tJyR + tKzO * tJzR;
		pRotationMatrix->data.fl[8] = tKxO * tKxR + tKyO * tKyR + tKzO * tKzR;

	}
	
	template <class T> void getRotationMatrix(const wykobi::vectornd<T,4> & pQuaternion, CvMat * pRotationMatrix) {

		float tQw = pQuaternion[0];
		float tQx = pQuaternion[1];
		float tQy = pQuaternion[2];
		float tQz = pQuaternion[3];

		pRotationMatrix->data.fl[0] = (1 - 2*tQy*tQy - 2*tQz*tQz );
		pRotationMatrix->data.fl[1] = 2*(tQx*tQy - tQz*tQw);
		pRotationMatrix->data.fl[2] = 2*(tQx*tQz + tQy*tQw);

		pRotationMatrix->data.fl[3] = 2*(tQx*tQy + tQz*tQw);
		pRotationMatrix->data.fl[4] = (1 - 2*tQx*tQx - 2*tQz*tQz );
		pRotationMatrix->data.fl[5] =  2*(tQy*tQz - tQx*tQw);

		pRotationMatrix->data.fl[6] = 2*(tQx*tQz - tQy*tQw);
		pRotationMatrix->data.fl[7] = 2*(tQy*tQz + tQx*tQw);
		pRotationMatrix->data.fl[8] = (1 - 2*tQx*tQx - 2*tQy*tQy );
	}

	template <class T> wykobi::point2d<T> rotateCorrect(T pAngle, const wykobi::point2d<T> & pPoint, const wykobi::point2d<T> & pCenter) {

		return wykobi::rotate(pAngle,pPoint,pCenter);
	}

	template <class T> wykobi::vector3d<T> getEulerAngles(const wykobi::point3d<T> pCenter, const wykobi::point3d<T> pPoint) {
		wykobi::vector3d<T> tEulerAngles;

		T tRadius = wykobi::distance(pCenter,pPoint);
		T tAlpha = (T)180.0;
		T tDx = pPoint.x - pCenter.x;
		T tDy = pPoint.y - pCenter.y;
		T tDz = pPoint.z - pCenter.z;

		T tTanAlpha = tDy / tDz;

		if (tDz>0) tAlpha = atan(tTanAlpha) * (T)wykobi::_180DivPI + (T)180.0;
		if (tDz<0) tAlpha = atan(tTanAlpha) * (T)wykobi::_180DivPI;
		if (tDz == 0) {
			if (tDx > 0) tAlpha = (T)-180.0;
			if (tDx < 0) tAlpha = (T)180.0;
			if (tDx == 0) tAlpha = (T)0.0;
		}

		T tBeta = asin(tDx / tRadius)*(T)wykobi::_180DivPI;

		tEulerAngles[0] = -tAlpha;
		tEulerAngles[1] = -tBeta;
		tEulerAngles[2] = 0.0f;

		return tEulerAngles;
	}

	template <class T> wykobi::vector3d<T> getEulerAngles(const wykobi::vectornd<T,4> & pQuaternion){

		T tW = pQuaternion[0];
		T tX = pQuaternion[1];
		T tY = pQuaternion[2];
		T tZ = pQuaternion[3];

		wykobi::vector3d<T> tRet;

		T test = tX*tY + tZ*tW;
		if (test > 0.499f) { // singularity at north pole
			tRet[0] = 2.0f * atan2(tX,tW);
			tRet[1] = (float)wykobi::PI / 2.0f;
			tRet[2] = 0.0f;
			return tRet;
		}
		if (test < -0.499f){ // singularity at south pole
			tRet[0] = - 2.0f * atan2(tX,tW);
			tRet[1] = - (float)wykobi::PI/2.0f;
			tRet[2] = 0.0f;
			return tRet;
		}
		T tSqx = tX*tX;
		T tSqy = tY*tY;
		T tSqz = tZ*tZ;
		tRet[0] = atan2(2.0f * tY * tW - 2.0f * tX * tZ , 1.0f - 2.0f * tSqy - 2.0f * tSqz);
		tRet[1] = asin(2.0f * test);
		tRet[2] = atan2(2.0f * tX * tW - 2.0f * tY * tZ , 1.0f - 2.0f * tSqx - 2.0f * tSqz);

		return tRet;
	}

	template <class T> wykobi::vectornd<T,4> getQuaternion(const CvMat * pRotationMatrix) {
	wykobi::vectornd<T,4> tQuaternion;

	T tM00 = pRotationMatrix->data.fl[0];
	T tM01 = pRotationMatrix->data.fl[1];
	T tM02 = pRotationMatrix->data.fl[2];
	T tM10 = pRotationMatrix->data.fl[3];
	T tM11 = pRotationMatrix->data.fl[4];
	T tM12 = pRotationMatrix->data.fl[5];
	T tM20 = pRotationMatrix->data.fl[6];
	T tM21 = pRotationMatrix->data.fl[7];
	T tM22 = pRotationMatrix->data.fl[8];

	T tR = tM00 + tM11 + tM22;
	T tQx;
	T tQy;
	T tQz;
	T tQw;
	T tS;

	if (tR > 0) {
		tS = (T)sqrt(tR+(T)1.0) * (T)2.0;
		tQw = (T)0.25 * tS;
		tQx = (tM21 - tM12) / tS;
		tQy = (tM02 - tM20) / tS;
		tQz = (tM10 - tM01) / tS;
	} else if ((tM00 > tM11)&(tM00 > tM22)) {
		tS = (T)sqrt((T)1.0 + tM00 - tM11 - tM22) * (T)2.0;
		tQw = (tM21 - tM12) / tS;
		tQx = (T)0.25 * tS;
		tQy = (tM01 + tM10) / tS;
		tQz = (tM02 + tM20) / tS;
	} else if (tM11 > tM22) {
		tS = (T)sqrt((T)1.0 + tM11 - tM00 - tM22) * (T)2.0;
		tQw = (tM02 - tM20) / tS;
		tQx = (tM01 + tM10) / tS;
		tQy = (T)0.25 * tS;
		tQz = (tM12 + tM21) / tS;
	} else {
		 tS = (T)sqrt((T)1.0 + tM22 - tM00 - tM11) * (T)2.0;
		 tQw = (tM10 - tM01) / tS;
		 tQx = (tM02 + tM20) / tS;
		 tQy = (tM12 + tM21) / tS;
		 tQz = (T)0.25 * tS;
	}

	tQuaternion[0]= tQw;
	tQuaternion[1]= tQx;
	tQuaternion[2]= tQy;
	tQuaternion[3]= tQz;

	return tQuaternion;
}

	template <class T> wykobi::vectornd<T,4> getQuaternion(const wykobi::vector3d<T> & pVector1, const wykobi::vector3d<T> & pVector2) {
		wykobi::vectornd<T,4> tQuaternion;

		wykobi::vector3d<T> tAxis = Geometry::crossProduct<T>(pVector1, pVector2);
		tQuaternion[0] = sqrt(Geometry::getVectorLengthSqrd<T>(pVector1) * Geometry::getVectorLengthSqrd<T>(pVector2)) + Geometry::dotProduct<T>(pVector1, pVector2);
		tQuaternion[1] = tAxis.x;
		tQuaternion[2] = tAxis.y;
		tQuaternion[3] = tAxis.z;

		return normalize(tQuaternion);
	}

	template <class T> wykobi::vectornd<T,4> getQuaternion(const wykobi::vector3d<T> & pEulerAngles){

		T tC1 = cos(pEulerAngles[0] / 2.0f);
		T tC2 = cos(pEulerAngles[1] / 2.0f);
		T tC3 = cos(pEulerAngles[2] / 2.0f);
		T tS1 = sin(pEulerAngles[0] / 2.0f);
		T tS2 = sin(pEulerAngles[1] / 2.0f);
		T tS3 = sin(pEulerAngles[2] / 2.0f);

		wykobi::vectornd<T,4> tQuaternion;

		tQuaternion[0] = tC1 * tC2 * tC3 - tS1 * tS2 * tS3;
		tQuaternion[1] = tS1 * tS2 * tC3 + tC1 * tC2 * tS3;
		tQuaternion[2] = tS1 * tC2 * tC3 + tC1 * tS2 * tS3;
		tQuaternion[3] = tC1 * tS2 * tC3 - tS1 * tC2 * tS3;

		return tQuaternion;

	}

	template <class T> wykobi::vectornd<T,4> sumQuaternions(const wykobi::vectornd<T,4> & pQuaternion1, const wykobi::vectornd<T,4> & pQuaternion2){

		wykobi::vectornd<T,4> tRet;

		tRet[0] = pQuaternion1[0] + pQuaternion2[0];
		tRet[1] = pQuaternion1[1] + pQuaternion2[1];
		tRet[2] = pQuaternion1[2] + pQuaternion2[2];
		tRet[3] = pQuaternion1[3] + pQuaternion2[3];

		return Geometry::normalize(tRet);

	}

	template <class T> wykobi::vectornd<T,4> multiplyQuaternions(const wykobi::vectornd<T,4> & pQuaternion1, const wykobi::vectornd<T,4> & pQuaternion2){

		wykobi::vectornd<T,4> tRet;

		T tW1 = pQuaternion2[0];
		T tX1 = pQuaternion2[1];
		T tY1 = pQuaternion2[2];
		T tZ1 = pQuaternion2[3];

		T tW2 = pQuaternion1[0];
		T tX2 = pQuaternion1[1];
		T tY2 = pQuaternion1[2];
		T tZ2 = pQuaternion1[3];

		tRet[0] = (tW1 * tW2) - (tX1 * tX2) - (tY1 * tY2) - (tZ1 * tZ2);
		tRet[1] = (tW1 * tX2) + (tX1 * tW2) + (tY1 * tZ2) - (tZ1 * tY2);
		tRet[2] = (tW1 * tY2) - (tX1 * tZ2) + (tY1 * tW2) + (tZ1 * tX2);
		tRet[3] = (tW1 * tZ2) + (tX1 * tY2) - (tY1 * tX2) + (tZ1 * tW2);

		return Geometry::normalize(tRet);

	}

	template <class T> wykobi::vector3d<T> getEulerAngles(const	CvMat * pRotationMatrix) {
		wykobi::vector3d<T> tEulerAngles;

		T tM00 = pRotationMatrix->data.fl[0];
		//T tM01 = pRotationMatrix->data.fl[1];
		T tM02 = pRotationMatrix->data.fl[2];
		T tM10 = pRotationMatrix->data.fl[3];
		T tM11 = pRotationMatrix->data.fl[4];
		T tM12 = pRotationMatrix->data.fl[5];
		T tM20 = pRotationMatrix->data.fl[6];
		//T tM21 = pRotationMatrix->data.fl[7];
		T tM22 = pRotationMatrix->data.fl[8];

		if (tM10 > 0.998) { // singularity at north pole
			tEulerAngles.x = atan2(tM02,tM22);
			tEulerAngles.y = (T)wykobi::PI/2;
			tEulerAngles.z = (T)0.0;
			return tEulerAngles;
		}
		if (tM10 < -0.998) { // singularity at south pole
			tEulerAngles.x = atan2(tM02,tM22);
			tEulerAngles.y = (T)wykobi::PI/2;
			tEulerAngles.z = (T)0.0;
			return tEulerAngles;
		}
		tEulerAngles.x = atan2(-tM20,tM00);
		tEulerAngles.y = atan2(-tM12,tM11);
		tEulerAngles.z = asin(tM10);

		return tEulerAngles;
	}

	template <class T> void split(const wykobi::polygon<T,2> &pSrc, const wykobi::line<T, 2> &pLine,wykobi::polygon<T,2> &pDst1, wykobi::polygon<T,2> &pDst2){
		pDst1.clear();
		pDst2.clear();

		bool tDstSwitch = true;

		for(size_t i = 0; i < pSrc.size(); ++i)
		{
			wykobi::segment<T,2> tEdge = wykobi::edge(pSrc, i);
			if (wykobi::robust_collinear(pLine, tEdge[0]))
			{
				pDst1.push_back(tEdge[0]);
				pDst2.push_back(tEdge[0]);
				typename wykobi::segment<T,2>::PointType tPreviousVertex =
						pSrc[(i+pSrc.size()-1)%pSrc.size()];
				if (wykobi::intersect(wykobi::make_segment(tPreviousVertex, tEdge[1]), pLine)) {
					tDstSwitch = !tDstSwitch;
				}
			} else if (wykobi::intersect(tEdge, pLine) && !wykobi::robust_collinear(pLine, tEdge[1])) {
				(tDstSwitch?pDst1:pDst2).push_back(tEdge[0]);
				typename wykobi::segment<T,2>::PointType tIntersection =
						wykobi::intersection_point(tEdge, pLine);
				pDst1.push_back(tIntersection);
				pDst2.push_back(tIntersection);
				tDstSwitch = !tDstSwitch;
			} else {
				(tDstSwitch?pDst1:pDst2).push_back(tEdge[0]);
			}
		}
	}

	template <class T> wykobi::rectangle<T> bounding_box(const wykobi::polygon<T,2> &pPolygon)
	{
		wykobi::rectangle<T> tBox = wykobi::make_rectangle(pPolygon[0], pPolygon[0]);
		for(size_t i = 1; i < pPolygon.size(); ++i)
		{
			T tX = pPolygon[i].x;
			T tY = pPolygon[i].y;
			tBox[0].x = wykobi::min(tBox[0].x, tX);
			tBox[0].y = wykobi::min(tBox[0].y, tY);
			tBox[1].x = wykobi::max(tBox[1].x, tX);
			tBox[1].y = wykobi::max(tBox[1].y, tY);
		}
		return tBox;
	}

	template <class T> void normalize(const wykobi::rectangle<T> pBoundingBox,wykobi::polygon<T,2> &pPolygon)
	{
		T tXMin = pBoundingBox[0].x;
		T tXNorm = pBoundingBox[1].x -tXMin;
		T tYMin = pBoundingBox[0].y;
		T tYNorm = pBoundingBox[1].y -tYMin;
		for(size_t i = 0; i < pPolygon.size(); ++i)
		{
			pPolygon[i].x = (pPolygon[i].x-tXMin)/tXNorm;
			pPolygon[i].y = (pPolygon[i].y-tYMin)/tYNorm;
		}
	}

	template <class T> wykobi::vectornd<T,4> normalize(const wykobi::vectornd<T,4> & pQuaternion){

		wykobi::vectornd<T,4> tQuaternion;

		float tMagnitude = sqrt(pQuaternion[0] * pQuaternion[0] + pQuaternion[1] * pQuaternion[1] + pQuaternion[2] * pQuaternion[2] + pQuaternion[3] * pQuaternion[3]);

		tQuaternion[0] = pQuaternion[0] / tMagnitude;
		tQuaternion[1] = pQuaternion[1] / tMagnitude;
		tQuaternion[2] = pQuaternion[2] / tMagnitude;
		tQuaternion[3] = pQuaternion[3] / tMagnitude;

		return tQuaternion;

	}

	template <class T> wykobi::vector3d<T> getNormalVector(const wykobi::point3d<T> pPoint1, const wykobi::point3d<T> pPoint2, const wykobi::point3d<T> pPoint3){
		T x1,y1,z1;
		T x2,y2,z2;
		T x3,y3,z3;

		x1 = pPoint1.x;
		x2 = pPoint2.x;
		x3 = pPoint3.x;

		y1 = pPoint1.y;
		y2 = pPoint2.y;
		y3 = pPoint3.y;

		z1 = pPoint1.z;
		z2 = pPoint2.z;
		z3 = pPoint3.z;

		T tA = y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2);
		T tB = z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2);
		T tC = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
		//- D = x1 (y2 z3 - y3 z2) + x2 (y3 z1 - y1 z3) + x3 (y1 z2 - y2 z1)

		wykobi::vector3d<T> tRet = wykobi::make_vector<T>(tA,tB,tC);
		return tRet;

	}

	template <class T> wykobi::vector3d<T> crossProduct(const wykobi::vector3d<T> & pVector1,const wykobi::vector3d<T> & pVector2){

		wykobi::vector3d<T> tRet;

		T a,b,c;
		T d,e,f;

		a= pVector1.x;
		b= pVector1.y;
		c= pVector1.z;

		d= pVector2.x;
		e= pVector2.y;
		f= pVector2.z;

		tRet.x = b*f - c*e;
		tRet.y = c*d - a*f;
		tRet.z = a*e - b*d;
		
		return tRet;
	
	}

	template <class T> T dotProduct(const wykobi::vector3d<T> & pVector1,const wykobi::vector3d<T> & pVector2){

		T a,b,c;
		T d,e,f;

		a= pVector1.x;
		b= pVector1.y;
		c= pVector1.z;

		d= pVector2.x;
		e= pVector2.y;
		f= pVector2.z;
		
		return a*d+b*e+c*f;
	
	}

	template <class T> T getVectorLength(const wykobi::vector3d<T> & pVector){
		return sqrt(pVector.x*pVector.x+pVector.y*pVector.y+pVector.z*pVector.z);
	}

	template <class T> T getVectorLengthSqrd(const wykobi::vector3d<T> & pVector){
		return pVector.x*pVector.x+pVector.y*pVector.y+pVector.z*pVector.z;
	}

	template <class T> wykobi::vector3d<T> getVector(const wykobi::point3d<T> &pPoint1, const wykobi::point3d<T> &pPoint2){
		T x1,y1,z1;
		T x2,y2,z2;

		x1 = pPoint1.x;
		x2 = pPoint2.x;

		y1 = pPoint1.y;
		y2 = pPoint2.y;

		z1 = pPoint1.z;
		z2 = pPoint2.z;

		wykobi::vector3d<T> tRet = wykobi::make_vector<T>(x2-x1,y2-y1,z2-z1);
		return tRet;

	}

	template <class T> wykobi::point3d<T> getPoint(const wykobi::point3d<T> pPoint, const wykobi::vector3d<T> pVector, T pDistance){
		T tPX,tPY,tPZ;
		T tVX,tVY,tVZ;

		tPX = pPoint.x;
		tPY = pPoint.y;
		tPZ = pPoint.z;

		tVX = pVector.x;
		tVY = pVector.y;
		tVZ = pVector.z;

		T tIncr = sqrt((pDistance*pDistance) / ((tVX*tVX)+(tVY*tVY)+(tVZ*tVZ)));

		if (pDistance<0) tIncr = tIncr*(-1.0f);

		wykobi::point3d<T> tRet = wykobi::make_point<T>(
				tPX+tVX*tIncr,
				tPY+tVY*tIncr,
				tPZ+tVZ*tIncr);

		return tRet;

	}

	template <class T> wykobi::point2d<T> getPoint(const wykobi::point2d<T> pPoint, const wykobi::vector2d<T> pVector, T pDistance){

		T tPX,tPY;
		T tVX,tVY;

		tPX = pPoint.x;
		tPY = pPoint.y;

		tVX = pVector.x;
		tVY = pVector.y;

		T tIncr = sqrt((pDistance*pDistance) / ((tVX*tVX)+(tVY*tVY)));

		if (pDistance<0) tIncr = tIncr*(-1.0f);

		wykobi::point2d<T> tRet = wykobi::make_point<T>(
				tPX+tVX*tIncr,
				tPY+tVY*tIncr);

		return tRet;

	}
	
	template wykobi::point3d<float> rotateCorrect(const wykobi::vector3d<float> & pEulerAngles, const wykobi::point3d<float> & pPoint);
	template wykobi::point3d<float> rotateCorrect(const wykobi::vector3d<float> & pEulerAngles, const wykobi::point3d<float> & pPoint, const wykobi::point3d<float> & pCenter);
	template wykobi::point3d<float> rotateCorrect(const CvMat * pRotationMatrix, const wykobi::point3d<float> & pPoint);
	template wykobi::point3d<float> rotateCorrect(const CvMat * pRotationMatrix, const wykobi::point3d<float> & pPoint, const wykobi::point3d<float> & pCenter);
	template wykobi::point2d<float> rotateCorrect(float pAngle, const wykobi::point2d<float> & pPoint, const wykobi::point2d<float> & pCenter);
	template wykobi::point3d<float> rotateCorrect(const wykobi::vectornd<float,4> & pQuaternion, const wykobi::point3d<float> & pPoint);
	template wykobi::point3d<float> rotateCorrect(const wykobi::vectornd<float,4> & pQuaternion, const wykobi::point3d<float> & pPoint, const wykobi::point3d<float> & pCenter);
	template wykobi::vector3d<float> rotateCorrect(const wykobi::vectornd<float,4> & pQuaternion, const wykobi::vector3d<float> & pVector);


	template  void getRotationMatrix(const wykobi::vector3d<float> & pEulerAngles, CvMat * pRotationMatrix);
	template  void getRotationMatrix(const wykobi::vector3d<float> * pNormalizedXAxis,const wykobi::vector3d<float> * pNormalizedYAxis,const wykobi::vector3d<float> * pNormalizedZAxis,CvMat * pRotationMatrix);
	template  void getRotationMatrix(const wykobi::vector3d<float> & pNormalizedXOAxis,const wykobi::vector3d<float> & pNormalizedYOAxis,const wykobi::vector3d<float> & pNormalizedZOAxis,const wykobi::vector3d<float> & pNormalizedXRAxis,const wykobi::vector3d<float> & pNormalizedYRAxis,const wykobi::vector3d<float> & pNormalizedZRAxis,CvMat * pRotationMatrix);
	template  void getRotationMatrix(const wykobi::vectornd<float,4> & pQuaternion, CvMat * pRotationMatrix);

	template  wykobi::vector3d<float> getEulerAngles(const	CvMat * pRotationMatrix);
	template  wykobi::vector3d<float> getEulerAngles(const wykobi::point3d<float> pCenter, const wykobi::point3d<float> pPoint);
	template  wykobi::vector3d<float> getEulerAngles(const wykobi::vectornd<float,4> & pQuaternion);

	template  wykobi::vectornd<float,4> getQuaternion(const	CvMat * pRotationMatrix);
	template  wykobi::vectornd<float,4> getQuaternion(const wykobi::vector3d<float> & pVector1,const wykobi::vector3d<float> & pVector2);
	template  wykobi::vectornd<float,4> getQuaternion(const wykobi::vector3d<float> & pEuler);
	template  wykobi::vectornd<float,4> sumQuaternions(const wykobi::vectornd<float,4> & pQuaternion1, const wykobi::vectornd<float,4> & pQuaternion2);
	template  wykobi::vectornd<float,4> multiplyQuaternions(const wykobi::vectornd<float,4> & pQuaternion1, const wykobi::vectornd<float,4> & pQuaternion2);

	template  void split(const wykobi::polygon<float,2> &pSrc, const wykobi::line<float, 2> &pLine,wykobi::polygon<float,2> &pDst1, wykobi::polygon<float,2> &pDst2);
	template  wykobi::rectangle<float> bounding_box(const wykobi::polygon<float,2> &pPolygon);
	template  void normalize(const wykobi::rectangle<float> pBoundingBox,wykobi::polygon<float,2> &pPolygon);
	template  wykobi::vectornd<float,4> normalize(const wykobi::vectornd<float,4> & pQuaternion);
	template  wykobi::vector3d<float> getNormalVector(const wykobi::point3d<float> pPoint1, const wykobi::point3d<float> pPoint2, const wykobi::point3d<float> pPoint3);
	template  wykobi::vector3d<float> getVector(const wykobi::point3d<float> &pPoint1, const wykobi::point3d<float> &pPoint2);
	template  wykobi::vector3d<float> crossProduct(const wykobi::vector3d<float> & pVector1,const wykobi::vector3d<float> & pVector2);
	template  float dotProduct(const wykobi::vector3d<float> & pVector1,const wykobi::vector3d<float> & pVector2);
	template  float getVectorLength(const wykobi::vector3d<float> & pVector);
	template  float getVectorLengthSqrd(const wykobi::vector3d<float> & pVector);

	template  wykobi::point3d<float> getPoint(const wykobi::point3d<float> pPoint, const wykobi::vector3d<float> pVector, float pDistance);
	template  wykobi::point2d<float> getPoint(const wykobi::point2d<float> pPoint, const wykobi::vector2d<float> pVector, float pDistance);
}

