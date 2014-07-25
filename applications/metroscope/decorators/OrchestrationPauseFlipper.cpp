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

#include "OrchestrationPauseFlipper.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>

const std::string decorators::OrchestrationPauseFlipper::scDecoratorName("OrchestrationPauseFlipper");
const  DecoratorManager::Registerer decorators::OrchestrationPauseFlipper::mRegisterer(decorators::OrchestrationPauseFlipper::scDecoratorName, &decorators::OrchestrationPauseFlipper::create);

decorators::FiducialDecorator *decorators::OrchestrationPauseFlipper::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::OrchestrationPauseFlipper(pDecoratorManager, (Flipper *) pDecoratorManager.loadDecorator(pSetting["flipper"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::OrchestrationPauseFlipper::OrchestrationPauseFlipper(DecoratorManager &pDecoratorManager, Flipper *pFlipper):
FiducialDecorator(pDecoratorManager, 0, true),
mFlipper(pFlipper),
mLastToggle(Time::MillisTimestamp())
{
}

void decorators::OrchestrationPauseFlipper::update() {
	//std::cout << "Entering orchestration decorator. Classroom pause: " << stateManager->isClassroomPaused() << std::endl;
	if (mFlipper->IsPresent())
	{
		//We have the pausing flipper on sight...
		stateManager->addMarkerToDeviceState("pause");

		//std::cout << "Pausing flipper is present" << std::endl;

		if(mFlipper->IsFlipped()){
			//...and it has been flipped, we toggle the pause if a certain amount of time has passed

			long tElapsedTime = Time::MillisTimestamp() - mLastToggle;


			//std::cout << "Pausing flipper is flipped" << std::endl;
			if(stateManager->isClassroomPaused() && tElapsedTime>scTogglePauseDelay){
				//std::cout << "Toggling flipper to false" << std::endl;
				mLastToggle = Time::MillisTimestamp();
				stateManager->SetClassroomPaused(false);
			}
			else if (!stateManager->isClassroomPaused() && tElapsedTime>scTogglePauseDelay){
				//std::cout << "Toggling flipper to true" << std::endl;
				mLastToggle = Time::MillisTimestamp();
				stateManager->SetClassroomPaused(true);
			}
		}else{
			//std::cout << "Pausing flipper is NOT flipped anymore" << std::endl;
		}

		//Note: We do not check for the pauserDevice, we assume we can toggle the pause at any time

	}
	else{
		//The flipper is not on sight
		stateManager->removeMarkerFromDeviceState("pause");
	}

	if(stateManager->isClassroomPaused()) blackoutScreen();


}

// Just draws a big black square on the screen
void decorators::OrchestrationPauseFlipper::blackoutScreen(){
//	if(mMarker->isPresent()){
			mDecoratorManager.GetDisplay().PushTransformation();
				mDecoratorManager.GetDisplay().RenderQuadFilled(0,0,
						mDecoratorManager.GetDisplay().GetWidth(),0,
						mDecoratorManager.GetDisplay().GetWidth(),mDecoratorManager.GetDisplay().GetHeight(),
						0,mDecoratorManager.GetDisplay().GetHeight(),
						scBLACK.r, scBLACK.g, scBLACK.b, 1.0f);
				mDecoratorManager.GetDisplay().PopTransformation();
//	}
}


