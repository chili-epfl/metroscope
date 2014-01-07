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

#ifndef BistableFiducialMarker_HPP
#define BistableFiducialMarker_HPP

#include "FiducialMarker.hpp"

class BistableFiducialMarker : public FiducialMarker
{
	public:
	BistableFiducialMarker(
			FiducialMarker &pFiducialMarker,
			float pLowThreshold,
			float pHighThreshold):
	mFiducialMarker(pFiducialMarker),
	mStable(false),
	mLowThreshold(pLowThreshold),
	mHighThreshold(pHighThreshold){}

	virtual bool isPresent() const {update(); return mStable;}
	virtual wykobi::quadix<float, 2> getCorners() const {update(); return mStableCorners;}

	protected:
	void update() const;

	FiducialMarker &mFiducialMarker;
	mutable bool mStable;
	const float mLowThreshold;
	const float mHighThreshold;
	mutable wykobi::quadix<float, 2> mStableCorners;

	private:
	BistableFiducialMarker();
	BistableFiducialMarker(const BistableFiducialMarker& );
	BistableFiducialMarker& operator=(const BistableFiducialMarker& );


};
#endif
