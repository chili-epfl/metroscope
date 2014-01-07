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

#include "VariableFractionCard.hpp"
#include <iostream>
#include "../FractionsConstants.hpp"

const std::string decorators::VariableFractionCard::scDecoratorName("VariableFractionCard");
const  DecoratorManager::Registerer decorators::VariableFractionCard::mRegisterer(decorators::VariableFractionCard::scDecoratorName, &decorators::VariableFractionCard::create);

decorators::FiducialDecorator *decorators::VariableFractionCard::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::VariableFractionCard(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), pSetting["default_numerator"], pSetting ["default_denominator"], pSetting["default_fraction_str"]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::VariableFractionCard::VariableFractionCard(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const int pDefaultNumerator, const int pDefaultDenominator, const std::string pDefaultFractionStr):
FractionCard(pDecoratorManager, pMarker, pDefaultNumerator, pDefaultDenominator, pDefaultFractionStr)
{

}

void decorators::VariableFractionCard::update() {
	if (mMarker->isPresent()){
		char tNumeratorText[scMAX_NUMERATOR_DIGITS + 1];
		char tDenominatorText[scMAX_DENOMINATOR_DIGITS + 1];
		sprintf(tNumeratorText, "%d", mNumerator);
		sprintf(tDenominatorText, "%d", mDenominator);
		DisplayFraction(tNumeratorText, tDenominatorText, 0.0f, 0.0f, false, scVARIABLE_FRACTION_COLOR);
	}
}

void decorators::VariableFractionCard::DisplayFraction(const char *pNumStr, const char *pDenomStr, float pXOffset, float pYOffset, bool pDrawCenterLine, const color *pFractionColor){

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarker, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
	mDecoratorManager.GetDisplay().RenderCenteredText(pNumStr,
								pXOffset, pYOffset - scVARIABLE_FRACTION_NUM_AND_DENOM_VERTICAL_OFFSET,
								true, scVARIABLE_FRACTION_NUMBER_SCALE,
								pFractionColor->r, pFractionColor->g, pFractionColor->b, pFractionColor->a);
	mDecoratorManager.GetDisplay().RenderCenteredText(pDenomStr,
								pXOffset, pYOffset + scVARIABLE_FRACTION_NUM_AND_DENOM_VERTICAL_OFFSET,
								true, scVARIABLE_FRACTION_NUMBER_SCALE,
								pFractionColor->r, pFractionColor->g, pFractionColor->b, pFractionColor->a);

	if(pDrawCenterLine) mDecoratorManager.GetDisplay().RenderLine(pXOffset - scVARIABLE_FRACTION_DIVISION_BAR_WIDTH, pYOffset,
											pXOffset + scVARIABLE_FRACTION_DIVISION_BAR_WIDTH, pYOffset,
											pFractionColor->r, pFractionColor->g, pFractionColor->b, pFractionColor->a);

	mDecoratorManager.GetDisplay().PopTransformation();
}

