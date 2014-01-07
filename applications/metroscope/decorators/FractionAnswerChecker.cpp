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

#include "FractionAnswerChecker.hpp"
#include <iostream>


const std::string decorators::FractionAnswerChecker::scDecoratorName("FractionAnswerChecker");
const  DecoratorManager::Registerer decorators::FractionAnswerChecker::mRegisterer(decorators::FractionAnswerChecker::scDecoratorName, &decorators::FractionAnswerChecker::create);

decorators::FiducialDecorator *decorators::FractionAnswerChecker::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::FractionAnswerChecker(pDecoratorManager, (Flipper *) pDecoratorManager.loadDecorator(pSetting["flipper"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::FractionAnswerChecker::FractionAnswerChecker(DecoratorManager &pDecoratorManager, Flipper *pFlipper):
FiducialDecorator(pDecoratorManager, 0),
mFlipper(pFlipper),
mCorrectFormat(false),
mMessage("")
{
}
decorators::FractionAnswerChecker::~FractionAnswerChecker(){
	/*Empty*/
}

void decorators::FractionAnswerChecker::update() {
	if(mFlipper->GetCurrentSide() != NULL && mFlipper->IsPresent()){
		if(!mCorrectFormat) mMessage = "";
		else if(IsChecking())displayCheckingGraphics(mFlipper->GetCurrentSide());
		else displayMessage(mFlipper->GetCurrentSide());
	}
}

void decorators::FractionAnswerChecker::displayCheckingGraphics(const FiducialMarker *pMarker) {

	long tTimeSinceLastFlip = Time::MillisTimestamp() - mFlipper->GetTimeOfLastFlip();
	float tTimePercent = (float)tTimeSinceLastFlip/scANSWER_CHECKING_TIME;
	float tIncompleteBarWidth = tTimePercent*scCHECKING_BAR_WIDTH;
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*pMarker, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_WIDTH_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
	mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth("Checking...", scTEXT_DELIMITERS, 0.0f, 0.0f, 5*scCARD_WIDTH_IN_PROJECTOR_COORDS/6, false,
										scANSWER_CHECKER_TEXT_SCALE, scCHECKING_TEXT_COLOR->r, scCHECKING_TEXT_COLOR->g, scCHECKING_TEXT_COLOR->b, scCHECKING_TEXT_COLOR->a);


	mDecoratorManager.GetDisplay().RenderQuadFilled(-scCHECKING_BAR_WIDTH/2, -scCHECKING_BAR_HEIGHT + scCHECKING_BAR_VERTICAL_OFFSET,
			-scCHECKING_BAR_WIDTH/2+tIncompleteBarWidth, -scCHECKING_BAR_HEIGHT+ scCHECKING_BAR_VERTICAL_OFFSET,
			-scCHECKING_BAR_WIDTH/2+tIncompleteBarWidth, scCHECKING_BAR_HEIGHT + scCHECKING_BAR_VERTICAL_OFFSET,
			-scCHECKING_BAR_WIDTH/2, scCHECKING_BAR_HEIGHT + scCHECKING_BAR_VERTICAL_OFFSET, scCHECKING_BAR_COLOR->r, scCHECKING_BAR_COLOR->g, scCHECKING_BAR_COLOR->b, scCHECKING_BAR_COLOR->a);

	mDecoratorManager.GetDisplay().PopTransformation();

}
void decorators::FractionAnswerChecker::displayMessage(const FiducialMarker *pMarker) {
	const color *tMessageColor = strcmp(mMessage, scCORRECT_MESSAGE) == 0? scCORRECT_COLOR: scINCORRECT_COLOR;
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*pMarker, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_WIDTH_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
	mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(mMessage, scTEXT_DELIMITERS, 0.0f, 0.0f, 5*scCARD_WIDTH_IN_PROJECTOR_COORDS/6, true, scANSWER_CHECKER_TEXT_SCALE, tMessageColor->r, tMessageColor->g, tMessageColor->b, tMessageColor->a);
	mDecoratorManager.GetDisplay().PopTransformation();
}
