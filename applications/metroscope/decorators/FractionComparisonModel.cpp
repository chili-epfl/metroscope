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

#include "FractionComparisonModel.hpp"
#include <iostream>

const std::string decorators::FractionComparisonModel::scDecoratorName("FractionComparisonModel");
const  DecoratorManager::Registerer decorators::FractionComparisonModel::mRegisterer(decorators::FractionComparisonModel::scDecoratorName, &decorators::FractionComparisonModel::create);

decorators::FiducialDecorator *decorators::FractionComparisonModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {

		ComparisonType tComparisonType;
		const char *tActivityName;
		const color *tActivityColor;
		switch((int)pSetting["comparison_type"]){
		case 0: tComparisonType = GreaterThan; tActivityName = scCOMPARISON_MORE_THAN_NAME; tActivityColor = scCOMPARISON_MORE_THAN_ACTIVITY_COLOR; break;
		case 1: tComparisonType = LesserThan; tActivityName = scCOMPARISON_LESS_THAN_NAME; tActivityColor = scCOMPARISON_LESS_THAN_ACTIVITY_COLOR; break;
		case 2: tComparisonType = EqualTo; tActivityName = scCOMPARISON_EQUAL_TO_NAME; tActivityColor = scCOMPARISON_EQUAL_TO_ACTIVITY_COLOR; break;
		default:
			std::cerr << "Creating a comparison activity decorator: Invalid comparison type" << std::endl;
			return 0;
		}

		return new decorators::FractionComparisonModel(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),
														pDecoratorManager.loadMarker(pSetting["stabilized_graphics_marker"]),
														(FractionModel *)pDecoratorManager.loadDecorator(pSetting["fraction_model"]),
														(FractionAnswerChecker *)pDecoratorManager.loadDecorator(pSetting["answer_checker"]),
														tComparisonType,
														tActivityName,
														tActivityColor
														);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::FractionComparisonModel::FractionComparisonModel(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pStabilizedGraphicsMarker, FractionModel *pModel, FractionAnswerChecker *pChecker, ComparisonType pComparisonType, const char *pActivityName, const color *pActivityColor):
FractionActivityCard(pDecoratorManager, pMarker, pStabilizedGraphicsMarker, pModel, pChecker, pActivityName, pActivityColor),
mFractionModel(pModel),
mComparisonType(pComparisonType)
{

}

decorators::FractionComparisonModel::~FractionComparisonModel(){
	/*Empty*/
}


bool decorators::FractionComparisonModel::properOperatorsPresent(){
	return true;
}

bool decorators::FractionComparisonModel::getOperands(){
	wykobi::quadix<float ,2> tMarkerCorners = mMarker->getCorners();
	mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
	wykobi::point2d<float> tComparisonCenter;
	wykobi::point2d<float> tXUnit;
	wykobi::point2d<float> tYUnit;
	FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tComparisonCenter, tXUnit, tYUnit);

	std::vector<decorators::FractionCard *> tCardArray;

	mFractionModel->getCardsInsideRectangle(tCardArray, tComparisonCenter, wykobi::make_vector(tXUnit), wykobi::make_vector(tYUnit), -3*scCARD_WIDTH, 0.0f, -scCARD_HEIGHT, scCARD_HEIGHT);
	if(tCardArray.size() != 1) {
		mErrorMessage = scCOMPARISON_INCORRECT_FORMAT_MESSAGE;
		return false;
	}
	mLHS = tCardArray[0];
	tCardArray.clear();

	mFractionModel->getCardsInsideRectangle(tCardArray, tComparisonCenter, wykobi::make_vector(tXUnit), wykobi::make_vector(tYUnit), 0.0f, 3*scCARD_WIDTH, -scCARD_HEIGHT, scCARD_HEIGHT);
	if(tCardArray.size() != 1) {
		mErrorMessage = scCOMPARISON_INCORRECT_FORMAT_MESSAGE;
		return false;
	}
	mRHS = tCardArray[0];
	return true;
}

bool decorators::FractionComparisonModel::answerIsCorrect(){
	switch (mComparisonType) {
			case LesserThan: return mLHS->GetValue() < mRHS->GetValue();
			case GreaterThan: return mLHS->GetValue() > mRHS->GetValue();
			case EqualTo: return mLHS->GetValue() == mRHS->GetValue();
			default: return false;
	}
}

void decorators::FractionComparisonModel::updateHintState(){ //FIXME
	float tFactor;
	switch (mComparisonType) {
		case LesserThan:
			switch(mFractionModel->GetHintLevel()){
			case 0: mFractionModel->SetHintMessage(scCOMPARISON_LESS_THAN_HINT_MESSAGE_1); break;
			case 1: mFractionModel->SetHintMessage(scCOMPARISON_LESS_THAN_HINT_MESSAGE_2); break;
			default: mFractionModel->SetHintMessage(scCOMPARISON_LESS_THAN_HINT_MESSAGE_3); break;
			}
			mFractionModel->IncreaseHintLevel();
			break;
		case LesserThan:
			switch(mFractionModel->GetHintLevel()){
			case 0: mFractionModel->SetHintMessage(scCOMPARISON_MORE_THAN_HINT_MESSAGE_1); break;
			case 1: mFractionModel->SetHintMessage(scCOMPARISON_MORE_THAN_HINT_MESSAGE_2); break;
			default: mFractionModel->SetHintMessage(scCOMPARISON_MORE_THAN_HINT_MESSAGE_3); break;
			}
			mFractionModel->IncreaseHintLevel();
			break;
		case EqualTo:
			int tNum1, tDenom1, tNum2, tDenom2;

			if(mLHS->GetNumerator() < mRHS->GetNumerator()){
				tNum1 = mLHS->GetNumerator();
				tDenom1 = mLHS->GetDenominator();
				tNum2 = mRHS->GetNumerator();
				tDenom2 = mRHS->GetDenominator();
			} else {
				tNum1 = mRHS->GetNumerator();
				tDenom1 = mRHS->GetDenominator();
				tNum2 = mLHS->GetNumerator();
				tDenom2 = mLHS->GetDenominator();
			}

			tFactor = ((float)tNum2)/tNum1;
			if(std::fmod(tFactor, 1.0f) == 0){
				sprintf(mStringBuffer, scCOMPARISON_EQUAL_TO_HINT_MESSAGE_1, tNum1, tDenom1, tNum1,
											tFactor, tNum2, tDenom2, tNum2, tNum2, tDenom2, tFactor);
			} else {
				sprintf(mStringBuffer, scCOMPARISON_EQUAL_TO_HINT_MESSAGE_2, tNum1, tDenom1, tNum1,
											tFactor, tNum2, tDenom2, tNum2, tNum2, tDenom2, tFactor);
			}
			mFractionModel->SetHintMessage(mStringBuffer);
			break;
		default: break;
	}


}


void decorators::FractionComparisonModel::updateActiveCards(){
	mFractionModel->GetActiveCards().clear();
	mFractionModel->GetActiveCards().push_back(mLHS);
	mFractionModel->GetActiveCards().push_back(mRHS);
}


