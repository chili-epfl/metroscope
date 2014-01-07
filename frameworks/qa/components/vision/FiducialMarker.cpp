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

#include "FiducialMarker.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>

wykobi::point2d<float> FiducialMarker::getCenter() const {
	wykobi::quadix<float, 2> tCorners = getCorners();
	wykobi::segment<float,2> tTLBR = wykobi::make_segment<float>(tCorners[0],tCorners[2]);
	wykobi::segment<float,2> tTRBL = wykobi::make_segment<float>(tCorners[1],tCorners[3]);
	return wykobi::intersection_point(tTLBR,tTRBL);
}

wykobi::point2d<float> FiducialMarker::getXUnit() const { //FIXME wrong formula
	wykobi::quadix<float, 2> tCorners = getCorners();
	return 0.5f*(tCorners[1]-tCorners[0]);
}

wykobi::point2d<float> FiducialMarker::getYUnit() const { //FIXME wrong formula
	wykobi::quadix<float, 2> tCorners = getCorners();
	return 0.5f*(tCorners[1]-tCorners[2]);
}

float FiducialMarker::getScale() const {
	wykobi::quadix<float, 2> tCorners = getCorners();
	return 0.5f*wykobi::distance(tCorners[0], tCorners[1]);
}

float FiducialMarker::getAngle() const {
	wykobi::quadix<float, 2> tCorners = getCorners();
	wykobi::point2d<float> tRightVector = tCorners[1]-tCorners[0];
	return wykobi::cartesian_angle(tRightVector);
}

void FiducialMarker::ComputeBasisFromSquare(const wykobi::quadix<float, 2> pCorners, wykobi::point2d<float> &pOrigin, wykobi::point2d<float> &pXUnit, wykobi::point2d<float> &pYUnit)
{
	pOrigin = 0.25f*(pCorners[0]+pCorners[1]+pCorners[2]+pCorners[3]);
	wykobi::point2d<float> tUpRight = pCorners[1]-pCorners[3];
	wykobi::point2d<float> tDownRight = pCorners[2]-pCorners[0];
	pXUnit = 0.25f*(tUpRight+tDownRight);
	pYUnit = 0.25f*(tUpRight-tDownRight);
}

wykobi::point3d<float> FiducialMarker::WorldCenter(float pMarkerSideLength, float pCameraWidth, float pCameraHeight, float pFocalLengthX, float pFocalLengthY) const {
	wykobi::quadix<float, 3> tCorners = WorldCorners(pMarkerSideLength, pCameraWidth, pCameraHeight, pFocalLengthX, pFocalLengthY);
	wykobi::point3d<float> tResult;

	tResult.x = (tCorners[0].x + tCorners[1].x + tCorners[2].x + tCorners[3].x)/4.0f;
	tResult.y = (tCorners[0].y + tCorners[1].y + tCorners[2].y + tCorners[3].y)/4.0f;
	tResult.z = (tCorners[0].z + tCorners[1].z + tCorners[2].z + tCorners[3].z)/4.0f;
	
	return tResult;
}

wykobi::quadix<float, 3> FiducialMarker::WorldCorners(
	float pMarkerSideLength,
	float pCameraWidth,
	float pCameraHeight,
	float pFocalLengthX,
	float pFocalLengthY)const {

	//static const float scFixedZ = 1400.0f;

	float tEps,tKx,tKy,tK;
	float tPhysDist=pMarkerSideLength*(sqrtf(2.0f)/2.0f);

	wykobi::quadix<float, 2> tCorners = getCorners();
	wykobi::point2d<float> t2DPoints[3];
	wykobi::point2d<float> tOrigin = getCenter();

	//Vertexes 0 and 2
	t2DPoints[0].x=tCorners[0].x - pCameraWidth/2.0f;
	t2DPoints[1].x=tOrigin.x - pCameraWidth/2.0f;
	t2DPoints[2].x=tCorners[2].x - pCameraWidth/2.0f;

	t2DPoints[0].y=tCorners[0].y - pCameraHeight/2.0f;
	t2DPoints[1].y=tOrigin.y - pCameraHeight/2.0f;
	t2DPoints[2].y=tCorners[2].y - pCameraHeight/2.0f;

	
	if (abs(t2DPoints[2].x-t2DPoints[1].x)>abs(t2DPoints[2].y-t2DPoints[1].y)){
		tEps=(float)(t2DPoints[1].x-t2DPoints[0].x)/(t2DPoints[2].x-t2DPoints[1].x);
	}
	else{
		tEps=(float)(t2DPoints[1].y-t2DPoints[0].y)/(t2DPoints[2].y-t2DPoints[1].y);
	}
	
	tKx=((tEps*tEps)*(t2DPoints[2].x*t2DPoints[2].x) + (-2.0f * tEps * t2DPoints[0].x * t2DPoints[2].x) + t2DPoints[0].x*t2DPoints[0].x)/(pFocalLengthX*pFocalLengthX);
	tKy=((tEps*tEps)*(t2DPoints[2].y*t2DPoints[2].y) + (-2.0f * tEps * t2DPoints[0].y * t2DPoints[2].y) + t2DPoints[0].y*t2DPoints[0].y)/(pFocalLengthY*pFocalLengthY);
	tK=(tEps*tEps) - 2.0f*tEps + 1.0f;

	wykobi::quadix<float, 3> tWorldCorners;

	tWorldCorners[0].z=sqrt((float)(4*tPhysDist*tPhysDist)/(tKx+tKy+tK));
//	mWorldCorners[0].z= scFixedZ;
	tWorldCorners[0].x=(tWorldCorners[0].z*t2DPoints[0].x)/pFocalLengthX;
	tWorldCorners[0].y=(tWorldCorners[0].z*t2DPoints[0].y)/pFocalLengthY;

	tWorldCorners[2].z=tWorldCorners[0].z*tEps;
//	mWorldCorners[2].z=scFixedZ;
	tWorldCorners[2].x=(tWorldCorners[2].z*t2DPoints[2].x)/pFocalLengthX;
	tWorldCorners[2].y=(tWorldCorners[2].z*t2DPoints[2].y)/pFocalLengthY;

	//Vertexes 1 and 3
	t2DPoints[0].x=tCorners[1].x - pCameraWidth/2.0f;
	t2DPoints[1].x=tOrigin.x - pCameraWidth/2.0f;
	t2DPoints[2].x=tCorners[3].x - pCameraWidth/2.0f;

	t2DPoints[0].y=tCorners[1].y - pCameraHeight/2.0f;
	t2DPoints[1].y=tOrigin.y - pCameraHeight/2.0f;
	t2DPoints[2].y=tCorners[3].y - pCameraHeight/2.0f;

	
	if (abs(t2DPoints[2].x-t2DPoints[1].x)>abs(t2DPoints[2].y-t2DPoints[1].y)){
		tEps=(float)(t2DPoints[1].x-t2DPoints[0].x)/(t2DPoints[2].x-t2DPoints[1].x);
	}
	else{
		tEps=(float)(t2DPoints[1].y-t2DPoints[0].y)/(t2DPoints[2].y-t2DPoints[1].y);
	}
	
	tKx=((tEps*tEps)*(t2DPoints[2].x*t2DPoints[2].x) + (-2.0f * tEps * t2DPoints[0].x * t2DPoints[2].x) + t2DPoints[0].x*t2DPoints[0].x)/(pFocalLengthX*pFocalLengthX);
	tKy=((tEps*tEps)*(t2DPoints[2].y*t2DPoints[2].y) + (-2.0f * tEps * t2DPoints[0].y * t2DPoints[2].y) + t2DPoints[0].y*t2DPoints[0].y)/(pFocalLengthY*pFocalLengthY);
	tK=(tEps*tEps) - 2.0f*tEps + 1.0f;

	tWorldCorners[1].z=sqrt((float)(4*tPhysDist*tPhysDist)/(tKx+tKy+tK));
//	mWorldCorners[1].z=scFixedZ;
	tWorldCorners[1].x=(tWorldCorners[1].z*t2DPoints[0].x)/pFocalLengthX;
	tWorldCorners[1].y=(tWorldCorners[1].z*t2DPoints[0].y)/pFocalLengthY;

	tWorldCorners[3].z=tWorldCorners[1].z*tEps;
//	mWorldCorners[3].z=scFixedZ;
	tWorldCorners[3].x=(tWorldCorners[3].z*t2DPoints[2].x)/pFocalLengthX;
	tWorldCorners[3].y=(tWorldCorners[3].z*t2DPoints[2].y)/pFocalLengthY;

	return tWorldCorners;
}

wykobi::quadix<float, 3> FiducialMarker::WorldCorners2(
	float pMarkerSideLength,
	float pCameraWidth,
	float pCameraHeight,
	float pFocalLengthX,
	float pFocalLengthY) const {

	wykobi::quadix<float, 2> tCorners = getCorners();

	float tXS1=tCorners[0].x - pCameraWidth/2.0f;
	float tXS2=tCorners[1].x - pCameraWidth/2.0f;
	float tXS3=tCorners[3].x - pCameraWidth/2.0f;
	float tXS4=tCorners[2].x - pCameraWidth/2.0f;

	float tYS1=tCorners[0].y - pCameraHeight/2.0f;
	float tYS2=tCorners[1].y - pCameraHeight/2.0f;
	float tYS3=tCorners[3].y - pCameraHeight/2.0f;
	float tYS4=tCorners[2].y - pCameraHeight/2.0f;

	float tXS1_2 = tXS1*tXS1;
	float tXS2_2 = tXS2*tXS2;
	float tXS3_2 = tXS3*tXS3;
	float tXS4_2 = tXS4*tXS4;
	float tYS1_2 = tYS1*tYS1;
	float tYS2_2 = tYS2*tYS2;
	float tYS3_2 = tYS3*tYS3;
	float tYS4_2 = tYS4*tYS4;
	float tFx_2 = pFocalLengthX*pFocalLengthX;
	float tFy_2 = pFocalLengthY*pFocalLengthY;
	float tDiag_2 = pMarkerSideLength*pMarkerSideLength+pMarkerSideLength*pMarkerSideLength;
	float tFx = pFocalLengthX;
	float tFy = pFocalLengthY;

	float tZ4Temp1 = 
		4.0f*tXS1_2*tFy_2*tXS4_2*tYS3_2+
		tFx_2*tFy_2*tXS2_2*tYS4_2+
		4.0f*tYS1_2*tFx_2*tXS3_2*tYS4_2+
		tYS1_2*tFx_2*tXS3_2*tYS2_2+
		4.0f*tXS1_2*tFy_2*tXS4_2*tYS2_2+
		tXS1_2*tFy_2*tXS2_2*tYS3_2+
		tFx_2*tFy_2*tXS4_2*tYS3_2+
		tXS4_2*tFy_2*tXS2_2*tYS3_2+
		tFx_2*tFy_2*tXS4_2*tYS2_2+
		tXS4_2*tFy_2*tXS2_2*tYS1_2+
		tXS4_2*tFy_2*tXS3_2*tYS2_2+
		tXS4_2*tFy_2*tXS3_2*tYS1_2+
		tYS1_2*tFx_2*tXS4_2*tYS3_2+
		tYS1_2*tFx_2*tXS4_2*tYS2_2+
		tYS4_2*tFx_2*tXS3_2*tYS2_2-2*tYS4_2*tFx_2*tXS1*tYS3_2*tXS2-4*tFx_2*tFy_2*tXS1*tYS2_2*tXS3-4*tFx_2*tFy_2*tXS1*tYS3_2*tXS2+
		4.0f*tFx_2*tFy_2*tXS1*tYS3*tXS3*tYS2+
		4.0f*tFx_2*tFy_2*tXS2*tYS1*tXS3*tYS2-4*tFx_2*tFy_2*tXS2_2*tYS1*tYS3-4*tFx_2*tFy_2*tXS3_2*tYS1*tYS2+
		4.0f*tFx_2*tFy_2*tXS2_2*tYS3_2+
		4.0f*tFx_2*tFy_2*tXS3_2*tYS2_2-8*tFx_2*tFy_2*tXS2*tYS3*tXS3*tYS2+
		2.0f*tFx_2*tFy_2*tXS3*tYS1*tXS1*tYS2+
		tFx_2*tFy_2*tXS2_2*tYS1_2+
		4.0f*tFx_2*tFy_2*tXS3*tYS1*tXS2*tYS3+
		4.0f*tFx_2*tFy_2*tXS1*tYS2*tXS2*tYS3-2*tFx_2*tFy_2*tXS2*tYS1*tXS1*tYS2-2*tFx_2*tFy_2*tXS3*tYS1*tXS1*tYS3+
		tFx_2*tFy_2*tXS1_2*tYS3_2+
		tFx_2*tFy_2*tXS3_2*tYS1_2-2*tFx_2*tFy_2*tXS3*tYS1_2*tXS2+
		2.0f*tFx_2*tFy_2*tXS2*tYS1*tXS1*tYS3-2*tFx_2*tFy_2*tXS1_2*tYS3*tYS2+
		tFx_2*tFy_2*tXS1_2*tYS2_2-2*tYS4_2*tFx_2*tXS3*tYS2_2*tXS1+
		2.0f*tFx_2*tFy_2*tXS2*tYS4*tXS4*tYS3+
		tYS4_2*tFx_2*tXS1_2*tYS2_2-2*tYS4_2*tFx_2*tXS1_2*tYS3*tYS2-4*tYS1_2*tFx_2*tXS2_2*tYS3*tYS4-2*tFx_2*tFy_2*tXS3*tYS4*tXS4*tYS3-4*tFx_2*tFy_2*tXS3_2*tYS4*tYS2+
		4.0f*tYS1_2*tFx_2*tXS2*tYS4*tXS3*tYS2-2*tFx_2*tFy_2*tXS3*tYS4_2*tXS2-8*tYS1_2*tFx_2*tXS2*tYS4_2*tXS3-2*tYS1_2*tFx_2*tXS4_2*tYS3*tYS2+
		4.0f*tYS1_2*tFx_2*tXS2*tYS4*tXS4*tYS3-2*tYS1_2*tFx_2*tXS4*tYS2_2*tXS3-4*tFx_2*tFy_2*tXS4*tYS2_2*tXS3+
		2.0f*tFx_2*tFy_2*tXS1*tYS2_2*tXS4+
		tYS4_2*tFx_2*tXS2_2*tYS3_2+
		2.0f*tFx_2*tFy_2*tXS1*tYS2*tXS3*tYS4+
		2.0f*tFx_2*tFy_2*tXS3*tYS1*tXS4*tYS2-2*tFx_2*tFy_2*tXS4_2*tYS3*tYS2-4*tFx_2*tFy_2*tXS4*tYS3_2*tXS2-4*tXS1*tXS4_2*tFy_2*tXS2*tYS3_2-4*tYS1*tYS4_2*tFx_2*tXS2_2*tYS3+
		2.0f*tYS1*tYS4*tFx_2*tXS2_2*tYS3_2+
		2.0f*tYS1*tYS4*tFx_2*tXS3_2*tYS2_2+
		4.0f*tFx_2*tFy_2*tXS3*tYS4*tXS2*tYS3-4*tYS1*tYS4_2*tFx_2*tXS3_2*tYS2+
		2.0f*tYS1_2*tFx_2*tXS4*tYS3*tXS3*tYS2+
		4.0f*tYS1_2*tFx_2*tXS4*tYS2*tXS3*tYS4+
		2.0f*tFx_2*tFy_2*tXS3_2*tYS1*tYS4+
		4.0f*tFx_2*tFy_2*tXS4*tYS3*tXS3*tYS2+
		2.0f*tFx_2*tFy_2*tXS2_2*tYS1*tYS4-8*tXS1_2*tFy_2*tXS4_2*tYS3*tYS2+
		4.0f*tXS1_2*tFy_2*tXS4*tYS2*tXS2*tYS3-4*tXS1_2*tFy_2*tXS4*tYS3_2*tXS2+
		4.0f*tFx_2*tFy_2*tXS2*tYS4*tXS3*tYS2-2*tXS1_2*tFy_2*tXS2_2*tYS4*tYS3-2*tXS1_2*tFy_2*tXS2*tYS4_2*tXS3-2*tXS1_2*tFy_2*tXS3_2*tYS4*tYS2-4*tYS1_2*tFx_2*tXS3_2*tYS4*tYS2-2*tYS1_2*tFx_2*tXS2*tYS3_2*tXS4-4*tXS1_2*tFy_2*tXS3*tYS2_2*tXS4-4*tXS1*tXS4_2*tFy_2*tXS3*tYS2_2-2*tYS1_2*tFx_2*tXS2*tYS3*tXS3*tYS2+
		2.0f*tXS1*tXS4*tFy_2*tXS2_2*tYS3_2+
		2.0f*tYS1_2*tFx_2*tXS2*tYS3*tXS4*tYS2+
		2.0f*tXS1*tXS4*tFy_2*tXS3_2*tYS2_2-2*tXS1_2*tFy_2*tXS3*tYS2*tXS2*tYS3-4*tFx_2*tFy_2*tXS2_2*tYS4*tYS3+
		2.0f*tXS1*tXS4*tFy_2*tXS2_2*tYS1*tYS4-2*tXS1*tXS4*tFy_2*tXS2_2*tYS3*tYS4+
		4.0f*tXS1*tXS4_2*tFy_2*tXS2*tYS1*tYS3+
		4.0f*tYS1_2*tFx_2*tXS2*tYS3*tXS3*tYS4-4*tXS1*tXS4_2*tFy_2*tXS3*tYS1*tYS3+
		2.0f*tXS1*tXS4*tFy_2*tXS3_2*tYS1*tYS4-2*tXS1*tXS4*tFy_2*tXS3_2*tYS1*tYS2-2*tXS4_2*tFy_2*tXS2_2*tYS1*tYS3-2*tXS4_2*tFy_2*tXS3_2*tYS1*tYS2+
		tYS4_2*tFx_2*tXS1_2*tYS3_2-2*tXS4_2*tFy_2*tXS3*tYS1_2*tXS2-2*tFx_2*tFy_2*tXS2*tYS4*tXS4*tYS2+
		2.0f*tFx_2*tFy_2*tXS3*tYS4*tXS4*tYS2+
		2.0f*tFx_2*tFy_2*tXS1*tYS3_2*tXS4-2*tYS1*tYS4*tFx_2*tXS3*tYS2_2*tXS4+
		2.0f*tYS1*tYS4*tFx_2*tXS1*tYS2_2*tXS4-4*tXS1*tXS4_2*tFy_2*tXS2*tYS1*tYS2+
		4.0f*tYS1*tYS4_2*tFx_2*tXS1*tYS2*tXS3+
		tXS1_2*tFy_2*tXS3_2*tYS4_2-2*tYS1*tYS4*tFx_2*tXS1*tYS2_2*tXS3+
		tXS1_2*tFy_2*tXS2_2*tYS4_2-2*tYS1*tYS4*tFx_2*tXS1*tYS3_2*tXS2-4*tYS1*tYS4_2*tFx_2*tXS1*tYS2*tXS2-2*tYS1*tYS4*tFx_2*tXS2*tYS3_2*tXS4+
		4.0f*tXS1*tXS4_2*tFy_2*tXS3*tYS1*tYS2-2*tXS1*tXS4*tFy_2*tXS3_2*tYS2*tYS4+
		4.0f*tYS1*tYS4_2*tFx_2*tXS2*tYS3*tXS3+
		4.0f*tXS1*tXS4_2*tFy_2*tXS2*tYS3*tYS2+
		4.0f*tFx_2*tFy_2*tXS4*tYS2*tXS2*tYS3-2*tXS1*tXS4*tFy_2*tXS2_2*tYS1*tYS3+
		4.0f*tXS1*tXS4_2*tFy_2*tXS3*tYS2*tYS3+
		4.0f*tXS1_2*tFy_2*tXS2*tYS4*tXS4*tYS3+
		2.0f*tYS1*tYS4*tFx_2*tXS1*tYS3_2*tXS4+
		2.0f*tXS4_2*tFy_2*tXS3*tYS1*tXS2*tYS3-4*tYS1*tYS4_2*tFx_2*tXS1*tYS3*tXS3+
		4.0f*tYS1*tYS4_2*tFx_2*tXS1*tYS3*tXS2+
		tFx_2*tFy_2*tXS3_2*tYS4_2+
		4.0f*tYS1*tYS4_2*tFx_2*tXS3*tYS2*tXS2-2*tFx_2*tFy_2*tXS1*tYS3*tXS3*tYS4-4*tXS1_2*tFy_2*tXS4*tYS3*tXS3*tYS4+
		4.0f*tXS1_2*tFy_2*tXS4*tYS3*tXS3*tYS2-4*tFx_2*tFy_2*tXS3*tYS1*tXS2*tYS4-2*tFx_2*tFy_2*tXS3*tYS1*tXS4*tYS3-2*tFx_2*tFy_2*tXS1*tYS2*tXS2*tYS4+
		2.0f*tXS1_2*tFy_2*tXS3*tYS4*tXS2*tYS3+
		2.0f*tXS1_2*tFy_2*tXS2*tYS4*tXS3*tYS2-4*tYS1_2*tFx_2*tXS2*tYS4*tXS4*tYS2-4*tYS1_2*tFx_2*tXS4*tYS3*tXS3*tYS4+
		4.0f*tXS1_2*tFy_2*tXS3*tYS4*tXS4*tYS2+
		4.0f*tYS1_2*tFx_2*tXS2_2*tYS4_2+
		tXS1_2*tFy_2*tXS3_2*tYS2_2+
		tYS1_2*tFx_2*tXS2_2*tYS3_2-2*tXS4_2*tFy_2*tXS3*tYS2*tXS2*tYS3+
		2.0f*tXS4_2*tFy_2*tXS3*tYS2*tXS2*tYS1-4*tXS1_2*tFy_2*tXS2*tYS4*tXS4*tYS2+
		2.0f*tXS1*tXS4*tFy_2*tXS2*tYS1*tXS3*tYS2+
		2.0f*tYS1*tYS4*tFx_2*tXS1*tYS3*tXS3*tYS2+
		2.0f*tYS1*tYS4*tFx_2*tXS2*tYS3*tXS4*tYS2+
		2.0f*tYS1*tYS4*tFx_2*tXS3*tYS2*tXS4*tYS3+
		2.0f*tYS1*tYS4*tFx_2*tXS1*tYS2*tXS2*tYS3-4*tYS1*tYS4*tFx_2*tXS1*tYS3*tXS4*tYS2+
		2.0f*tFx_2*tFy_2*tXS1*tYS3*tXS2*tYS4-4*tFx_2*tFy_2*tXS1*tYS3*tXS4*tYS2+
		2.0f*tFx_2*tFy_2*tXS2*tYS1*tXS4*tYS3-2*tFx_2*tFy_2*tXS2*tYS1*tXS4*tYS2+
		2.0f*tYS4_2*tFx_2*tXS1*tYS2*tXS2*tYS3+
		2.0f*tYS4_2*tFx_2*tXS3*tYS2*tXS1*tYS3-2*tYS4_2*tFx_2*tXS3*tYS2*tXS2*tYS3+
		2.0f*tXS1*tXS4*tFy_2*tXS3*tYS1*tXS2*tYS3-4*tYS1*tYS4*tFx_2*tXS2*tYS3*tXS3*tYS2-4*tXS1*tXS4*tFy_2*tXS3*tYS1*tXS2*tYS4-4*tXS1*tXS4*tFy_2*tXS3*tYS2*tXS2*tYS3+
		2.0f*tXS1*tXS4*tFy_2*tXS3*tYS2*tXS2*tYS4+
		2.0f*tXS1*tXS4*tFy_2*tXS2*tYS3*tXS3*tYS4;

	float tZ4 = (( -1.0f/(tZ4Temp1)*sqrt((tZ4Temp1)*(tDiag_2))*(-tXS3*tYS1+tXS3*tYS2+tXS2*tYS1+tXS1*tYS3-tXS1*tYS2-tXS2*tYS3)*tFx*tFy));
	float tZ1 = tZ4*(tXS3*tYS4-tXS4*tYS3-tXS2*tYS4+tXS4*tYS2+tXS2*tYS3-tXS3*tYS2)/(-tXS2*tYS3+tXS2*tYS1+tXS1*tYS3+tXS3*tYS2-tXS3*tYS1-tXS1*tYS2);
	float tZ2 = (tXS3*tYS4-tXS4*tYS3+tYS1*tXS4-tYS4*tXS1-tXS3*tYS1+tXS1*tYS3)*tZ4/(-tXS2*tYS3+tXS2*tYS1+tXS1*tYS3+tXS3*tYS2-tXS3*tYS1-tXS1*tYS2);
	float tZ3 = -tZ4*(tXS2*tYS4-tXS2*tYS1-tYS4*tXS1-tXS4*tYS2+tYS1*tXS4+tXS1*tYS2)/(-tXS2*tYS3+tXS2*tYS1+tXS1*tYS3+tXS3*tYS2-tXS3*tYS1-tXS1*tYS2);

	float tX1 = tZ1 * tXS1 / tFx;
	float tX2 = tZ2 * tXS2 / tFx;
	float tX3 = tZ3 * tXS3 / tFx;
	float tX4 = tZ4 * tXS4 / tFx;
	float tY1 = tZ1 * tYS1 / tFy;
	float tY2 = tZ2 * tYS2 / tFy;
	float tY3 = tZ3 * tYS3 / tFy;
	float tY4 = tZ4 * tYS4 / tFy;

	wykobi::quadix<float, 3> tWorldCorners;

	tWorldCorners[0].x=tX1;
	tWorldCorners[0].y=tY1;
	tWorldCorners[0].z=tZ1;

	tWorldCorners[1].x=tX2;
	tWorldCorners[1].y=tY2;
	tWorldCorners[1].z=tZ2;

	tWorldCorners[2].x=tX3;
	tWorldCorners[2].y=tY3;
	tWorldCorners[2].z=tZ3;

	tWorldCorners[3].x=tX4;
	tWorldCorners[3].y=tY4;
	tWorldCorners[3].z=tZ4;

	return tWorldCorners;
}

wykobi::point3d<float> FiducialMarker::EulerAngles(wykobi::quadix<float, 3> &pWorldCorners) const {

	float tDegrees2Radians = 180.0f / (float)wykobi::PI;

	wykobi::vector3d<float> tLURU(pWorldCorners[1]-pWorldCorners[0]);
	wykobi::vector3d<float> tLULD(pWorldCorners[3]-pWorldCorners[0]);

	CvMat *tRotationMatrix = cvCreateMat(3,3,CV_32FC1);

	float tLURUNorm = sqrtf(tLURU.x*tLURU.x + tLURU.y*tLURU.y + tLURU.z*tLURU.z);
	float tLULDNorm = sqrtf(tLULD.x*tLULD.x + tLULD.y*tLULD.y + tLULD.z*tLULD.z);

	float tNorm = tLURUNorm*tLULDNorm;

	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			tRotationMatrix->data.fl[i*3+j] = (tLURU[j]*tLULD[i])/tNorm;
		}
	}

	wykobi::point3d<float> tEulerAngles;

	if(tRotationMatrix->data.fl[6] != 1.0f && tRotationMatrix->data.fl[6] != -1.0f)
	{
		float tRotMat20;
		tRotMat20 = (tRotationMatrix->data.fl[6]<-1.0f) ? -1.0f : tRotationMatrix->data.fl[6];
		tRotMat20 = (tRotationMatrix->data.fl[6]>1.0f) ? 1.0f : tRotationMatrix->data.fl[6];

		float rotY = -asinf(tRotMat20);
		tEulerAngles.y = rotY * tDegrees2Radians;
		tEulerAngles.x = atan2(tRotationMatrix->data.fl[7]/cos(rotY), tRotationMatrix->data.fl[8]/cos(rotY)) * tDegrees2Radians;
		tEulerAngles.z = atan2(tRotationMatrix->data.fl[3]/cos(rotY), tRotationMatrix->data.fl[0]/cos(rotY)) * tDegrees2Radians;
	}
	else
	{
		tEulerAngles.z = 0.0f;
		if(tRotationMatrix->data.fl[6] == -1.0f)
		{
			tEulerAngles.y = 90.0f;
			tEulerAngles.x = atan2(tRotationMatrix->data.fl[1], tRotationMatrix->data.fl[2]) * tDegrees2Radians;
		}
		else
		{
			tEulerAngles.y = -90.0f;
			tEulerAngles.x = atan2(-tRotationMatrix->data.fl[1], -tRotationMatrix->data.fl[1]) * tDegrees2Radians;
		}
	}
	return tEulerAngles;
}
