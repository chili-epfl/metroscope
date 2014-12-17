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

#include "Executor.hpp"
#include <qa/utils/Time.hpp>

#include <iostream>
#include <sstream>
#include <math.h>

const std::string decorators::Executor::scDecoratorName("Executor");
const  DecoratorManager::Registerer decorators::Executor::mRegisterer(decorators::Executor::scDecoratorName, &decorators::Executor::create);

decorators::FiducialDecorator *decorators::Executor::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::Executor(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),  pDecoratorManager.loadMarker(pSetting["messagemarker"]), pSetting["command"], pSetting["message"], pSetting["countdown"]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::Executor::Executor(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pMarkerMessages, std::string pCommand, std::string pMessage, int pCountdown):
FiducialDecorator(pDecoratorManager, pMarker),
mMarkerMessages(pMarkerMessages),
mCommand(pCommand),
mCountdown(pCountdown),
mCountdownStart(0),
mMessage(pMessage)
{
}

void decorators::Executor::update() {
	if (mMarker->isPresent())
	{
		if(mCountdown>0){
			if(mCountdownStart==0) mCountdownStart=Time::MillisTimestamp();//If we just detected the marker, start the countdown
			displayCountdown();
		}
		displayMessage();
		long time = Time::MillisTimestamp();
		if(mCountdown==0 || (time - mCountdownStart)>(mCountdown*1000)){
			mCountdownStart=0;
			executeCommand();
		}
	}
}

/*Just displays a text message next to the marker*/
void decorators::Executor::displayMessage(){
	if(mMarker->isPresent()){
			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarkerMessages, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
				mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(mMessage.c_str(), scTEXT_DELIMITERS,
						0, 0, 900.0,
						false, 3,
						scMESSAGE_COLOR->r, scMESSAGE_COLOR->g, scMESSAGE_COLOR->b, scMESSAGE_COLOR->a);
				mDecoratorManager.GetDisplay().PopTransformation();
	}
}

/*Displays a fading countdown message below the normal message*/
void decorators::Executor::displayCountdown(){
	if(mMarker->isPresent() && mCountdown!=0 && mCountdownStart!=0){
			long time = Time::MillisTimestamp();
			float timepassed = (time - mCountdownStart)/1000.0;
			float intpart;
			float fractpart = modff(timepassed,&intpart);
			int number = mCountdown - intpart;
			float transparency= 1.0 - fractpart;
			std::ostringstream oss;
			oss << number;

			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarkerMessages, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
				mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(oss.str().c_str(), scTEXT_DELIMITERS,
						0, 200.0, 400.0,
						false, 3,
						scMESSAGE_COLOR->r, scMESSAGE_COLOR->g, scMESSAGE_COLOR->b, transparency);
				mDecoratorManager.GetDisplay().PopTransformation();
	}
}

/*Just displays a text message next to the marker*/
void decorators::Executor::executeCommand(){
	if(mMarker->isPresent()){

			//SUBSTITUTE THESE LINES WITH THE CODE TO EXECUTE THE COMMAND, stored in (std::string) mCommand
			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarkerMessages, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
				mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth("BAM!", scTEXT_DELIMITERS,
						0, -100, 400.0,
						false, 3,
						scMESSAGE_COLOR->r, scMESSAGE_COLOR->g, scMESSAGE_COLOR->b, scMESSAGE_COLOR->a);
				mDecoratorManager.GetDisplay().PopTransformation();
			////////////////////////////
	}
}
