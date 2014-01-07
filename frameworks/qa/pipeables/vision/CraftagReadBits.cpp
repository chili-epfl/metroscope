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

#include "CraftagReadBits.hpp"
#include <opencv2/opencv.hpp>

//#define DEBUG_CraftagReadBits
#ifdef DEBUG_CraftagReadBits
#include <opencv2/highgui/highgui.hpp>
#endif

namespace {
	static const int scDataSize = 6;
	static const int scTagWarpZoom = 16;
}

CraftagReadBits::CraftagReadBits(const IplImage *const *pInputImage):
mAdaptiveThreshold(0.95f, 0.5f, pInputImage),
mMatrix(new unsigned char[scDataSize*scDataSize])
{
#ifdef DEBUG_CraftagReadBits
	cvNamedWindow("CraftagReadBits");
#endif
}

CraftagReadBits::~CraftagReadBits()
{
	delete[] mMatrix;
}


void CraftagReadBits::run()
{
	mAdaptiveThreshold.start();
	const IplImage * tBinarizedImage = *mAdaptiveThreshold.GetOutputImage();
	unsigned char *tData = (unsigned char *) (tBinarizedImage)->imageData;
	int tWidthStep = tBinarizedImage->widthStep;

#ifdef DEBUG_CraftagReadBits
	cvShowImage("CraftagReadBits", tBinarizedImage);
	cvWaitKey(0);
#endif

	for (int i = 0; i < scDataSize; ++i)
	{
		for (int j = 0; j < scDataSize; ++j)
		{
			int tVotesForWhite = 0;
			for(int y=0; y<scTagWarpZoom; ++y)
			{
				for(int x=0; x<scTagWarpZoom; ++x)
				{
					int tPosition = (scTagWarpZoom*i+y)*tWidthStep + j*scTagWarpZoom+x;
					tVotesForWhite += tData[tPosition]/255;
				}
			}
			mMatrix[i*scDataSize + j] = (tVotesForWhite > scTagWarpZoom*scTagWarpZoom/2);
		}
	}
}
