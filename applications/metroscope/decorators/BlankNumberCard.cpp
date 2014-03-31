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

#include "BlankNumberCard.hpp"
#include <iostream>

const std::string decorators::BlankNumberCard::scDecoratorName("BlankNumberCard");
const DecoratorManager::Registerer decorators::BlankNumberCard::mRegisterer(decorators::BlankNumberCard::scDecoratorName, &decorators::BlankNumberCard::create);

decorators::FiducialDecorator *decorators::BlankNumberCard::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::BlankNumberCard(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), pSetting["digit_type"]);
	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::BlankNumberCard::BlankNumberCard(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, int pType):
		FiducialDecorator(pDecoratorManager, pMarker),
		mType(pType),
		mNumber(),
		mIsInsideRectangle(false)
{
	switch(mType)
		{
			case 0: r = 0.0f; g = 0.0f; b = 204.0f; break;
			case 1: r = 0.0f; g = 153.0f; b = 0.0f;break;
			case 2: r = 255.0f; g = 255.0f; b = 0.0f;break;
		}
}

void decorators::BlankNumberCard::update() {

}

void decorators::BlankNumberCard::DisplayNumber(const char *pNumber, float pXOffset, float pYOffset){
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarker, 20.0f, 20.0f, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());

	mDecoratorManager.GetDisplay().RenderCenteredText(pNumber,pXOffset, pYOffset,true, 1.0f,r, g, b, 1.0f);
	mDecoratorManager.GetDisplay().PopTransformation();
}


