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

#include "ResetSentenceChecker.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>
#include <wykobi/wykobi_utilities.hpp>

const std::string decorators::ResetSentenceChecker::scDecoratorName("ResetSentenceChecker");
const DecoratorManager::Registerer decorators::ResetSentenceChecker::mRegisterer(decorators::ResetSentenceChecker::scDecoratorName, &decorators::ResetSentenceChecker::create);

decorators::FiducialDecorator *decorators::ResetSentenceChecker::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::ResetSentenceChecker(
				pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),
				(Flipper *) pDecoratorManager.loadDecorator(pSetting["flipper"]),
				(GrammarChecker *) pDecoratorManager.loadDecorator(pSetting["target"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::ResetSentenceChecker::ResetSentenceChecker(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, Flipper *pFlipper, GrammarChecker *pChecker):
FiducialDecorator(pDecoratorManager, pMarker),
mFlipper(pFlipper),
mChecker(pChecker),
mLastShot(Time::MillisTimestamp())
{
}

void decorators::ResetSentenceChecker::update()
{
	//If the flipper is present and flipped and in side 2, we reset the sentence buffer and show a confirmation message in red
	if (mFlipper->IsPresent() && (mFlipper->IsFlipped() || (mLastShot < mFlipper->GetTimeOfLastFlip()+scRESET_MESSAGE_TIMER)) && mFlipper->IsSide2()){

		int numSentences = mChecker->GetNumSentences();
		mChecker->ResetBuffer();
		displayConfirmationReset(numSentences);

	} else if (mFlipper->IsPresent() && mFlipper->IsSide1()){	//If the flipper is present and in side 1, we show the big red confirmation question

		displayConfirmationQuestion();

	}


}

//Displays a confirmation of the reset next to the reset card
void decorators::ResetSentenceChecker::displayConfirmationReset(int numSentencesDeleted){

		mDecoratorManager.GetDisplay().PushTransformation();
		//mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mFlipper->GetCurrentSide(), scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
			mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(scRESET_MESSAGE.c_str(), scTEXT_DELIMITERS,
					mMarker->getCenter().x, mMarker->getCenter().y, scRESET_MESSAGE_WIDTH,
					false, scRESET_SCALE,
					scDEEP_RED.r, scDEEP_RED.g, scDEEP_RED.b, scDEEP_RED.a);
		mDecoratorManager.GetDisplay().PopTransformation();

}

//Puts all the screen in red and displays a question next to the card
void decorators::ResetSentenceChecker::displayConfirmationQuestion(){

		mDecoratorManager.GetDisplay().PushTransformation();
		//display the color bar indicating that this is important
		mDecoratorManager.GetDisplay().RenderQuadFilled(0.0f, 0.0f,
				mDecoratorManager.GetDisplay().GetWidth(), 0.0f,
				mDecoratorManager.GetDisplay().GetWidth(), mDecoratorManager.GetDisplay().GetHeight(),
				0.0f, mDecoratorManager.GetDisplay().GetHeight(),
				scLIGHT_RED.r, scLIGHT_RED.g, scLIGHT_RED.b, 0.3f);

		//mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mFlipper->GetCurrentSide(), scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
			mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(scRESET_QUESTION.c_str(), scTEXT_DELIMITERS,
					mMarker->getCenter().x, mMarker->getCenter().y, scRESET_MESSAGE_WIDTH,
					false, scRESET_SCALE,
					scDEEP_RED.r, scDEEP_RED.g, scDEEP_RED.b, scDEEP_RED.a);

		mDecoratorManager.GetDisplay().PopTransformation();



}

