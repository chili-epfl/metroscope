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
#include <qa/pipeables/vision/Color2Gray.hpp>
#include <qa/pipeables/vision/CraftagUpdate.hpp>
#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <qa/pipeables/misc/Noop.hpp>
#include <qa/pipeables/io/CraftagStop.hpp>
#include <qa/pipeables/misc/FPSPrinter.hpp>
#include "CamBackground.hpp"
#include "SlideShow.hpp"
#include <qa/pipeables/io/CvShower.hpp>
#include <qa/pipeables/io/CvWaiter.hpp>
#include "decorators/all.hpp"
#include <stdlib.h>

#include <time.h>

//#define DEBUG_metroscope_NOMT

//NetworkedStateManager* stateManager;


int main(int argc, char* argv[])
{
	int tScreenWidth = 1280;
	int tScreenHeight = 1024;
	if (argc > 4) {
		tScreenWidth = atoi(argv[3]);
		tScreenHeight = atoi(argv[4]);
	}
	int tScreenX = 0;
	int tScreenY = 0;
	if (argc > 6) {
		tScreenX = atoi(argv[5]);
		tScreenY = atoi(argv[6]);
	}
	auto tOpenGl2DDisplay = new OpenGl2DDisplay(tScreenX, tScreenY, tScreenWidth, tScreenHeight);
	auto tGrabber = new OpenCVGrabber(1280, 1024, 30, (argc>1)?atoi(argv[1]):-1);
	auto tPreProcess = new Color2Gray(tGrabber->Image());
	CraftagUpdate *tCraftagUpdate = new CraftagUpdate(tPreProcess->GetOutputImage());

	OpenCVGrabber *tGrabber2 = 0;
	Color2Gray *tPreProcess2 = 0;
	CraftagUpdate *tCraftagUpdate2 = 0;
	SlideShow *tSlideShow = 0;
	CraftagRegistrar tRegistrar2;
	if (argc > 2)
	{
		tGrabber2 = new OpenCVGrabber(1600, 1200, 25, atoi(argv[2]));
		tPreProcess2 = new Color2Gray(tGrabber2->Image());
		tCraftagUpdate2 = new CraftagUpdate(tPreProcess2->GetOutputImage(), tRegistrar2);
		tSlideShow = new SlideShow(*tOpenGl2DDisplay, tRegistrar2, "./slides.cfg");
	}

	FPSPrinter *tFPSPrinter = new FPSPrinter();
	auto tCamBackground = new CamBackground(*tOpenGl2DDisplay, tGrabber->Image());
	const PlaneCoordinateMapper tDefaultCam2World(0);
	const PlaneCoordinateMapper tDefaultWorld2Proj(0, 1.0f);
	DecoratorManager *tDecoratorManager = new DecoratorManager("activities/", *tOpenGl2DDisplay, tDefaultCam2World, tDefaultWorld2Proj, tPreProcess->GetOutputImage());
	CraftagStop *tCraftagStop = new CraftagStop(0);

#ifdef DEBUG_metroscope_NOMT
	*tOpenGl2DDisplay
	| *tGrabber;
	| *tPreProcess
	| *tCraftagUpdate
	| *tFPSPrinter
	| *tCamBackground
	| *tDecoratorManager
	| *tCraftagStop
	| *tOpenGl2DDisplay;

	if (tGrabber2)
	{
		*tCraftagUpdate
		| *tGrabber2
		| *tPreProcess2
		| *tCraftagUpdate2
		| *tFPSPrinter;

		*tDecoratorManager
		| *tSlideShow
		| *tCraftagStop;
	}

	OpenGl2DDisplay::scDefaultOpenGl2DDisplay.startDebug();
#else

	*tGrabber
	| *tPreProcess
	| *tCraftagUpdate
	| *tGrabber;

	if (tGrabber2)
	{
		*tGrabber2
		| *tPreProcess2
		| *tCraftagUpdate2
		| *tGrabber2;
	}

	*tOpenGl2DDisplay
	| *tFPSPrinter
	| *tCamBackground
	| *tDecoratorManager
	| *tCraftagStop
	| *tOpenGl2DDisplay;

	if (tGrabber2)
	{
		*tDecoratorManager
		| *tSlideShow
		| *tCraftagStop;
	}

	tGrabber->startNoWait();
	if (tGrabber2) tGrabber2->startNoWait();
	tOpenGl2DDisplay->start();
	if (tGrabber2) tGrabber2->stop();
	if (tGrabber2) tGrabber2->join();
	tGrabber->stop();
	tGrabber->join();

#endif

	delete tCraftagStop;
	delete tDecoratorManager;
	delete tCamBackground;
	delete tFPSPrinter;
	if (tGrabber2)
	{
		delete tSlideShow;
		delete tCraftagUpdate2;
		delete tPreProcess2;
		delete tGrabber2;
	}
	delete tCraftagUpdate;
	delete tPreProcess;
	delete tGrabber;

	return 0;
}
