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

#include "FractionBugHint.hpp"
#include <iostream>

const std::string decorators::FractionBugHint::scDecoratorName("FractionBugHint");
const  DecoratorManager::Registerer decorators::FractionBugHint::mRegisterer(decorators::FractionBugHint::scDecoratorName, &decorators::FractionBugHint::create);

decorators::FiducialDecorator *decorators::FractionBugHint::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::FractionBugHint(pDecoratorManager, (FiducialMarker *)pDecoratorManager.loadMarker(pSetting["marker"]),(int)pSetting ["hint_type"]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::FractionBugHint::FractionBugHint(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, int pHintType):
FiducialDecorator(pDecoratorManager, pMarker),
mHintType(pHintType)
{

}
decorators::FractionBugHint::~FractionBugHint(){ /*Empty*/}

void decorators::FractionBugHint::update() {
}


