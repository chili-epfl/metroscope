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

#include "FractionSimplifier.hpp"
#include <iostream>

const std::string decorators::FractionSimplifier::scDecoratorName("FractionSimplifier");
const  DecoratorManager::Registerer decorators::FractionSimplifier::mRegisterer(decorators::FractionSimplifier::scDecoratorName, &decorators::FractionSimplifier::create);

decorators::FiducialDecorator *decorators::FractionSimplifier::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::FractionSimplifier(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), (VariableFractionCard *)pDecoratorManager.loadDecorator(pSetting["variable_fraction"]), pDecoratorManager.loadMarker(pSetting["stabilized_graphics_marker"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::FractionSimplifier::FractionSimplifier(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, VariableFractionCard *pVariableFractionCard, FiducialMarker *pStabilizedGraphicsMarker):
NumberDial(pDecoratorManager, pMarker, pStabilizedGraphicsMarker, 1, scMAX_SIMPLIFYING_FACTOR, scDIAL_MAX_NUMBERS_PER_CIRCLE, 1, scSIMPLIFIER_DIAL_COLOR),
mVariableFractionCard(pVariableFractionCard)
{
}

void decorators::FractionSimplifier::update() {
	if (mMarker->isPresent())
	{
		float tAngle = mMarker->getAngle();
		updateValue(tAngle);
		displayDialNumbers(tAngle);
		if(mVariableFractionCard->GetMarker()->isPresent())displaySimplifiedFractionGraphics();
	}


}

void decorators::FractionSimplifier::displaySimplifiedFractionGraphics(){
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*(mVariableFractionCard->GetMarker()), scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
	mDecoratorManager.GetDisplay().RenderCenteredText("=", 2*scCARD_WIDTH_IN_PROJECTOR_COORDS/3, 0.0f, true, scVARIABLE_FRACTION_NUMBER_SCALE,  scBLACK.r, scBLACK.g, scBLACK.b, scBLACK.a);
	mDecoratorManager.GetDisplay().RenderCenteredText("x", 4*scCARD_WIDTH_IN_PROJECTOR_COORDS/3, 0.0f, true, scVARIABLE_FRACTION_NUMBER_SCALE, mHighlightColor->r, mHighlightColor->g, mHighlightColor->b, mHighlightColor->a);
	mDecoratorManager.GetDisplay().PopTransformation();

	char tNumStr[scMAX_NUMERATOR_DIGITS + 5];
	char tDenomStr[scMAX_DENOMINATOR_DIGITS + 5];

	sprintf(tNumStr, "%d", mCurrentNumber);
	mVariableFractionCard->DisplayFraction(tNumStr, tNumStr, scCARD_WIDTH_IN_PROJECTOR_COORDS, 0.0f, true, mHighlightColor);

	float tReducedNumerator = ((float)mVariableFractionCard->GetNumerator())/mCurrentNumber;
	float tReducedDenominator = ((float)mVariableFractionCard->GetDenominator())/mCurrentNumber;
	float tIntPart;
	if(std::modf(tReducedNumerator, &tIntPart) == 0.0f && std::modf(tReducedDenominator, &tIntPart) ==0){
		sprintf(tNumStr, "%.0f", tReducedNumerator);
		sprintf(tDenomStr, "%.0f", tReducedDenominator);
	} else {
		sprintf(tNumStr, "%s", "XX");
		sprintf(tDenomStr, "%s", "XX");
	}

	mVariableFractionCard->DisplayFraction(tNumStr, tDenomStr, 5*scCARD_WIDTH_IN_PROJECTOR_COORDS/3, 0.0f, true, mHighlightColor);

}
