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

#include "CannyEdgeDetector.hpp"
#include "../../utils/Image.hpp"

CannyEdgeDetector::CannyEdgeDetector(
	const int pLowThreshold,
	const int pHighThreshold,
	const int pApertureSize,
	const IplImage *const *pInputImage
	):
mLowThreshold(pLowThreshold),
mHighThreshold(pHighThreshold),
mApertureSize(pApertureSize),
mInputImage(pInputImage),
mOutputImage(cvCreateImage(cvSize((*pInputImage)->width,(*pInputImage)->height),(*pInputImage)->depth,1))
{
}

CannyEdgeDetector::~CannyEdgeDetector()
{
	cvReleaseImage(&mOutputImage);
}

void CannyEdgeDetector::run()
{
    const IplImage *const tInputImage = *mInputImage;
	Image::matchImageFormats(tInputImage, &mOutputImage, true);
	cvCanny(tInputImage,mOutputImage,mLowThreshold,mHighThreshold,mApertureSize);
}
