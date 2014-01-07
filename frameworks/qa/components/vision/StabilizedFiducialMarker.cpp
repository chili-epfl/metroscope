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

#include "StabilizedFiducialMarker.hpp"

const float StabilizedFiducialMarker::scDefaultMinTagTranslation = 2.75f;

wykobi::quadix<float, 2> StabilizedFiducialMarker::getCorners() const {
	wykobi::quadix<float, 2> tNewCorners = mFiducialMarker.getCorners();
	if (   wykobi::distance(mStableCorners[0], tNewCorners[0]) > mMinTagTranslation
		|| wykobi::distance(mStableCorners[1], tNewCorners[1]) > mMinTagTranslation
		|| wykobi::distance(mStableCorners[2], tNewCorners[2]) > mMinTagTranslation
		|| wykobi::distance(mStableCorners[3], tNewCorners[3]) > mMinTagTranslation ) {
		mStableCorners = tNewCorners;
	}
	return mStableCorners;
}
