/*******************************************************************************
*   Copyright 2014 EPFL                                                        *
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

#include "ActivityInstruction.hpp"
#include <iostream>

const std::string decorators::ActivityInstruction::scDecoratorName("ActivityInstruction");
const  DecoratorManager::Registerer decorators::ActivityInstruction::mRegisterer(decorators::ActivityInstruction::scDecoratorName, &decorators::ActivityInstruction::create);

decorators::FiducialDecorator *decorators::ActivityInstruction::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::ActivityInstruction(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), pSetting["message"], pDecoratorManager.loadMarker(pSetting["position"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::ActivityInstruction::ActivityInstruction(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, std::string pMessage, FiducialMarker *pTextPosition):
FiducialDecorator(pDecoratorManager, pMarker),
mMessage(pMessage),
mMessagePositionMarker(pTextPosition)
{
}

void decorators::ActivityInstruction::update() {
	if (mMarker->isPresent())
	{
		displayMessage();
	}
}

/*Just displays a text message next to the marker*/
void decorators::ActivityInstruction::displayMessage(){
	if(mMarker->isPresent()){
			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(mMessage.c_str(), scTEXT_DELIMITERS,
						mMessagePositionMarker->getCenter().x, mMessagePositionMarker->getCenter().y, scINSTRUCTION_WIDTH,
						false, scINSTRUCTION_SCALE,
						scMESSAGE_COLOR->r, scMESSAGE_COLOR->g, scMESSAGE_COLOR->b, scMESSAGE_COLOR->a);
			mDecoratorManager.GetDisplay().PopTransformation();
	}
}

