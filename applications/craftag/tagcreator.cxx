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

#include <cstdlib>
#include <string>
#include <qa/components/vision/TagTranscoder.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

int main(int argc, char **argv)
{
	static const int scTagBorder = 2;
	static const int scDataSize = 6;
	static const int scTagSize = scDataSize+2*scTagBorder;

	int id = 0;
	std::string tOutputFilename = "0.png";
	if (argc>1) {
		id = std::atoi(argv[1]); //ahah seriously?
		tOutputFilename = std::string(argv[1])+".png";
	}
	bool **tMatrix = new bool*[scTagSize];
	for (int i = 0; i < scTagSize; ++i) {
		tMatrix[i] = new bool[scTagSize];
	}

	TagTranscoder tTagTranscoder(10, 16, 10, "1010101010", "10001000000100001");
	unsigned char *tData = new unsigned char[scDataSize*scDataSize];
	tTagTranscoder.getTagEncodedId(id, tData);

	IplImage *tImage = cvCreateImage(cvSize(scTagSize, scTagSize), IPL_DEPTH_8U, 1);
	int tImageStep = tImage->widthStep;
	uchar *tImageData = (uchar *)tImage->imageData;

	cvZero(tImage);
	for(int i=scTagBorder; i<scTagSize-scTagBorder; ++i) for(int j=scTagBorder; j<scTagSize-scTagBorder; ++j)
			tImageData[i*tImageStep+j]=tData[(i-scTagBorder)*(scDataSize)+j-scTagBorder]*255;

	cvSaveImage(tOutputFilename.c_str(), tImage);

	return 0;
}
