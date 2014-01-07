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

#ifndef CalibrationBenchmark_HPP
#define CalibrationBenchmark_HPP

#include <qa/pipeables/Pipeable.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <wykobi/wykobi.hpp>

namespace {
	static const float scStep = 50.0f;
	static const float scNReadings = 3;
}

template <int scMinX, int scMinY, int scMaxX, int scMaxY>
class CalibrationBenchmark: public Pipeable
{
    public:
        CalibrationBenchmark(const wykobi::point2d<float> * const *pCenters, const int *pNumComponent):
        	mCenters(pCenters),
        	mNumComponent(pNumComponent),
        	mExpectedX(scMinX),
        	mExpectedY(scMinY),
        	mNReadings(scNReadings)
        {
			std::cout << "mExpectedX" << "," << "mExpectedY" << "," << "tFoundCorner.x" << "," << "tFoundCorner.y" << std::endl;
        }

        const float *GetExpectedX() const {return &mExpectedX;}
        const float *GetExpectedY() const {return &mExpectedY;}
        const float *GetFoundX() const {return &mFoundX;}
        const float *GetFoundY() const {return &mFoundY;}

    protected:
        void run()
        {
        	mFoundX = mFoundY = -100000.0f;
        	int tNCorners = *mNumComponent;
        	if (tNCorners > 0) {
        		auto tFoundCorner = (*mCenters)[0];
        		mFoundX = tFoundCorner.x;
        		mFoundY = tFoundCorner.y;

        		if (wykobi::distance(tFoundCorner.x, tFoundCorner.y, mExpectedX, mExpectedY) < scStep/2.0f)
        		{
        			std::cout << mExpectedX << "," << mExpectedY << "," << tFoundCorner.x << "," << tFoundCorner.y << std::endl;
//        			std::cout << wykobi::distance(tFoundCorner.x, tFoundCorner.y, mExpectedX, mExpectedY) << std::endl;

        			if (--mNReadings <= 0)
        			{
        				mNReadings = scNReadings;

        				mExpectedX += scStep;
        				if (mExpectedX > scMaxX) {
        					if (mExpectedY > scMaxY) {
        						stop();
        						return;
        					}
        					mExpectedX = scMinX;
        					mExpectedY += scStep;
        				}
        			}
        		}
        	}
        }

    	const wykobi::point2d<float> * const*mCenters;
    	const int *mNumComponent;
		float mExpectedX;
		float mExpectedY;
		float mFoundX;
		float mFoundY;
		int mNReadings;

    private:
        CalibrationBenchmark();
        CalibrationBenchmark(const CalibrationBenchmark&);
        CalibrationBenchmark& operator=(const CalibrationBenchmark&);
};

#endif
