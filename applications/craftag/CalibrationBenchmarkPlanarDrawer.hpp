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

#ifndef CalibrationBenchmarkPlanarDrawer_HPP
#define CalibrationBenchmarkPlanarDrawer_HPP

#include <qa/pipeables/Pipeable.hpp>
#include <qa/pipeables/io/OpenGl2DDisplay.hpp>
#include <qa/components/vision/PlaneCoordinateMapper.hpp>

class CalibrationBenchmarkPlanarDrawer: public Pipeable
{
    public:
        CalibrationBenchmarkPlanarDrawer(
                const PlaneCoordinateMapper &pCam2World,
                const PlaneCoordinateMapper &pWorld2Proj,
                OpenGl2DDisplay *pOpenGl2DDisplay,
        		const float *pExpectedX, const float *pExpectedY,
        		const float *pFoundX, const float *pFoundY):
            mCam2World(pCam2World),
            mWorld2Proj(pWorld2Proj),
            mOpenGl2DDisplay(pOpenGl2DDisplay),
        	mExpectedX(pExpectedX), mExpectedY(pExpectedY),
        	mFoundX(pFoundX), mFoundY(pFoundY)
        	{}


    protected:
        void run() {
			float tFoundX = *mFoundX;
			float tFoundY = *mFoundY;
			mCam2World.InterpolatedMap(tFoundX, tFoundY);
			mWorld2Proj.InterpolatedMap(tFoundX, tFoundY);
			mOpenGl2DDisplay->RenderEllipse(tFoundX, tFoundY, 50.0f, 50.0f, 0.9f, 0.9f, 0.9f);

			float tMappedX = *mExpectedX;
			float tMappedY = *mExpectedY;
			mCam2World.InterpolatedMap(tMappedX, tMappedY);
			mWorld2Proj.InterpolatedMap(tMappedX, tMappedY);
			mOpenGl2DDisplay->RenderFilledEllipse(tMappedX, tMappedY, 10.0f, 10.0f, 0.0f, 0.0f, 0.0f);
        }

        const PlaneCoordinateMapper &mCam2World;
        const PlaneCoordinateMapper &mWorld2Proj;
        OpenGl2DDisplay *mOpenGl2DDisplay;
		const float *mExpectedX;
		const float *mExpectedY;
		const float *mFoundX;
		const float *mFoundY;

    private:
        CalibrationBenchmarkPlanarDrawer();
        CalibrationBenchmarkPlanarDrawer(const CalibrationBenchmarkPlanarDrawer&);
        CalibrationBenchmarkPlanarDrawer& operator=(const CalibrationBenchmarkPlanarDrawer&);
};

#endif
