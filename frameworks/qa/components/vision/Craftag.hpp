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

#ifndef Craftag_HPP
#define Craftag_HPP

#include "FiducialMarker.hpp"
#include "CraftagRegistrar.hpp"
#include "../vision/Color.hpp"
#include "../geometry/Angle.hpp"

#include <opencv2/opencv.hpp>


class Craftag : public FiducialMarker
{
	public:

	Craftag(int pMarkerId, int pNFramesToPersist = 0, CraftagRegistrar &pCraftagRegistrar = CraftagRegistrar::GetDefault());
	virtual bool isPresent() const;
	virtual wykobi::quadix<float, 2> getCorners() const;
	void draw(IplImage*, const wykobi::point2d<int>&, const Angle<float>&, float, const Color&) const;

	int GetFramesNumberSinceLastDetected();
	int GetMarkerId() {return mMarkerId;}

	protected:
	int mMarkerId;
	int mNFramesToPersist;
	CraftagRegistrar &mCraftagRegistrar;

	private:
	Craftag();
	Craftag(const Craftag& );
	Craftag& operator=(const Craftag& );
};
#endif
