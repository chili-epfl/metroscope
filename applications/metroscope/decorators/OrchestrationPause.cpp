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

#include "OrchestrationPause.hpp"
#include <iostream>

const std::string decorators::OrchestrationPause::scDecoratorName("OrchestrationPause");
const  DecoratorManager::Registerer decorators::OrchestrationPause::mRegisterer(decorators::OrchestrationPause::scDecoratorName, &decorators::OrchestrationPause::create);

decorators::FiducialDecorator *decorators::OrchestrationPause::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::OrchestrationPause(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::OrchestrationPause::OrchestrationPause(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker):
FiducialDecorator(pDecoratorManager, pMarker, true)
{
}

void decorators::OrchestrationPause::update() {
	//std::cout << "Entering orchestration decorator. Classroom pause: " << stateManager->isClassroomPaused() << std::endl;
	if (mMarker->isPresent())
	{
		stateManager->addMarkerToDeviceState("pause");
		stateManager->SetClassroomPaused(true);
	}
	else{
		stateManager->removeMarkerFromDeviceState("pause");
		if(stateManager->getPauserDevice().compare(stateManager->getDeviceId())==0) stateManager->SetClassroomPaused(false);//if this was the device that paused, we unpause
	}
	if(stateManager->isClassroomPaused()) blackoutScreen();


}

// Just draws a big black square on the screen
void decorators::OrchestrationPause::blackoutScreen(){
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


