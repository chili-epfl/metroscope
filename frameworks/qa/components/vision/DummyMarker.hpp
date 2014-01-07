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

#ifndef DummyMarker_HPP
#define DummyMarker_HPP

#include "FiducialMarker.hpp"

class DummyMarker : public FiducialMarker
{
	public:
	DummyMarker(float x1 = 0.0f, float y1 = 0.0f,
			float x2 = 0.0f, float y2 = 0.0f,
			float x3 = 0.0f, float y3 = 0.0f,
			float x4 = 0.0f, float y4 = 0.0f)
	{
		mCorners[0].x = x1;
		mCorners[0].y = y1;
		mCorners[1].x = x2;
		mCorners[1].y = y2;
		mCorners[2].x = x3;
		mCorners[2].y = y3;
		mCorners[3].x = x4;
		mCorners[3].y = y4;
	}
	virtual bool isPresent() const {return true;}
	virtual wykobi::quadix<float, 2> getCorners() const {return mCorners;}

	wykobi::quadix<float, 2> mCorners;
};
#endif
