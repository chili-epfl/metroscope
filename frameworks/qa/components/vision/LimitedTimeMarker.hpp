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

#ifndef LimitedTimeMarker_HPP
#define LimitedTimeMarker_HPP

#include "FiducialMarker.hpp"
#include "CraftagRegistrar.hpp"

class LimitedTimeMarker : public FiducialMarker
{
	public:
	LimitedTimeMarker(FiducialMarker &pFiducialMarker, unsigned int pMaxApparitions, const CraftagRegistrar &pCraftagRegistrar = CraftagRegistrar::GetDefault()):
		mFiducialMarker(pFiducialMarker),
		mMaxApparitions(pMaxApparitions),
		mApparitionsRemaining(pMaxApparitions),
		mCraftagRegistrar(pCraftagRegistrar),
		mLastApparitions(mCraftagRegistrar.getFrameId()-1){}
	virtual bool isPresent() const;
	virtual wykobi::quadix<float, 2> getCorners() const { return mFiducialMarker.getCorners(); }

	unsigned int getMaxApparitions() {return mMaxApparitions;}
	unsigned int getApparitionsRemaining() {return mApparitionsRemaining;}

	protected:
	LimitedTimeMarker();
	FiducialMarker &mFiducialMarker;
	mutable unsigned int mMaxApparitions;
	mutable unsigned int mApparitionsRemaining;
	const CraftagRegistrar &mCraftagRegistrar;
	mutable int mLastApparitions;

private:
	LimitedTimeMarker(const LimitedTimeMarker& );
	LimitedTimeMarker& operator=(const LimitedTimeMarker& );

};
#endif
