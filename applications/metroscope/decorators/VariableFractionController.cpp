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

#include "VariableFractionController.hpp"
#include <iostream>

const std::string decorators::VariableFractionController::scDecoratorName("VariableFractionController");
const  DecoratorManager::Registerer decorators::VariableFractionController::mRegisterer(decorators::VariableFractionController::scDecoratorName, &decorators::VariableFractionController::create);

decorators::FiducialDecorator *decorators::VariableFractionController::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::VariableFractionController(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),
															(VariableFractionCard *) pDecoratorManager.loadDecorator(pSetting["variable_fraction"]),
															pDecoratorManager.loadMarker(pSetting["stabilized_graphics_marker"]),
															pSetting["isNumerator"]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::VariableFractionController::VariableFractionController(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, VariableFractionCard *pVariableFractionCard, FiducialMarker *pStabilizedGraphicsMarker, bool pIsNumerator):
NumberDial(pDecoratorManager,
		pMarker,
		pStabilizedGraphicsMarker,
		pIsNumerator?scMIN_NUMERATOR:scMIN_DENOMINATOR,
		pIsNumerator? scMAX_NUMERATOR - scMIN_NUMERATOR + 1: scMAX_DENOMINATOR - scMIN_DENOMINATOR + 1,
		scDIAL_MAX_NUMBERS_PER_CIRCLE,
		1,
		pIsNumerator? scNUMERATOR_DIAL_COLOR: scDENOMINATOR_DIAL_COLOR),
mType(pIsNumerator?Numerator:Denominator),
mVariableFractionCard(pVariableFractionCard)
{
}


void decorators::VariableFractionController::update() {
	if (mMarker->isPresent())
		{
			float tAngle = mMarker->getAngle();
			updateValue(tAngle);
			mType == Numerator? mVariableFractionCard->SetNumerator(mCurrentNumber) : mVariableFractionCard->SetDenominator(mCurrentNumber);
			displayDialNumbers(tAngle);
		}
}
