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

#include "DenominatorCard.hpp"
#include <iostream>

const std::string decorators::DenominatorCard::scDecoratorName("DenominatorCard");
const DecoratorManager::Registerer decorators::DenominatorCard::mRegisterer(decorators::DenominatorCard::scDecoratorName, &decorators::DenominatorCard::create);

decorators::FiducialDecorator *decorators::DenominatorCard::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager){
	try{
		return new decorators::DenominatorCard(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), pSetting["numb_equal_parts"]);
	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::DenominatorCard::DenominatorCard(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, int pDenominator):
		FiducialDecorator(pDecoratorManager,pMarker),
		mDenominator(pDenominator){

}

void decorators::DenominatorCard::update(){

}

