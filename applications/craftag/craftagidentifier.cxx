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

#include <qa/pipeables/grabbers/OpenCVGrabber.hpp>
#include <qa/pipeables/vision/Flip.hpp>
#include <qa/pipeables/vision/Color2Gray.hpp>
#include <qa/pipeables/misc/Noop.hpp>
#include <qa/pipeables/vision/CraftagUpdate.hpp>
#include <qa/pipeables/misc/FPSPrinter.hpp>
#include "CraftagIdentifier.hpp"

/**
 * Once it detects tags, it decorates them with their IDs
 *
 * Ignore the warnings: "VIDIOC_QUERYMENU: Invalid argument" and "HIGHGUI ERROR"
 */
int main(int , char* [])
{
	Pipeable *tGrabber = 0;
	Pipeable *tPreProcess = 0;
	const IplImage* const *tGrabbedImage = 0;
			tGrabber = new OpenCVGrabber(640, 480, 30, 0);
			tPreProcess = new Color2Gray(((OpenCVGrabber *) tGrabber)->Image());
			tGrabbedImage = ((Color2Gray *)tPreProcess)->GetOutputImage();

	CraftagUpdate *tCraftagUpdate = new CraftagUpdate(tGrabbedImage);
	FPSPrinter *tFPSPrinter = new FPSPrinter();
	CraftagIdentifier *tCraftagIdentifier = new CraftagIdentifier(tGrabbedImage);

	*tGrabber
	| *tPreProcess
	| *tCraftagUpdate
	| *tFPSPrinter
	| *tCraftagIdentifier
	| *tGrabber;

	tGrabber->start();

	delete tCraftagIdentifier;
	delete tFPSPrinter;
	delete tCraftagUpdate;
	delete tPreProcess;
	delete tGrabber;

	return 0;
}
