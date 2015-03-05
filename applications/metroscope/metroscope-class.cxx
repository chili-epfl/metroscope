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
#include <qa/pipeables/misc/Wait.hpp>
#include <qa/components/misc/NetworkedStateManager.hpp>
#include <qa/pipeables/networking/PutPostRemoteJSONString.hpp>
#include <qa/pipeables/networking/GetRemoteJSONString.hpp>

#include "decorators/all.hpp"

#include <time.h>
#include <exception>
#include <string>
#include <iostream>

//#define DEBUG_metroscope_NOMT


//We define the global variable for the local states
NetworkedStateManager* stateManager(new NetworkedStateManager());

std::string extractIdFromURL(std::string pUrl){
	//Just extracts the last thing after the '/' in an URL, or the same string if there is no /
	unsigned found = pUrl.rfind("/");
	  if (found!=std::string::npos){//We found a slash
		  std::string newString = pUrl.substr(found+1,std::string::npos);//we get until the end of the string
		  return newString;
	  }
	  else return pUrl;
}



int main(int argc, char* argv[])
{
	Pipeable *tGrabber = 0;
	const IplImage* const *tGrabbedImage = 0;
	Pipeable *tPreProcess = 0;
	bool tNoLog = false;

	int tWaitDeviceTime = 500;
	int tWaitClassroomTime = 1000;
	std::string tUrlDevice = "";
	std::string tUrlClassroom = "";
	std::string tUrlDeviceHistory = "";
	std::string tUrlClassroomHistory = "";

	//Parsing the arguments from the command line:
	//metroscope-class [-opencv] [-log] [-waitdevice=500] [-waitclassroom=1000] -urldevice=http://... -urlclassroom=http://...
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
		else if (tArgument.substr(0,12) == "-waitdevice=")
		{
			if(tArgument.length()>12){
				  try
				  {
					  tWaitDeviceTime = atoi(tArgument.substr(12,tArgument.length()).c_str());
				  }
				  catch (std::exception& e)
				  {
				    std::cout << "Incorrect syntax in -waitdevice: " << std::endl;
				    std::cout << "metroscope-class [-opencv] [-nolog] [-waitdevice=500] [-waitclassroom=1000] -urldevice=http://... -urlclassroom=http://... -urldevicehistory=http://... -urlclassroomhistory=http://..." << std::endl;
				    return(0);
				  }

			}
		}
		else if (tArgument.substr(0,15) == "-waitclassroom=")
		{
			if(tArgument.length()>15){
				  try
				  {
					  tWaitClassroomTime = atoi(tArgument.substr(15,tArgument.length()).c_str());
				  }
				  catch (std::exception& e)
				  {
					  std::cout << "Incorrect syntax in -waitclassroom: " << std::endl;
					  std::cout << "metroscope-class [-opencv] [-nolog] [-waitdevice=500] [-waitclassroom=1000] -urldevice=http://... -urlclassroom=http://... -urldevicehistory=http://... -urlclassroomhistory=http://..." << std::endl;
				    return(0);
				  }

			}
		}
		else if (tArgument.substr(0,11) == "-urldevice=")
		{
			if(tArgument.length()>11){
			  tUrlDevice = tArgument.substr(11,tArgument.length());
			}
		}
		else if (tArgument.substr(0,14) == "-urlclassroom=")
		{
			if(tArgument.length()>14){
			  tUrlClassroom = tArgument.substr(14,tArgument.length());
			}
		}
		else if (tArgument.substr(0,18) == "-urldevicehistory=")
		{
			if(tArgument.length()>18){
			  tUrlDeviceHistory = tArgument.substr(18,tArgument.length());
			}
		}
		else if (tArgument.substr(0,21) == "-urlclassroomhistory=")
		{
			if(tArgument.length()>21){
			  tUrlClassroomHistory = tArgument.substr(21,tArgument.length());
			}
		}
	}

	//If we do not have URLs, we just quit
	if(tUrlClassroom.length()==0 || tUrlDevice.length()==0 || tUrlClassroomHistory.length()==0 || tUrlDeviceHistory.length()==0 ){
			  std::cout << "Missing parameters: " << std::endl;
			  std::cout << "metroscope-class [-opencv] [-nolog] [-waitdevice=500] [-waitclassroom=1000] -urldevice=http://... -urlclassroom=http://... -urldevicehistory=http://... -urlclassroomhistory=http://..." << std::endl;
		      return(0);

			}

	//We extract the meteor IDs to pass them to the state manager (it is the only part we know in advance)
    //std::cout << "Extracting meteor ids from command line" << std::endl;
	std::string deviceMeteorId = extractIdFromURL(tUrlDevice);
	stateManager->SetDeviceMeteorId(deviceMeteorId);

	//Initializing the networking pipeables...
    //std::cout << "Initializing networking pipeables" << std::endl;
	Wait tWaitDevice(tWaitDeviceTime);
	Wait tWaitClassroom(tWaitClassroomTime);
	PutPostRemoteJSONString tPutRemoteDevice(tUrlDevice, tUrlDeviceHistory, DEVICE);
	PutPostRemoteJSONString tPutRemoteClassroom(tUrlClassroom, tUrlClassroomHistory, CLASSROOM);
	GetRemoteJSONString tGetRemoteClassroom(tUrlClassroom, CLASSROOM);


	//Initializing the CV and tag detection pipeables...
	if (!tGrabber)
	{
		tGrabber = new FlyCapture2Grabber(1280, 960, 15.0f);
		tPreProcess = new Flip(Flip::scHorizontally, ((FlyCapture2Grabber *) tGrabber)->Image());
		tGrabbedImage = ((Flip *) tPreProcess)->GetOutputImage();
	}

	auto tOpenGl2DDisplay = new OpenGl2DDisplay(0, 0, 1280, 768);
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

	//Define and start the device HTTP putter thread...
    //std::cout << "Starting networking thread" << std::endl;
//    tPutRemoteDevice
//    | tWaitDevice
//    | tPutRemoteDevice;

//    tPutRemoteDevice.startNoWait();

	//Define and start the classroom HTTP getter/putter thread...
	tGetRemoteClassroom
	| tWaitClassroom
//	| tPutRemoteClassroom
	| tGetRemoteClassroom;

	tGetRemoteClassroom.startNoWait();


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

//	tPutRemoteDevice.stop();
//	tPutRemoteDevice.join();

	tGetRemoteClassroom.stop();
	tGetRemoteClassroom.join();


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
