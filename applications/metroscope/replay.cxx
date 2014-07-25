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

#include <iostream>

#include <qa/pipeables/io/CraftagReplay.hpp>
#include <qa/pipeables/io/CraftagStop.hpp>
#include <qa/pipeables/misc/SynchronizedStop.hpp>
#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "decorators/all.hpp"
#include <qa/pipeables/grabbers/OpenGlGrabber.hpp>
#include <qa/pipeables/vision/Flip.hpp>
#include <qa/pipeables/io/VideoSaver.hpp>

#include "TimeStampRenderer.hpp"

NetworkedStateManager* stateManager;


int main(int argc, char* argv[])
{
	auto tOpenGl2DDisplay = new OpenGl2DDisplay(0, 0, 1280, 768);
	if (argc!=2 && argc!=3) { std::cout << "RTFS" << std::endl; return -1; }
	CraftagReplay *tCraftagReplay = new CraftagReplay(argv[1]);
	CraftagStop *tCraftagStop = new CraftagStop(0);
	SynchronizedStop *tSynchronizedStop = new SynchronizedStop(*tCraftagReplay);
    const PlaneCoordinateMapper tDefaultCam2World("config/cam2world.map");
    const PlaneCoordinateMapper tDefaultWorld2Proj("config/world2proj.map", 2.0f);
	DecoratorManager *tDecoratorManager = new DecoratorManager("activities/", *tOpenGl2DDisplay, tDefaultCam2World, tDefaultWorld2Proj, 0, argv[1]);
	TimeStampRenderer *tTimeStampRenderer = new TimeStampRenderer(tCraftagReplay->Timestamp(), tOpenGl2DDisplay);
	bool tSaveVideo = (argc == 3);
	OpenGlGrabber *tOpenGlGrabber = tSaveVideo?new OpenGlGrabber(1280, 768):0;
	Flip *tFlip = tSaveVideo?new Flip(Flip::scVertically, tOpenGlGrabber->Image()):0;
	VideoSaver<true> *tVideoSaver = tSaveVideo?new VideoSaver<true>(tFlip->GetOutputImage(),argv[2], 30.0f, CV_FOURCC('X','V','I','D')):0;

	*tCraftagReplay | *tCraftagReplay;

	if (tSaveVideo)
		*tOpenGl2DDisplay
		| *tCraftagStop
		| *tSynchronizedStop
		| *tDecoratorManager
		| *tTimeStampRenderer
		| *tOpenGlGrabber
		| *tFlip
		| *tVideoSaver
		| *tOpenGl2DDisplay;
	else
		*tOpenGl2DDisplay
		| *tCraftagStop
		| *tSynchronizedStop
		| *tDecoratorManager
		| *tTimeStampRenderer
		| *tOpenGl2DDisplay;

	tCraftagReplay->startNoWait();
	tOpenGl2DDisplay->start();
	tCraftagReplay->stop();
	tCraftagReplay->join();

	if (tSaveVideo)
	{
		delete tVideoSaver;
		delete tFlip;
		delete tOpenGlGrabber;
	}
	delete tTimeStampRenderer;
	delete tDecoratorManager;
	delete tCraftagStop;
	delete tCraftagReplay;

	return 0;
}
