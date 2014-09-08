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

#include "FractionToken.hpp"
#include <iostream>

const std::string decorators::FractionToken::scDecoratorName("FractionToken");
const DecoratorManager::Registerer decorators::FractionToken::mRegisterer(decorators::FractionToken::scDecoratorName, &decorators::FractionToken::create);

decorators::FiducialDecorator *decorators::FractionToken::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try{
		return new decorators::FractionToken(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),(int)pSetting["color"]);
	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;

}

decorators::FractionToken::FractionToken(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, int pType):
		FiducialDecorator(pDecoratorManager,pMarker),
		mType(pType),
		mQuadrant(0){

}

/*
 * Gets the position of the tokens and sets the quadrant
 * that they belong to.
 */
void decorators::FractionToken::update(){
	wykobi::point2d<float> tLocation = mMarker->getCenter();
	float tWidth = mDecoratorManager.GetDisplay().GetWidth();
	float tHeight = mDecoratorManager.GetDisplay().GetHeight();
	mQuadrant = 0;

	if(tLocation.x < tWidth/2){ // 2 or 3 quadrant
		if (tLocation.y < tHeight/2) mQuadrant = 2;
		else mQuadrant = 3;
	}else{
		if (tLocation.y < tHeight/2) mQuadrant = 1;
		else mQuadrant = 4;
	}
}

