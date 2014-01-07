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

#include "TranslatedFiducialMarker.hpp"

wykobi::quadix<float, 2> TranslatedFiducialMarker::getCorners() const
{
	wykobi::quadix<float, 2> tResult = mFiducialMarker.getCorners();
	wykobi::point2d<float> tTranslation = mXTranslation*mFiducialMarker.getXUnit() + mYTranslation*mFiducialMarker.getYUnit();
	tResult[0] = tResult[0]+tTranslation;
	tResult[1] = tResult[1]+tTranslation;
	tResult[2] = tResult[2]+tTranslation;
	tResult[3] = tResult[3]+tTranslation;
	return tResult;
}
