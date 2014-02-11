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

#include "WordsCard.hpp"
#include "../FractionsConstants.hpp"
#include <iostream>

const std::string decorators::WordsCard::scDecoratorName("WordsCard");
const  DecoratorManager::Registerer decorators::WordsCard::mRegisterer(decorators::WordsCard::scDecoratorName, &decorators::WordsCard::create);

decorators::FiducialDecorator *decorators::WordsCard::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::WordsCard(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), pSetting["words"], pSetting["guideline"]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::WordsCard::WordsCard(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, std::string pWords, bool pGuideline):
FiducialDecorator(pDecoratorManager, pMarker),
mWords(pWords),
mGuideline(pGuideline)
{
}

void decorators::WordsCard::update() {
	if (mMarker->isPresent())
	{
		//If the guideline is activated, we draw two lines above and below the card, to guide in the construction of the sentence
		if(mGuideline){
			mDecoratorManager.GetDisplay().PushTransformation();
			//mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarker, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
			mDecoratorManager.GetDisplay().RenderLine(0.0f, mMarker->getCenter().y+scGUIDELINE_DISTANCE, mDecoratorManager.GetDisplay().GetWidth(), mMarker->getCenter().y+scGUIDELINE_DISTANCE, scBLACK.r, scBLACK.g, scBLACK.b, scBLACK.a);
			mDecoratorManager.GetDisplay().RenderLine(0.0f, mMarker->getCenter().y-scGUIDELINE_DISTANCE, mDecoratorManager.GetDisplay().GetWidth(), mMarker->getCenter().y-scGUIDELINE_DISTANCE, scBLACK.r, scBLACK.g, scBLACK.b, scBLACK.a);
			//mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(sMessage.c_str(), scTEXT_DELIMITERS,
				//		scHINT_MESSAGE_OFFSET_X, scHINT_MESSAGE_OFFSET_Y, scHINT_MESSAGE_WIDTH,
				//		false, scHINT_MESSAGE_SCALE,
				//		scMESSAGE_COLOR->r, scMESSAGE_COLOR->g, scMESSAGE_COLOR->b, scMESSAGE_COLOR->a);
			mDecoratorManager.GetDisplay().PopTransformation();

		}
	}
}
