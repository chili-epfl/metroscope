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

#ifndef AggregatedFiducialMarker_HPP
#define AggregatedFiducialMarker_HPP

#include "FiducialMarker.hpp"
#include <opencv2/opencv.hpp>
#include <vector>

class AggregatedFiducialMarker : public FiducialMarker
{
	public:
	AggregatedFiducialMarker(float pSize = 2.0f, float pTopLeftX = 0.0f, float pTopLeftY = 0.0f);
//	virtual ~AggregatedFiducialMarker();
	virtual bool isPresent() const;
	//isPresent() should return true before calling getCorners()
	virtual wykobi::quadix<float, 2> getCorners() const;
	void addFiducialMarker(const FiducialMarker *pMarker, float pTopLeftX, float pTopLeftY, float pSize = 2.0f);

	protected:
	const wykobi::quadix<float, 2> mSrcCorners;

	std::vector<const FiducialMarker *> mComponents;
	std::vector<wykobi::quadix<float, 2> > mComponentsPositions;

	// The set of present markers is cached to maintain coherence between a call
    // to isPresent() and a call to getCorners()
	mutable std::vector<wykobi::point2d<float> > mSrcPoints;
	mutable std::vector<wykobi::point2d<float> > mDstPoints;


private:
	AggregatedFiducialMarker(const AggregatedFiducialMarker& );
	AggregatedFiducialMarker& operator=(const AggregatedFiducialMarker& );

};
#endif
