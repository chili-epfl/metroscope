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

#ifndef StabilizedFiducialMarker_HPP
#define StabilizedFiducialMarker_HPP

#include "FiducialMarker.hpp"

class StabilizedFiducialMarker : public FiducialMarker
{
	public:
	static const float scDefaultMinTagTranslation;

	StabilizedFiducialMarker(FiducialMarker &pFiducialMarker, float pMinTagTranslation = scDefaultMinTagTranslation):
		mFiducialMarker(pFiducialMarker),
		mMinTagTranslation(pMinTagTranslation){}

	virtual bool isPresent() const {return mFiducialMarker.isPresent();}
	virtual wykobi::quadix<float, 2> getCorners() const;

	protected:
	StabilizedFiducialMarker();
	FiducialMarker &mFiducialMarker;

	const float mMinTagTranslation;
	mutable wykobi::quadix<float, 2> mStableCorners;
	

private:
	StabilizedFiducialMarker(const StabilizedFiducialMarker& );
	StabilizedFiducialMarker& operator=(const StabilizedFiducialMarker& );

};
#endif
