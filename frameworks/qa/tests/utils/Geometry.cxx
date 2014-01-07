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

#include <iostream>
#include <qa/utils/Tests.hpp>
#include <qa/utils/Geometry.hpp>
#include <qa/utils/Camera3DProjections.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <qa/pipeables/io/OpenGl3DDisplay.hpp>
#include "GeometryDisplayer.hpp"
#include <qa/pipeables/misc/Stopper.hpp>
#include <qa/pipeables/io/CvShower.hpp>
#include <qa/pipeables/io/CvWaiter.hpp>

//#define DEBUG_Geometry

void testAngles() {

	int tWidth = 1280;
	int tHeight = 960;
	int tChannels = 1;
	float tFrameRate = 15;
	float tAlpha=0.0f;
	float tCx=670.117981f;
	float tCy=597.9520264f;
	float tFx=2136.883057f;
	float tFy=2154.24707f;
	float tK1=-0.3f;
	float tK2=0.1849f;
	float tK3=0.003872f;
	float tK4=0.005652f;

	Camera *tCamera = new Camera(
			tWidth,
			tHeight,
			tChannels,
			tFrameRate,
			tAlpha,
			tCx,
			tCy,
			tFx,
			tFy,
			tK1,
			tK2,
			tK3,
			tK4);

	wykobi::point3d<float> tCenter = wykobi::make_point<float>(0,0,50);
	wykobi::point3d<float> tXAxis = wykobi::make_point<float>(10,0,50);
	wykobi::point3d<float> tYAxis = wykobi::make_point<float>(0,10,50);
	wykobi::point3d<float> tZAxis = wykobi::make_point<float>(0,0,60);

//	wykobi::vector3d<float> tEulerAngles = Geometry::getEulerAngles<float>(tCenter,tPoint);
	wykobi::vector3d<float> tEulerAngles = wykobi::make_vector<float>(0,0,0);

	IplImage* tImage = cvCreateImage(cvSize(tWidth,tHeight),IPL_DEPTH_8U,3);

	//OpenGl3DDisplay *tOpenGl3DDisplay = new OpenGl3DDisplay(0,0,tWidth,tHeight);
	//tOpenGl3DDisplay->setCameraProjectionMatrix(tCamera,0,1000);

	//GeometryDisplayer *tGeometryDisplayer = new GeometryDisplayer(&tCenter,&tPoint,&tEulerAngles);

	CvWaiter *tWaiter = (new CvWaiter())
		->BlockingTimeMillis(20);
	CvShower *tShower =	new CvShower('1',  &tImage, tWaiter->KeyPressed());
	tShower->StartingPosition(cvPoint(0,0));
	Stopper *tStopper = new Stopper(tWaiter->KeyPressed());
	tStopper->StopKey('q');

	*tShower
	| *tWaiter
	| *tStopper;

	CvMat *tRotationMatrix = cvCreateMat(3,3,CV_32FC1);

	for (int tTime = 0; tTime <1500; tTime++){

		Geometry::getRotationMatrix(tEulerAngles,tRotationMatrix);

		wykobi::point3d<float> tNewXAxis = Geometry::rotateCorrect(tRotationMatrix,tXAxis,tCenter);
		wykobi::point3d<float> tNewYAxis = Geometry::rotateCorrect(tRotationMatrix,tYAxis,tCenter);
		wykobi::point3d<float> tNewZAxis = Geometry::rotateCorrect(tRotationMatrix,tZAxis,tCenter);

		wykobi::point2d<float> tCenter2D = Camera3DProjections::project(tCenter,tCamera);
		wykobi::point2d<float> tNewXAxis2D = Camera3DProjections::project(tNewXAxis,tCamera);
		wykobi::point2d<float> tNewYAxis2D = Camera3DProjections::project(tNewYAxis,tCamera);
		wykobi::point2d<float> tNewZAxis2D = Camera3DProjections::project(tNewZAxis,tCamera);

		cvCircle(tImage,cvPoint(tNewZAxis2D.x,tNewZAxis2D.y),3,	cvScalar(50,255,255),	0.5f*tFx/tNewZAxis.z);

		cvLine(tImage,cvPoint(tCenter2D.x,tCenter2D.y),cvPoint(tNewXAxis2D.x,tNewXAxis2D.y),cvScalar(255,250,250),2);
		cvLine(tImage,cvPoint(tCenter2D.x,tCenter2D.y),cvPoint(tNewYAxis2D.x,tNewYAxis2D.y),cvScalar(255,250,250),2);
		cvLine(tImage,cvPoint(tCenter2D.x,tCenter2D.y),cvPoint(tNewZAxis2D.x,tNewZAxis2D.y),cvScalar(255,250,250),2);

		cvCircle(tImage,cvPoint(tCenter2D.x,tCenter2D.y),3,		cvScalar(0,0,255),		0.5f*tFx/tCenter.z);
		cvCircle(tImage,cvPoint(tNewXAxis2D.x,tNewXAxis2D.y),3,	cvScalar(255,50,50),	0.5f*tFx/tNewXAxis.z);
		cvCircle(tImage,cvPoint(tNewYAxis2D.x,tNewYAxis2D.y),3,	cvScalar(50,255,50),	0.5f*tFx/tNewYAxis.z);

		tShower->start();
		if (tTime>0 && tTime<500)
			tEulerAngles.x+=0.3f;
		if (tTime>500 && tTime<1000)
			tEulerAngles.y+=0.3f;
		if (tTime>1000 && tTime<1500)
			tEulerAngles.z+=0.1f;
		cvZero(tImage);
	}


	//delete tGeometryDisplayer;

	//delete tOpenGl3DDisplay;
	cvReleaseMat(&tRotationMatrix);
	cvReleaseImage(&tImage);
	delete tCamera;
	exit(-1);
}

int main()
{

	testAngles();

    Tests::TestSection("Geometry::split");

    Tests::TestSection("Functional: square split in the middle");
	wykobi::polygon<float,2> pTempSrc;
	pTempSrc.push_back(wykobi::make_point(-1.0f, 1.0f));
	pTempSrc.push_back(wykobi::make_point( 1.0f, 1.0f));
	pTempSrc.push_back(wykobi::make_point( 1.0f,-1.0f));
	pTempSrc.push_back(wykobi::make_point(-1.0f,-1.0f));
	wykobi::line<float, 2> pLine = wykobi::make_line(-1.0f, 0.0f, 1.0f, 0.0f);
	const wykobi::polygon<float,2> pSrc(pTempSrc);
	wykobi::polygon<float,2> pDst1;
	wykobi::polygon<float ,2> pDst2;

	Geometry::split(pSrc, pLine, pDst1, pDst2);

	wykobi::polygon<float,2> pExpected1;
	pExpected1.push_back(wykobi::make_point(-1.0f, 1.0f));
	pExpected1.push_back(wykobi::make_point( 1.0f, 1.0f));
	pExpected1.push_back(wykobi::make_point( 1.0f, 0.0f));
	pExpected1.push_back(wykobi::make_point(-1.0f, 0.0f));
	wykobi::polygon<float ,2> pExpected2;
	pExpected2.push_back(wykobi::make_point( 1.0f, 0.0f));
	pExpected2.push_back(wykobi::make_point( 1.0f,-1.0f));
	pExpected2.push_back(wykobi::make_point(-1.0f,-1.0f));
	pExpected2.push_back(wykobi::make_point(-1.0f, 0.0f));

#ifdef DEBUG_Geometry
	Tests::verifyEqual(pDst1.size(), pExpected1.size());
	for (size_t i = 0; i<pDst1.size(); ++i) Tests::verifyEqual(pDst1[i], pExpected1[i]);
	Tests::verifyEqual(pDst2.size(), pExpected2.size());
	for (size_t i = 0; i<pDst2.size(); ++i) Tests::verifyEqual(pDst2[i], pExpected2[i]);
#else
	Tests::verifyEqual(pDst1, pExpected1);
	Tests::verifyEqual(pDst2, pExpected2);
#endif

    Tests::TestSection("Border case: tangent split on a vertex");
	pLine = wykobi::make_line(0.0f, -2.0f, 2.0f, 0.0f);
	Geometry::split(pSrc, pLine, pDst1, pDst2);
	pExpected1 = pSrc;
	pExpected2.clear();
	pExpected2.push_back(wykobi::make_point( 1.0f,-1.0f));
#ifdef DEBUG_Geometry
	Tests::verifyEqual(pDst1.size(), pExpected1.size());
	for (size_t i = 0; i<pDst1.size(); ++i) Tests::verifyEqual(pDst1[i], pExpected1[i]);
	Tests::verifyEqual(pDst2.size(), pExpected2.size());
	for (size_t i = 0; i<pDst2.size(); ++i) Tests::verifyEqual(pDst2[i], pExpected2[i]);
#else
	Tests::verifyEqual(pDst1, pExpected1);
	Tests::verifyEqual(pDst2, pExpected2);
#endif

    Tests::TestSection("Border case: split on a vertex");
	pLine = wykobi::make_line(0.0f, -1.0f, 1.0f, 1.0f);
	Geometry::split(pSrc, pLine, pDst1, pDst2);
	pExpected1.clear();
	pExpected1.push_back(wykobi::make_point(-1.0f, 1.0f));
	pExpected1.push_back(wykobi::make_point( 1.0f, 1.0f));
	pExpected1.push_back(wykobi::make_point( 0.0f,-1.0f));
	pExpected1.push_back(wykobi::make_point(-1.0f,-1.0f));
	pExpected2.clear();
	pExpected2.push_back(wykobi::make_point( 1.0f, 1.0f));
	pExpected2.push_back(wykobi::make_point( 1.0f,-1.0f));
	pExpected2.push_back(wykobi::make_point( 0.0f,-1.0f));
#ifdef DEBUG_Geometry
	Tests::verifyEqual(pDst1.size(), pExpected1.size());
	for (size_t i = 0; i<pDst1.size(); ++i) Tests::verifyEqual(pDst1[i], pExpected1[i]);
	Tests::verifyEqual(pDst2.size(), pExpected2.size());
	for (size_t i = 0; i<pDst2.size(); ++i) Tests::verifyEqual(pDst2[i], pExpected2[i]);
#else
	Tests::verifyEqual(pDst1, pExpected1);
	Tests::verifyEqual(pDst2, pExpected2);
#endif

    Tests::TestSection("Border case: split on an edge");
	pLine = wykobi::make_line(-1.0f, -1.0f, 1.0f, -1.0f);
	Geometry::split(pSrc, pLine, pDst1, pDst2);
	pExpected1 = pSrc;
	pExpected2.clear();
	pExpected2.push_back(wykobi::make_point( 1.0f,-1.0f));
	pExpected2.push_back(wykobi::make_point(-1.0f,-1.0f));
#ifdef DEBUG_Geometry
	Tests::verifyEqual(pDst1.size(), pExpected1.size());
	for (size_t i = 0; i<pDst1.size(); ++i) Tests::verifyEqual(pDst1[i], pExpected1[i]);
	Tests::verifyEqual(pDst2.size(), pExpected2.size());
	for (size_t i = 0; i<pDst2.size(); ++i) Tests::verifyEqual(pDst2[i], pExpected2[i]);
#else
	Tests::verifyEqual(pDst1, pExpected1);
	Tests::verifyEqual(pDst2, pExpected2);
#endif


    Tests::TestSection("Border case: split outside");
	pLine = wykobi::make_line(-1.0f, 10.0f, 1.0f, 10.0f);
	Geometry::split(pSrc, pLine, pDst1, pDst2);
	Geometry::split(pSrc, pLine, pDst1, pDst2);
	pExpected1 = pSrc;
	pExpected2.clear();
	Tests::verifyEqual(pDst1, pExpected1);
	Tests::verifyEqual(pDst2, pExpected2);
}

