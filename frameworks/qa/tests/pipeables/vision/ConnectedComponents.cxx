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
#include <qa/pipeables/vision/ConnectedComponents.hpp>
#include <qa/pipeables/vision/Color2Gray.hpp>
#include <qa/pipeables/io/CvWaiter.hpp>
#include <qa/pipeables/io/CvShower.hpp>
#include <qa/pipeables/misc/Stopper.hpp>
#include <opencv2/highgui/highgui.hpp>

int main()
{
	unsigned int tMinBlobArea = 0;
	unsigned int tMaxBlobArea = 10000;
	unsigned int tMaxNumBlobs = 1000;

//Test Blob in the LU Corner
    IplImage* tBlobImage = cvLoadImage("data/unittest/blob-CornerLU.png");

	Color2Gray *tColor2Gray = new Color2Gray(&tBlobImage);

	ConnectedComponents<false> *tConnectedComponents = new ConnectedComponents<false>(tMinBlobArea, tMaxBlobArea, tMaxNumBlobs, tColor2Gray->GetOutputImage());

	CvWaiter *tWaiter = (new CvWaiter())
		->BlockingTimeMillis(1);

	CvShower *tConnectedComponentsShower = (new CvShower('1',  tConnectedComponents->GetOutputImage(), tWaiter->KeyPressed()))
		->StartingPosition(cvPoint(0,0));

	*tColor2Gray
	| *tConnectedComponents
	| *tConnectedComponentsShower
	| *tWaiter;

	tColor2Gray->start();

	Tests::TestSection("\n\nTesting 1 Blob in the LU Corner");

	Tests::TestSection("Number of detected Blobs");
	const Component* const *tComponents = *(tConnectedComponents->GetComponents());
	int tNumComponents = *tConnectedComponents->GetNumComponents();

	Tests::verifyEqual(tNumComponents, 1);

	Tests::TestSection(" Area");
    int tExpectedArea = 24;

	if (tNumComponents>0) Tests::verifyEqual(tComponents[0]->mArea, tExpectedArea);
  
    Tests::TestSection(" Centers");
    Tests::verifyEqual(tComponents[0]->mX, 2.0f);
    Tests::verifyEqual(tComponents[0]->mY, 2.5f);
	cvWaitKey(0);

//Test Blob in the RU Corner
	cvReleaseImage (&tBlobImage);
    tBlobImage = cvLoadImage("data/unittest/blob-CornerRU.png");
	tColor2Gray->start();

	Tests::TestSection("\n\nTesting 1 Blob in the RU Corner");

	Tests::TestSection("Number of detected Blobs");
	tNumComponents = *tConnectedComponents->GetNumComponents();

	Tests::verifyEqual(tNumComponents, 1);


	Tests::TestSection(" Area");
	
	if (tNumComponents>0) Tests::verifyEqual(tComponents[0]->mArea, tExpectedArea);
	
	Tests::TestSection(" Centers");
	if (tNumComponents>0) {
		Tests::verifyEqual(tComponents[0]->mX, 19.0f);
		Tests::verifyEqual(tComponents[0]->mY, 2.5f);
	}
	cvWaitKey(0);

//Test Blob in the LD Corner
	cvReleaseImage (&tBlobImage);
    tBlobImage = cvLoadImage("data/unittest/blob-CornerLD.png");
	tColor2Gray->start();

	Tests::TestSection("\n\nTesting 1 Blob in the LD Corner");

	Tests::TestSection("Number of detected Blobs");
	tNumComponents = *tConnectedComponents->GetNumComponents();

	Tests::verifyEqual(tNumComponents, 1);

	Tests::TestSection(" Area");
	
	if (tNumComponents>0) Tests::verifyEqual(tComponents[0]->mArea, tExpectedArea);

	Tests::TestSection(" Centers");
	if (tNumComponents>0) {
		Tests::verifyEqual(tComponents[0]->mX, 2.0f);
		Tests::verifyEqual(tComponents[0]->mY, 11.5f);
	}
	cvWaitKey(0);

//Test Blob in the RD Corner
	cvReleaseImage (&tBlobImage);
    tBlobImage = cvLoadImage("data/unittest/blob-CornerRD.png");
	tColor2Gray->start();

	Tests::TestSection("\nTesting 1 Blob in the RD Corner");
	Tests::TestSection("Number of detected Blobs");
	tNumComponents = *tConnectedComponents->GetNumComponents();

	Tests::verifyEqual(tNumComponents, 1);

	Tests::TestSection(" Area");

	if (tNumComponents>0) Tests::verifyEqual(tComponents[0]->mArea, tExpectedArea);

	Tests::TestSection(" Centers");
	if (tNumComponents>0) {
	    Tests::verifyEqual(tComponents[0]->mX, 19.0f);
	    Tests::verifyEqual(tComponents[0]->mY, 11.5f);
	}
	cvWaitKey(0);

//Test Snake Blob
	cvReleaseImage (&tBlobImage);
    tBlobImage = cvLoadImage("data/unittest/blob-Snake.png");
	tColor2Gray->start();

	Tests::TestSection("\nTesting 1 Snake Blob");
	Tests::TestSection("Number of detected Blobs");
	tNumComponents = *tConnectedComponents->GetNumComponents();

	Tests::verifyEqual(tNumComponents, 1);

	Tests::TestSection(" Area");
	tExpectedArea=59;
	if (tNumComponents>0) Tests::verifyEqual(tComponents[0]->mArea, tExpectedArea);
	
	Tests::TestSection(" Centers");
	if (tNumComponents>0) {
		Tests::verifyEqual(tComponents[0]->mX, 11.0f);
		Tests::verifyEqual(tComponents[0]->mY, 7.0f);
	}
	cvWaitKey(0);

	//Test Snake Blob
	cvReleaseImage (&tBlobImage);
    tBlobImage = cvLoadImage("data/unittest/blob-multiple.png");
	tColor2Gray->start();

	Tests::TestSection("\nTesting 9 Blobs");
	Tests::TestSection("Number of detected Blobs");
	tNumComponents = *tConnectedComponents->GetNumComponents();

	Tests::verifyEqual(tNumComponents, 9);

	Tests::TestSection(" Areas");

    int tExpectedAreas[] = {22, 7, 3, 18, 2, 30, 4, 8, 5};

    for (int i=0; i< tNumComponents; ++i) {
        Tests::verifyEqual(tComponents[i]->mArea, tExpectedAreas[i]);
    }

    Tests::TestSection("Centers");
    Tests::verifyEqual(tComponents[2]->mX, 10.0f);
    Tests::verifyEqual(tComponents[2]->mY, 2.0f);
    Tests::verifyEqual(tComponents[5]->mX, 9.0f);
    Tests::verifyEqual(tComponents[5]->mY, 8.5f);
    Tests::verifyEqual(tComponents[8]->mX, 13.0f);
    Tests::verifyEqual(tComponents[8]->mY, 12.0f);

	cvWaitKey(0);


//Test Big Blob in the LU Corner

	tMinBlobArea = 100;
	tMaxBlobArea = 100000;

	cvReleaseImage (&tBlobImage);
    tBlobImage = cvLoadImage("data/unittest/blob-CornerLU-big.png");
	tColor2Gray->start();

	Tests::TestSection("\n\nTesting 1 Blob in the LU Corner");

	Tests::TestSection("Number of detected Blobs");
	tNumComponents = *tConnectedComponents->GetNumComponents();

	Tests::verifyEqual(tNumComponents, 1);


	Tests::TestSection(" Area");
	tExpectedArea=2400;
	if (tNumComponents>0) Tests::verifyEqual(tComponents[0]->mArea, tExpectedArea);
	
	Tests::TestSection(" Centers");
	if (tNumComponents>0) {
		Tests::verifyEqual(tComponents[0]->mX, 24.5f);
		Tests::verifyEqual(tComponents[0]->mY, 29.5f);
	}
	cvWaitKey(0);

//Test Big Blob in the RU Corner

	cvReleaseImage (&tBlobImage);
    tBlobImage = cvLoadImage("data/unittest/blob-CornerRU-big.png");
	tColor2Gray->start();

	Tests::TestSection("\n\nTesting 1 Blob in the RU Corner");

	Tests::TestSection("Number of detected Blobs");
	tNumComponents = *tConnectedComponents->GetNumComponents();

	Tests::verifyEqual(tNumComponents, 1);


	Tests::TestSection(" Area");
	if (tNumComponents>0) Tests::verifyEqual(tComponents[0]->mArea, tExpectedArea);
	
	Tests::TestSection(" Centers");
	if (tNumComponents>0) {
		Tests::verifyEqual(tComponents[0]->mX, 194.5f);
		Tests::verifyEqual(tComponents[0]->mY, 29.5f);
	}
	cvWaitKey(0);

//Test Blob in the LD Corner
	cvReleaseImage (&tBlobImage);
    tBlobImage = cvLoadImage("data/unittest/blob-CornerLD-big.png");
	tColor2Gray->start();

	Tests::TestSection("\n\nTesting 1 Blob in the LD Corner");

	Tests::TestSection("Number of detected Blobs");
	tNumComponents = *tConnectedComponents->GetNumComponents();

	Tests::verifyEqual(tNumComponents, 1);

	Tests::TestSection(" Area");
	
	if (tNumComponents>0) Tests::verifyEqual(tComponents[0]->mArea, tExpectedArea);

	Tests::TestSection(" Centers");
	if (tNumComponents>0) {
		Tests::verifyEqual(tComponents[0]->mX, 24.5f);
		Tests::verifyEqual(tComponents[0]->mY, 119.5f);
	}
	cvWaitKey(0);

//Test Blob in the RD Corner
	cvReleaseImage (&tBlobImage);
    tBlobImage = cvLoadImage("data/unittest/blob-CornerRD-big.png");
	tColor2Gray->start();

	Tests::TestSection("\nTesting 1 Blob in the RD Corner");
	Tests::TestSection("Number of detected Blobs");
	tNumComponents = *tConnectedComponents->GetNumComponents();

	Tests::verifyEqual(tNumComponents, 1);

	Tests::TestSection(" Area");

	if (tNumComponents>0) Tests::verifyEqual(tComponents[0]->mArea, tExpectedArea);

	Tests::TestSection(" Centers");
	if (tNumComponents>0) {
	    Tests::verifyEqual(tComponents[0]->mX, 194.5f);
	    Tests::verifyEqual(tComponents[0]->mY, 119.5f);
	}
	cvWaitKey(0);

//Test Snake Blob
	cvReleaseImage (&tBlobImage);
    tBlobImage = cvLoadImage("data/unittest/blob-Snake-big.png");
	tColor2Gray->start();

	Tests::TestSection("\nTesting 1 Snake Blob");
	Tests::TestSection("Number of detected Blobs");
	tNumComponents = *tConnectedComponents->GetNumComponents();

	Tests::verifyEqual(tNumComponents, 1);

	Tests::TestSection(" Area");
	tExpectedArea=5900;
	if (tNumComponents>0) Tests::verifyEqual(tComponents[0]->mArea, tExpectedArea);
	
	Tests::TestSection(" Centers");
	if (tNumComponents>0) {
		Tests::verifyEqual(tComponents[0]->mX, 114.5f);
		Tests::verifyEqual(tComponents[0]->mY, 74.5f);
	}
	cvWaitKey(0);
	/*
	Tests::TestSection("Areas");
    unsigned int tExpectedArea[] = {22, 7, 3, 18, 2, 30, 4, 8, 5};
    int tExpectedAreaIndex = 0;
	const ConnectedComponents<false>::Component* const *tComponents = *(tConnectedComponents->Components());
	int tNumComponents = *tConnectedComponents->NumComponents();
    for (int i=0; i< tNumComponents; ++i) {
        Tests::verifyEqual(tComponents[i]->mArea, tExpectedArea[tExpectedAreaIndex++]);
    }

    Tests::TestSection("Centers");
    Tests::verifyEqual(tComponents[2]->mX, 10.0f);
    Tests::verifyEqual(tComponents[2]->mY, 2.0f);
    Tests::verifyEqual(tComponents[5]->mX, 9.0f);
    Tests::verifyEqual(tComponents[5]->mY, 8.5f);
    Tests::verifyEqual(tComponents[8]->mX, 13.0f);
    Tests::verifyEqual(tComponents[8]->mY, 12.0f);
	*/
    //FIXME something is wrong here...
//	cvReleaseImage(&img);
//    img = cvLoadImage("projects/toot/data/unittest/pitiblobs.png");
//
//CvWaiter *tCvWaiter = (new CvWaiter());
//// ->BlockingTimeMillis(0);
////CvShower *tCvShower = new CvShower('1', tConnectedComponents->Visualisation(), 0);
////*tConnectedComponents | *tCvShower | *tCvWaiter;
////Stopper *tStopper = new Stopper(0);
//
//    tConnectedComponents->start();
//
//    Tests::TestSection("Pitiblobs");
//    for (std::vector<ConnectedComponents<false>::Component>::const_iterator it = tComponents.begin();
//        it != tComponents.end(); ++it) {
//        std::cout << it->area << ": " << it->x << "," << it->y << std::endl;
//    }
//
    delete tConnectedComponents;
	delete tColor2Gray;
	delete tConnectedComponentsShower;
	delete tWaiter;

    cvReleaseImage(&tBlobImage);
	cvWaitKey(0);
	return 0;
}
