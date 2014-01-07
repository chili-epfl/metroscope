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

#include "MergedFiducialMarker.hpp"

void MergedFiducialMarker::addFiducialMarker(FiducialMarker *pMarker) {
	mComponents.push_back(pMarker);
}

bool MergedFiducialMarker::isPresent() const
{
	bool tIspresent = false;
	for (unsigned int i = 0; !tIspresent && i < mComponents.size(); ++i) {
		tIspresent = tIspresent || mComponents[i]->isPresent();
	}
	return tIspresent;
}

wykobi::quadix<float, 2> MergedFiducialMarker::getCorners() const
{
	float tCount = 0.0f;
	wykobi::quadix<float, 2> tResult;

	for (unsigned int i = 0; i < mComponents.size(); ++i) {
		if (mComponents[i]->isPresent()) {
			wykobi::quadix<float, 2> tCorners = mComponents[i]->getCorners();
			tResult[0].x += tCorners[0].x;
			tResult[1].x += tCorners[1].x;
			tResult[2].x += tCorners[2].x;
			tResult[3].x += tCorners[3].x;
			tResult[0].y += tCorners[0].y;
			tResult[1].y += tCorners[1].y;
			tResult[2].y += tCorners[2].y;
			tResult[3].y += tCorners[3].y;
			++tCount;
		}
	}

	tResult[0].x /= tCount;
	tResult[1].x /= tCount;
	tResult[2].x /= tCount;
	tResult[3].x /= tCount;
	tResult[0].y /= tCount;
	tResult[1].y /= tCount;
	tResult[2].y /= tCount;
	tResult[3].y /= tCount;
	return tResult;
}
