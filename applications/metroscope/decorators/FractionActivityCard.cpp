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

#include "FractionActivityCard.hpp"
#include <iostream>

const std::string decorators::FractionActivityCard::scDecoratorName("FractionActivityCard");
namespace{


}

decorators::FractionActivityCard::FractionActivityCard(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pStabilizedGraphicsMarker, FractionModel *pModel, FractionAnswerChecker *pChecker, const char * const pActivityName, const color *pActivityColor):
FiducialDecorator(pDecoratorManager, pMarker),
mStabilizedGraphicsMarker(pStabilizedGraphicsMarker),
mFractionModel(pModel),
mChecker(pChecker),
mActivityName(pActivityName),
mActivityColor(pActivityColor),
mErrorMessage(""),
mErrorMessageColor(scERROR_MESSAGE_COLOR),
mOperatorsPresent(false),
mOperandsPresent(false),
mStringBuffer(new char[scMAX_MESSAGE_STRING_LENGTH]),
mErrorStringBuffer(new char[scMAX_MESSAGE_STRING_LENGTH])
{
}

decorators::FractionActivityCard::~FractionActivityCard(){
	delete [] mStringBuffer;
	delete [] mErrorStringBuffer;
}

void decorators::FractionActivityCard::update() {
	if (mMarker->isPresent()) {
		if(strcmp(mFractionModel->GetCurrentActivity(), mActivityName) == 0){
			if(!mPreviouslyCurrentActivity){
				mFractionModel->ResetHintState();
				mPreviouslyCurrentActivity = true;
				mFractionModel->ClearActiveCards();
				mOperatorsPresent = false;
				mOperandsPresent = false;
			}
			displayActivityGraphics();
			if(properOperatorsPresent()){
				if (!mOperatorsPresent) mErrorMessage = ""; //Clears error message if operators were previously missing
				mOperatorsPresent = true;
				if(mChecker->IsFlipped()&& (mOperandsPresent = getOperands())){
						mErrorMessage = "";
						updateActiveCards();
						if(answerIsCorrect()){
							mFractionModel->ResetHintState();
							mChecker->SetMessage(scCORRECT_MESSAGE);
						} else {
							updateHintState();
							mChecker->SetMessage(scINCORRECT_MESSAGE);
						}
				}
			} else {
				mOperatorsPresent = false;
			}
			mChecker->SetCorrectFormat(mOperatorsPresent && mOperandsPresent);
		} else {
			mErrorMessage = otherActivityCardsPresentMessage();
			mPreviouslyCurrentActivity = false;
		}
		displayErrorMessage();
		mFractionModel->SetCurrentActivity(mActivityName);
	} else {
		mFractionModel->RemoveActivity(mActivityName);
	}
}



void decorators::FractionActivityCard::displayErrorMessage(){
	PlaceCenteredTextAtLocationRelativeToMarker(mErrorMessage, mStabilizedGraphicsMarker,
			scACTIVITY_ERROR_MESSAGE_OFFSET_X, scACTIVITY_ERROR_MESSAGE_OFFSET_Y,
			scACTIVITY_ERROR_MESSAGE_SCALE, scACTIVITY_ERROR_MESSAGE_WIDTH,
			scACTIVITY_ERROR_MESSAGE_COLOR->r, scACTIVITY_ERROR_MESSAGE_COLOR->g, scACTIVITY_ERROR_MESSAGE_COLOR->b, scACTIVITY_ERROR_MESSAGE_COLOR->a);
}

void decorators::FractionActivityCard::drawScreenBorder(float pThickness, const color *pColor){
	mDecoratorManager.GetDisplay().RenderQuadFilled(0, 0,
								mDecoratorManager.GetDisplay().GetWidth(), 0,
								mDecoratorManager.GetDisplay().GetWidth(), pThickness,
								0, pThickness,
								pColor->r, pColor->g, pColor->b, pColor->a);

		mDecoratorManager.GetDisplay().RenderQuadFilled(0, mDecoratorManager.GetDisplay().GetHeight() - pThickness,
									mDecoratorManager.GetDisplay().GetWidth(), mDecoratorManager.GetDisplay().GetHeight() - pThickness,
									mDecoratorManager.GetDisplay().GetWidth(), mDecoratorManager.GetDisplay().GetHeight(),
									0, mDecoratorManager.GetDisplay().GetHeight(),
									pColor->r, pColor->g, pColor->b, pColor->a);
		mDecoratorManager.GetDisplay().RenderQuadFilled(0, pThickness,
									pThickness, pThickness,
									pThickness, mDecoratorManager.GetDisplay().GetHeight() - pThickness,
									0, mDecoratorManager.GetDisplay().GetHeight() - pThickness,
									pColor->r, pColor->g, pColor->b, pColor->a);

		mDecoratorManager.GetDisplay().RenderQuadFilled(mDecoratorManager.GetDisplay().GetWidth() - pThickness, pThickness,
									mDecoratorManager.GetDisplay().GetWidth(), pThickness,
									mDecoratorManager.GetDisplay().GetWidth(), mDecoratorManager.GetDisplay().GetHeight() - pThickness,
									mDecoratorManager.GetDisplay().GetWidth() - pThickness, mDecoratorManager.GetDisplay().GetHeight() - pThickness,
									pColor->r, pColor->g, pColor->b, pColor->a);
}


void decorators::FractionActivityCard::displayActivityGraphics() {

	drawScreenBorder(scBORDER_THICKNESS, mActivityColor);

	mDecoratorManager.GetDisplay().PushTransformation();
	float tTextCenterX = 4*scBORDER_THICKNESS;
	float tTextCenterY = .5*mDecoratorManager.GetDisplay().GetHeight();
	mDecoratorManager.GetDisplay().Rotate(90.0f, tTextCenterX, tTextCenterY);
	mDecoratorManager.GetDisplay().RenderCenteredText(mActivityName, tTextCenterX, tTextCenterY, true, scACTIVITY_NAME_TEXT_SCALE, mActivityColor->r, mActivityColor->g, mActivityColor->b, mActivityColor->a);
	mDecoratorManager.GetDisplay().PopTransformation();


}


void decorators::FractionActivityCard::PlaceCenteredTextAtLocationRelativeToMarker(const char *pText, const FiducialMarker *pMarker, float pTextOffsetX, float pTextOffsetY, float pTextScale, float pTextWidth, float pR, float pG, float pB, float pA)
{
	if(pMarker->isPresent()){
			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*pMarker, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_WIDTH_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
			mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(pText, scTEXT_DELIMITERS, pTextOffsetX, pTextOffsetY, pTextWidth, false, pTextScale, pR, pG, pB, pA);
			mDecoratorManager.GetDisplay().PopTransformation();
	}
}

const char *decorators::FractionActivityCard::otherActivityCardsPresentMessage(){
	int tNumOtherActivities = mFractionModel->GetPresentActivities().size() - 1;
	int tNumActivitiesPrinted = 0;
	int tNumChars = strlen(scREMOVE_ACTIVITY_MESSAGE_PART_1);
	sprintf((char *)mErrorStringBuffer, scREMOVE_ACTIVITY_MESSAGE_PART_1); //"To do this activity, please remove the ";

	for(std::vector<const char*>::iterator it = mFractionModel->GetPresentActivities().begin(); it != mFractionModel->GetPresentActivities().end(); ++it){
		if((*it) != mActivityName){
			if(tNumOtherActivities > 1 && tNumActivitiesPrinted == tNumOtherActivities - 1){
				sprintf((char *)mErrorStringBuffer + tNumChars, " and");
				tNumChars += 4;
			}
			sprintf((char *)mErrorStringBuffer + tNumChars, " %s", *it);
			tNumChars += strlen(*it) + 1;
			if(tNumOtherActivities > 2 && tNumActivitiesPrinted < tNumOtherActivities - 1){
				sprintf((char *)mErrorStringBuffer + tNumChars, ",");
				tNumChars++;
			}
			tNumActivitiesPrinted++;
		}
	}
	tNumOtherActivities > 1? sprintf((char *)mErrorStringBuffer + tNumChars, scREMOVE_ACTIVITY_MESSAGE_PART_2_PLURAL): sprintf((char *)mErrorStringBuffer + tNumChars, scREMOVE_ACTIVITY_MESSAGE_PART_2_SINGULAR);
	return mErrorStringBuffer;
}


