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

#include <qa/pipeables/io/OpenGl2DDisplay.hpp>
#include <qa/pipeables/grabbers/OpenCVGrabber.hpp>
#include <qa/pipeables/grabbers/FlyCapture2Grabber.hpp>
#include <qa/pipeables/vision/Color2Gray.hpp>
#include <qa/pipeables/vision/Flip.hpp>
#include <qa/pipeables/io/CraftagLogger.hpp>
#include <qa/pipeables/io/SnapshotTaker.hpp>
#include <qa/pipeables/vision/CraftagUpdate.hpp>
#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <qa/pipeables/misc/Noop.hpp>
#include <qa/pipeables/io/CraftagStop.hpp>
#include <qa/pipeables/misc/FPSPrinter.hpp>
#include <qa/pipeables/io/CvShower.hpp>
#include <qa/pipeables/io/CvWaiter.hpp>
#include "decorators/all.hpp"

#include <time.h>

//#define DEBUG_metroscope_NOMT

int main(int argc, char* argv[])
{
	Pipeable *tGrabber = 0;
	const IplImage* const *tGrabbedImage = 0;
	Pipeable *tPreProcess = 0;
	bool tNoLog = false;
	for (int i = 1; i < argc; ++i)
	{
		std::string tArgument(argv[i]);
		if (tArgument == "-opencv")
		{
			tGrabber = new OpenCVGrabber(640, 480, 30);
			tPreProcess = new Color2Gray(((OpenCVGrabber *) tGrabber)->Image());
			tGrabbedImage = ((Color2Gray *)tPreProcess)->GetOutputImage();
		}
		else if (tArgument == "-nolog")
		{
			tNoLog = true;
		}
	}
	if (!tGrabber)
	{
		tGrabber = new FlyCapture2Grabber(1280, 960, 15.0f);
		tPreProcess = new Flip(Flip::scHorizontally, ((FlyCapture2Grabber *) tGrabber)->Image());
		tGrabbedImage = ((Flip *) tPreProcess)->GetOutputImage();
	}


	auto tOpenGl2DDisplay = new OpenGl2DDisplay(0, 0, 1280, 768);
	tOpenGl2DDisplay->BackgroundColor(0.8f,0.8f,0.8f,1.0f);
	CraftagUpdate *tCraftagUpdate = new CraftagUpdate(tGrabbedImage);
    const PlaneCoordinateMapper tDefaultCam2World("config/cam2world.map");
    const PlaneCoordinateMapper tDefaultWorld2Proj("config/world2proj.map", 2.0f);
	DecoratorManager *tDecoratorManager = new DecoratorManager("activities/", *tOpenGl2DDisplay, tDefaultCam2World, tDefaultWorld2Proj, tGrabbedImage);
	CraftagStop *tCraftagStop = new CraftagStop(0);
	FPSPrinter *tFPSPrinter = new FPSPrinter();
	Pipeable *tCraftagLogger = 0;
	Pipeable *tSnapshotTaker = 0;
	if (tNoLog) {
		tCraftagLogger = new Noop();
		tSnapshotTaker = new Noop();
	} else {
		char tFilename[256];
		time_t tTimestamp = time(0);
		struct tm *tTime = localtime(&tTimestamp);
		sprintf(tFilename, "./%d-%.2d-%.2d_%.2d-%.2d-%.2d.tags.log", tTime->tm_year+1900, tTime->tm_mon+1, tTime->tm_mday, tTime->tm_hour, tTime->tm_min, tTime->tm_sec);
		tCraftagLogger = new CraftagLogger(tFilename);
		tSnapshotTaker = new SnapshotTaker(tGrabbedImage, "./", 1000l, ".jpg");
	}

#ifdef DEBUG_metroscope_NOMT
	*tOpenGl2DDisplay
	| *tGrabber;
	| *tPreProcess
	| *tCraftagUpdate
	| *tCraftagLogger
	| *tSnapshotTaker
	| *tFPSPrinter
	| *tDecoratorManager
	| *tCraftagStop
	| *tOpenGl2DDisplay;

	OpenGl2DDisplay::scDefaultOpenGl2DDisplay.startDebug();
#else

	*tGrabber
	| *tPreProcess
	| *tCraftagUpdate
	| *tFPSPrinter
	| *tCraftagLogger
	| *tSnapshotTaker
	| *tGrabber;

	*tOpenGl2DDisplay
	| *tDecoratorManager
	| *tCraftagStop
	| *tOpenGl2DDisplay;

	tGrabber->startNoWait();
	tOpenGl2DDisplay->start();
	tGrabber->stop();
	tGrabber->join();

#endif

	delete tSnapshotTaker;
	delete tCraftagLogger;
	delete tFPSPrinter;
	delete tCraftagStop;
	delete tDecoratorManager;
	delete tCraftagUpdate;
	delete tPreProcess;
	delete tGrabber;

	return 0;
}
