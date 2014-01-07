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

#include "BistableFiducialMarker.hpp"

#include <iostream>

void BistableFiducialMarker::update() const{
	if (mFiducialMarker.isPresent())
	{
		wykobi::quadix<float, 2> tNewCorners = mFiducialMarker.getCorners();
		if (mStable)
		{
			if (   wykobi::distance(mStableCorners[0], tNewCorners[0]) > mHighThreshold
				|| wykobi::distance(mStableCorners[1], tNewCorners[1]) > mHighThreshold
				|| wykobi::distance(mStableCorners[2], tNewCorners[2]) > mHighThreshold
				|| wykobi::distance(mStableCorners[3], tNewCorners[3]) > mHighThreshold ) {
				mStable = false;
			}
		}
		else
		{
			if (   wykobi::distance(mStableCorners[0], tNewCorners[0]) < mLowThreshold
				&& wykobi::distance(mStableCorners[1], tNewCorners[1]) < mLowThreshold
				&& wykobi::distance(mStableCorners[2], tNewCorners[2]) < mLowThreshold
				&& wykobi::distance(mStableCorners[3], tNewCorners[3]) < mLowThreshold ) {
				mStable = true;
			}
			mStableCorners = tNewCorners;
		}
	}
	else
	{
		mStable = false;
	}
}
