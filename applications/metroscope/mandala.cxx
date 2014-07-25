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
#include <qa/pipeables/grabbers/FileGrabber.hpp>
#include <qa/pipeables/vision/Color2Gray.hpp>
#include <qa/pipeables/io/SnapshotTaker.hpp>
#include <qa/pipeables/vision/CraftagUpdate.hpp>
#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <qa/components/misc/NetworkedStateManager.hpp>
#include "decorators/all.hpp"

NetworkedStateManager* stateManager;

int main(int /*argc*/, char* /*argv*/[])
{
	auto tOpenGl2DDisplay = new OpenGl2DDisplay(0, 0, 1280, 768);
	FileGrabber *tFileGrabber = new FileGrabber("./activities/symmetriesStBlaise/mandala");
	Color2Gray *tPreProcess = new Color2Gray(tFileGrabber->Image());
	CraftagUpdate *tCraftagUpdate = new CraftagUpdate(tPreProcess->GetOutputImage());
    const PlaneCoordinateMapper tDefaultCam2World("config/cam2world.map");
    const PlaneCoordinateMapper tDefaultWorld2Proj("config/world2proj.map", 2.0f);
    DecoratorManager *tDecoratorManager = new DecoratorManager("activities/", *tOpenGl2DDisplay, tDefaultCam2World, tDefaultWorld2Proj, tFileGrabber->Image());
	std::string tMultipleMirrorName("activities/symmetriesStBlaise.cfgpage440");
	decorators::MultipleMirror *tMultipleMirror = (decorators::MultipleMirror *) tDecoratorManager->getDecorator(tMultipleMirrorName);
	SnapshotTaker *tSnapshotTaker = new SnapshotTaker(tMultipleMirror->GetExtractedImage(), "./activities/symmetriesStBlaise/mandala/", 0l, ".jpg");

	*tOpenGl2DDisplay
	| *tFileGrabber
	| *tPreProcess
	| *tCraftagUpdate
	| *tDecoratorManager
	| *tSnapshotTaker
	| *tOpenGl2DDisplay;

	tOpenGl2DDisplay->start();

	delete tSnapshotTaker;
	delete tDecoratorManager;
	delete tCraftagUpdate;
	delete tPreProcess;
	delete tFileGrabber;

	return 0;
}
