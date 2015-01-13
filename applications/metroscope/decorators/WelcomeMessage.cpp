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

#include "WelcomeMessage.hpp"
#include <iostream>

const std::string decorators::WelcomeMessage::scDecoratorName("WelcomeMessage");
const  DecoratorManager::Registerer decorators::WelcomeMessage::mRegisterer(decorators::WelcomeMessage::scDecoratorName, &decorators::WelcomeMessage::create);

decorators::FiducialDecorator *decorators::WelcomeMessage::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting &tActiveMarkersString = pSetting ["active_markers"];

		int numActiveMarkers = pSetting["num_active_markers"];
		Executor **tActiveMarkers = new Executor *[numActiveMarkers];

		for(int i = 0 ; i < numActiveMarkers ; ++i){
			tActiveMarkers[i] = (Executor *) pDecoratorManager.loadDecorator(tActiveMarkersString[i]);
		}

		return new decorators::WelcomeMessage(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), pSetting["message"], pSetting["img_file"], numActiveMarkers, tActiveMarkers);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::WelcomeMessage::WelcomeMessage(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, std::string message, std::string imgFile, int numActiveMarkers, Executor ** activeMarkers):
FiducialDecorator(pDecoratorManager, pMarker),
sMessage(message),
sImgFile(imgFile),
mNumActiveMarkers(numActiveMarkers),
mActiveMarkers(activeMarkers)
{
}

void decorators::WelcomeMessage::update() {
	if (mMarker->isPresent())
	{
		//We check that none of the demo markers are present
		bool markersPresent = false;
		for(int i = 0; i < mNumActiveMarkers; i++){
			if(mActiveMarkers[i]->IsPresent()){
				markersPresent = true;
				break;
			}
		}

		if(!markersPresent){ //If no markers are present, we display the message and epfl logo
			displayMessage();
		}
	}
}

/*Just displays a text message next to the marker*/
void decorators::WelcomeMessage::displayMessage(){
	if(mMarker->isPresent()){
			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(sMessage.c_str(), scTEXT_DELIMITERS,
						mMarker->getCenter().x, mMarker->getCenter().y, mDecoratorManager.GetDisplay().GetWidth()-100,
						false, scHINT_MESSAGE_SCALE,
						scBLACK.r, scBLACK.g, scBLACK.b, scBLACK.a);

			int tTextureId = mDecoratorManager.GetDisplay().LoadTexture(sImgFile.c_str()); //114x236 image
			mDecoratorManager.GetDisplay().RenderTexture(tTextureId,
				mDecoratorManager.GetDisplay().GetWidth()-256, mDecoratorManager.GetDisplay().GetHeight()-134,
				mDecoratorManager.GetDisplay().GetWidth()-20, mDecoratorManager.GetDisplay().GetHeight()-134,
				mDecoratorManager.GetDisplay().GetWidth()-20, mDecoratorManager.GetDisplay().GetHeight()-20,
				mDecoratorManager.GetDisplay().GetWidth()-256, mDecoratorManager.GetDisplay().GetHeight()-20);


				mDecoratorManager.GetDisplay().PopTransformation();
	}
}

