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

#include <qa/components/vision/PlaneCoordinateMapper.hpp>
#include <qa/pipeables/io/OpenGl2DDisplay.hpp>
#include <qa/pipeables/grabbers/FlyCapture2Grabber.hpp>
#include <qa/pipeables/vision/Flip.hpp>
#include <qa/pipeables/vision/AdaptiveThreshold.hpp>
#include <qa/pipeables/vision/ConnectedComponents.hpp>
#include <qa/pipeables/io/CvWaiter.hpp>
#include <qa/pipeables/io/CvShower.hpp>
#include "CalibrationBenchmark.hpp"
#include "CalibrationBenchmarkPlanarDrawer.hpp"

int main(int /*argc*/, char */*argv*/[])
{
	auto tCameraGrabber = new FlyCapture2Grabber(1280, 960, 15.0f);
	auto tFlip = new Flip(Flip::scHorizontally, tCameraGrabber->Image());
	auto tAdaptiveThreshold = new AdaptiveThreshold(0.89f, 1.0f, tFlip->GetOutputImage());
#ifdef debug_benchmark
	auto tConnectedComponents = new ConnectedComponents<true>(25, 500, 10, tAdaptiveThreshold->GetOutputImage());
	auto tCvWaiter = new CvWaiter();
	auto tCvShower = new CvShower('1',tConnectedComponents->GetOutputImage(),tCvWaiter->KeyPressed());
#else
	auto tConnectedComponents = new ConnectedComponents<false>(25, 500, 10, tAdaptiveThreshold->GetOutputImage());
#endif
	auto tCalibrationBenchmark = new CalibrationBenchmark<200, 200, 1280-200, 768-200>
		(tConnectedComponents->GetCenters(), tConnectedComponents->GetNumComponents());
	auto tOpenGl2DDisplay = new OpenGl2DDisplay(0, 0, 1280, 768);
    const PlaneCoordinateMapper tDefaultCam2World("config/cam2world.map");
    const PlaneCoordinateMapper tDefaultWorld2Proj("config/world2proj.map", 2.0f);
	auto tCalibrationBenchmarkDrawer = new CalibrationBenchmarkPlanarDrawer(
            tDefaultCam2World, tDefaultWorld2Proj, tOpenGl2DDisplay,
			tCalibrationBenchmark->GetExpectedX(), tCalibrationBenchmark->GetExpectedY(),
			tCalibrationBenchmark->GetFoundX(), tCalibrationBenchmark->GetFoundY());

	*tOpenGl2DDisplay
	| *tCameraGrabber
	| *tFlip
	| *tAdaptiveThreshold
	| *tConnectedComponents
#ifdef debug_benchmark
	| *tCvShower
	| *tCvWaiter
#endif
	| *tCalibrationBenchmark
	| *tCalibrationBenchmarkDrawer
	| *tOpenGl2DDisplay;

	tOpenGl2DDisplay->start();

	delete tCalibrationBenchmarkDrawer;
	delete tCalibrationBenchmark;
    delete tOpenGl2DDisplay;
#ifdef debug_benchmark
	delete tCvShower;
	delete tCvWaiter;
#endif
	delete tConnectedComponents;
	delete tAdaptiveThreshold;
	delete tFlip;
	delete tCameraGrabber;

	return 0;
}
