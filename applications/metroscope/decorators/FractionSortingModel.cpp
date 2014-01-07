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

#include "FractionSortingModel.hpp"
#include <iostream>

const std::string decorators::FractionSortingModel::scDecoratorName("FractionSortingModel");
const  DecoratorManager::Registerer decorators::FractionSortingModel::mRegisterer(decorators::FractionSortingModel::scDecoratorName, &decorators::FractionSortingModel::create);


decorators::FiducialDecorator *decorators::FractionSortingModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::FractionSortingModel(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),
									pDecoratorManager.loadMarker(pSetting["stabilized_graphics_marker"]),
									(FractionModel *)pDecoratorManager.loadDecorator(pSetting["fraction_model"]),
									(FractionAnswerChecker *)pDecoratorManager.loadDecorator(pSetting["answer_checker"]),
									pDecoratorManager.loadMarker((pSetting["largest_marker"])));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::FractionSortingModel::FractionSortingModel(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pStabilizedGraphicsMarker, FractionModel *pModel, FractionAnswerChecker *pChecker, /*FiducialMarker *pSmallest,*/ FiducialMarker *pLargest):
FractionActivityCard(pDecoratorManager, pMarker, pStabilizedGraphicsMarker, pModel, pChecker, scSORTING_NAME, scSORTING_ACTIVITY_COLOR),
mLargestMarker(pLargest)
{

}

decorators::FractionSortingModel::~FractionSortingModel(){/*Empty*/}

bool decorators::FractionSortingModel::properOperatorsPresent(){
	if(!mLargestMarker->isPresent()) {
		mErrorMessage = "Please place the card marked \"LARGEST\" on the table";
	}
	return mLargestMarker->isPresent();
}
bool decorators::FractionSortingModel::getOperands(){
	wykobi::point2d<float> tSmallestCenter = mMarker->getCenter();
	wykobi::point2d<float> tLargestCenter = mLargestMarker->getCenter();
	mDecoratorManager.GetCam2World().InterpolatedMap(tSmallestCenter);
	mDecoratorManager.GetCam2World().InterpolatedMap(tLargestCenter);

	wykobi::vector2d<float> tXVector = wykobi::make_vector(tLargestCenter.x - tSmallestCenter.x, tLargestCenter.y - tSmallestCenter.y);
	wykobi::vector2d<float> tYVector = -1.0f*wykobi::perpendicular(tXVector);

	float tXRange = wykobi::distance(tSmallestCenter, tLargestCenter); //VECTOR NORM

	mCardsInRange.clear();
	mFractionModel->getCardsInsideRectangle(mCardsInRange, tSmallestCenter, tXVector, tYVector, 0.0f, tXRange, -scCARD_HEIGHT, scCARD_HEIGHT);
	if(mCardsInRange.size() == 0) {
		mErrorMessage = scEMPTY_SORTING_MESSAGE;
		return false;
	}
	return true;
}
bool decorators::FractionSortingModel::answerIsCorrect (){
	return cardsInCorrectOrder();
}

void decorators::FractionSortingModel::updateHintState(){
	mFractionModel->IncreaseHintLevel();
	if(mFractionModel->GetHintLevel()== 1) mFractionModel->SetHintMessageGraphicsType(scHINT_MESSAGE_GRAPHICS_TYPE);
	int tNumerator1, tDenominator1, tNumerator2, tDenominator2;
	switch(mFractionModel->GetHintLevel()){
		case 1:
			mFractionModel->SetHintMessageGraphicsFractions(3,4,5,6);
			mFractionModel->SetHintMessage(scSORTING_HINT_MESSAGE_1);
			mFractionModel->SetHintErrorType(FractionWithLabels);
			break;
		case 2:
			tNumerator1 = 1;			//FIXME - Get RID OF MAGIC NUMBERS
			tDenominator1 = 2;
			tNumerator2 = 1;
			tDenominator2 = 3;
			sprintf(mStringBuffer, scSORTING_HINT_MESSAGE_2, tNumerator1, tDenominator1, tNumerator2, tDenominator2);
			mFractionModel->SetHintMessage(mStringBuffer);
			mFractionModel->SetHintErrorType(TwoFractionsWithGraphics);
			mFractionModel->SetHintMessageGraphicsFractions(tNumerator1, tDenominator1, tNumerator2, tDenominator2);
			break;
		default: break;
	}
}

/*Checks that cards within the range (mCardsInRange) are in the correct order*/

bool decorators::FractionSortingModel::cardsInCorrectOrder()
{
		wykobi::point2d<float> tSmallestCenter = mMarker->getCenter();//mSmallestMarker->getCenter();
		wykobi::point2d<float> tLargestCenter = mLargestMarker->getCenter();
		mDecoratorManager.GetCam2World().InterpolatedMap(tSmallestCenter);
		mDecoratorManager.GetCam2World().InterpolatedMap(tLargestCenter);

		float tXOrientation = tLargestCenter.x - tSmallestCenter.x;

		std::sort(mCardsInRange.begin(), mCardsInRange.end(), FractionModel::FractionCardSortByValue);

		wykobi::point2d<float> tPreviousCard;
		wykobi::point2d<float> tCurrentCard;
		wykobi::line<float, 2> tXAxis = wykobi::make_line(tSmallestCenter, tLargestCenter);
		for(std::vector<FractionCard *>::iterator it = mCardsInRange.begin(); it != mCardsInRange.end(); ++it) {
			FractionCard *tCard = *it;
			tCurrentCard = wykobi::closest_point_on_line_from_point(tXAxis, tCard->GetLocation());
			if(tPreviousCard.x != 0){
				if(tXOrientation > 0 && tCurrentCard.x < tPreviousCard.x) return false;
				else if (tXOrientation <= 0 && tCurrentCard.x > tPreviousCard.x)return false;
			}
			tPreviousCard = tCurrentCard;
		}
		return true;
}


void decorators::FractionSortingModel::updateActiveCards(){
	mFractionModel->GetActiveCards().clear();
	for(std::vector<FractionCard *>::iterator it = mCardsInRange.begin(); it != mCardsInRange.end(); ++it) {
		mFractionModel->GetActiveCards().push_back(*it);
	}
}




