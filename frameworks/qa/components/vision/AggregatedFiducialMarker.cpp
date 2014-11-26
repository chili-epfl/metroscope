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

#include "AggregatedFiducialMarker.hpp"
#include <opencv2/opencv.hpp>
#include <qa/utils/CvWykobiBridge.hpp>
#include <algorithm>
#include <iterator>
//#define DEBUG_AggregatedFiducialMarker
#ifdef DEBUG_AggregatedFiducialMarker
#include <iostream>
#endif

AggregatedFiducialMarker::AggregatedFiducialMarker(
	float pSize,
	float pTopLeftX, float pTopLeftY):
mSrcCorners(wykobi::make_quadix(
		pTopLeftX, pTopLeftY,
		pTopLeftX+pSize, pTopLeftY,
		pTopLeftX+pSize, pTopLeftY+pSize,
		pTopLeftX, pTopLeftY+pSize)),
mComponents(),
mComponentsPositions(),
mSrcPoints(),
mDstPoints()
{
}

void AggregatedFiducialMarker::addFiducialMarker(const FiducialMarker *pMarker, float pTopLeftX, float pTopLeftY, float pSize)
{
	mComponents.push_back(pMarker);
	mComponentsPositions.push_back(wykobi::make_quadix(
		pTopLeftX, pTopLeftY,
		pTopLeftX+pSize, pTopLeftY,
		pTopLeftX+pSize, pTopLeftY+pSize,
		pTopLeftX, pTopLeftY+pSize));
}

bool AggregatedFiducialMarker::isPresent() const
{
	mSrcPoints.clear();
	mDstPoints.clear();
	for (size_t i = 0; i < mComponents.size(); ++i) {
		if (mComponents[i]->isPresent())
		{
			wykobi::quadix<float, 2> tDstCorners = mComponents[i]->getCorners();
			wykobi::quadix<float, 2> tSrcCorners = mComponentsPositions[i];
			for (int j = 0; j < 4; ++j) {
				mSrcPoints.push_back(tSrcCorners[j]);
				mDstPoints.push_back(tDstCorners[j]);
			}
		}
	}
	return !mSrcPoints.empty();
}

wykobi::quadix<float, 2> AggregatedFiducialMarker::getCorners() const
{
	float mHomography[9];
	if(mSrcPoints.size()<4) return mSrcCorners;//If the corners disappeared, we just return the points themselves
	CvWykobiBridge::findHomography(mSrcPoints, mDstPoints, mHomography);
	wykobi::quadix<float, 2> tResult = mSrcCorners;
	CvWykobiBridge::applyHomography(mHomography, tResult);
	return tResult;
}
