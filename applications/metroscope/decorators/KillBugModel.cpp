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

#include "KillBugModel.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>
#include <wykobi/wykobi_utilities.hpp>

const std::string decorators::KillBugModel::scDecoratorName("KillBugModel");
const DecoratorManager::Registerer decorators::KillBugModel::mRegisterer(decorators::KillBugModel::scDecoratorName, &decorators::KillBugModel::create);

decorators::FiducialDecorator *decorators::KillBugModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager){
	try{
		libconfig::Setting &tFractionCardsString = pSetting ["fraction_cards"];
		libconfig::Setting &tCartesString = pSetting ["carte_cards"];
		libconfig::Setting &tHintCardsString = pSetting ["hint_cards"];

		FractionCard **tFractionCards = new FractionCard *[scFractionCards];
		Carte **tCarteCards = new Carte *[scCarteCards];
		FractionBugHint **tHintCards = new FractionBugHint *[scHintCards];

		for(int i = 0 ; i < scFractionCards ; ++i){
			tFractionCards[i] = (FractionCard *) pDecoratorManager.loadDecorator(tFractionCardsString[i]);
		}

		for(int i = 0 ; i < scCarteCards ; ++i){
			tCarteCards[i] = (Carte *) pDecoratorManager.loadDecorator(tCartesString[i]);
		}

		for(int i = 0 ; i < scHintCards ; ++i){
			tHintCards[i] = (FractionBugHint*) pDecoratorManager.loadDecorator(tHintCardsString[i]);
		}

		FlipperKillBug ** tFlipper = new FlipperKillBug *[2];

		tFlipper[0] = (FlipperKillBug *) pDecoratorManager.loadDecorator(pSetting["flipper_non_oriented"]);
		tFlipper[1] = (FlipperKillBug *) pDecoratorManager.loadDecorator(pSetting["flipper_oriented"]);

		return new decorators::KillBugModel (pDecoratorManager,
				(CircularFractionModel *) pDecoratorManager.loadDecorator(pSetting["circular_1"]),
				(CircularFractionModel *) pDecoratorManager.loadDecorator(pSetting["circular_2"]),
				(RectangleFractionModel *) pDecoratorManager.loadDecorator(pSetting["rectangle_1"]),
				(RectangleFractionModel *) pDecoratorManager.loadDecorator(pSetting["rectangle_2"]),
				(TokenModel *) pDecoratorManager.loadDecorator(pSetting["token_1"]),
				(FlipperKillBug **) tFlipper,
				(FractionBugHint **) tHintCards, (FractionCard **) tFractionCards, (Carte **) tCarteCards);

	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::KillBugModel::KillBugModel(DecoratorManager &pDecoratorManager, CircularFractionModel *pAngleModel1,
		CircularFractionModel *pAngleModel2, RectangleFractionModel *pRectangleModel1, RectangleFractionModel *pRectangleModel2,
		TokenModel *pTokenModel1, FlipperKillBug **pFlipper,
		FractionBugHint ** pFractionHints,
		FractionCard ** pFractionCards, Carte ** pCartes):
			FiducialDecorator(pDecoratorManager, 0),
			mCircularModel1(pAngleModel1),
			mCircularModel2(pAngleModel2),
			mRectangleModel1(pRectangleModel1),
			mRectangleModel2(pRectangleModel2),
			mTokenModel(pTokenModel1),
			mHints(pFractionHints),
			mFractionCards(pFractionCards),
			mCartes(pCartes),
			mFlipper(pFlipper),
			mActualFlipper(0),
			mActualCarte(0),
			mActualHint(0),
			mProportion1(0.0),
			mProportion2(0.0),
			mProportion3(0.0),
			mProportion4(0.0),
			mBugPosition(wykobi::make_point(0,0)),
			mSteps(0),
			mGameStarted(false),
			mMapFinished(false),
			mMapNew(false),
			mActiveManipulatives(0),
			mLastShot(Time::MillisTimestamp()),
			mProportion1Numerator(0),
			mProportion1Denominator(1),
			mProportion2Numerator(0),
			mProportion2Denominator(1),
			mProportion3Numerator(0),
			mProportion3Denominator(1),
			mProportion4Numerator(0),
			mProportion4Denominator(1),
			mProportionNumerator(0),
			mProportionDenominator(0),
			mProportion(0),
			mWrongMove(false),
			mWrongMovementFrames(0),
			mNewMapFrames(0),
			mProportionFeedbackFrames13(0),
			mProportionFeedbackFrames24(0),
			mProportion1Greater(false),
			mProportion2Greater(false),
			mProportion3Greater(false),
			mProportion4Greater(false),
			mIsCurrentActivity(false)
{
}

decorators::KillBugModel::~KillBugModel(){ /*Empty*/}

/*
 * If the card is present and the game has started, then look for proportions
 * If the Flipper is in the table, then display the animation
 * If the Hint card is present, it will be stored
 */
void decorators::KillBugModel::update(){
	if(mIsCurrentActivity){
		if(IsCartePresent()){
			if(mGameStarted)	FetchProportions();

			if(IsFlipperPresent())	DisplayFlipperFeedback();

			CheckHints();
		}
	}else{
		mActualCarte = NULL;
	}
}

/*
 * Returns true if one of the maps has been showed, the map doesn't have to be
 * all the time in the table (to have less cards in the table)
 */
bool decorators::KillBugModel::IsCartePresent(){
	Carte * tPreviusCarte = mActualCarte;
	for(int i = 0 ; i < scCarteCards ; i++){
		if(mCartes[i]->isPresent()){
			mActualCarte = mCartes[i];
			if(tPreviusCarte != mActualCarte)	Start();
		}
	}

	if(mActualCarte!=NULL)	mMapFinished = mActualCarte->IsFinished();
	return (mActualCarte!=NULL);
}

/*
 * Checks if the Flipper is present. The hint doesn't have to stay all the
 * time in the table (to have less cards in the table)
 */
bool decorators::KillBugModel::IsFlipperPresent(){
	if(mFlipper[0]->IsPresent())	mActualFlipper = mFlipper[0];
	else if(mFlipper[1]->IsPresent())	mActualFlipper = mFlipper[1];
	else	mActualFlipper = NULL;

	return (mActualFlipper!=NULL);
}

/*
 * Returns true if one of the hints has been showed, it doesn't need to persist
 * (just to have less cards in the table)
 */
bool decorators::KillBugModel::IsHintPresent(){
	return (mActualHint!=NULL);
}

/*
 * Check each hint card and see which one is shown
 */
void decorators::KillBugModel::CheckHints(){
	for(int i = 0 ; i < scHintCards ; i++){
		if(mHints[i]->IsPresent()){
			mActualHint = mHints[i];
		}
	}
}

/*
 * Checks if the answer is "correct" (i.e. moving to a correct
 * position) only if the map is not finished
 */
void decorators::KillBugModel::MakeMove(){

	if(!mActualCarte->IsFinished()){
		int tNewPositionX , tNewPositionY;

		mProportion1Greater = false;
		mProportion3Greater = false;
		mProportion2Greater = false;
		mProportion4Greater = false;

		// wrongMove is: 1) try to go to an obstacle 2) try to go out the map
		// First check the proportion 2 and 4 (Upper-Left and Bottom-Right)
		float tComparision24 = mProportion2 - mProportion4;

		if(tComparision24 < 0 ){ //if prop 2 < prop 4
			tNewPositionX = (mBugPosition.x + 1 < mActualCarte->getSize()) ? mBugPosition.x + 1 : mBugPosition.x; // New pos is one step to the right, just if is a good move
			mWrongMove = !(mBugPosition.x +1 < mActualCarte->getSize()); // If tries to go out the map, then WrongMove is true

		} else if(tComparision24 > 0){ //if prop 2 > prop 4
			tNewPositionX = (mBugPosition.x > 0) ? mBugPosition.x - 1 : mBugPosition.x; // New pos is one step to the left, just if is a good move
			mWrongMove = !(mBugPosition.x > 0); // If tries to go out the map, then WrongMove is true

		} else { // prop 2 = prop4
			tNewPositionX = mBugPosition.x;
			mWrongMove = false;
		}

		mProportion4Greater = (tComparision24 < 0);
		mProportion2Greater = (tComparision24 > 0);

		// Now check the proportion 1 and 3 (Upper-Right and Bottom-Left)
		float tComparision13 = mProportion1 - mProportion3;

		if(tComparision13 < 0){ // prop 1 < prop 3
			if(!mWrongMove){
				tNewPositionY = (mBugPosition.y +1 < mActualCarte->getSize()) ? mBugPosition.y + 1 : mBugPosition.y; // New pos is one step down, just if is a good move
				mWrongMove = !(mBugPosition.y +1 < mActualCarte->getSize()); // If tries to go out the map, then WrongMove is true
			}
		} else if(tComparision13 > 0){ // prop 1 > prop 3
			if(!mWrongMove){
				tNewPositionY = (mBugPosition.y > 0) ?  mBugPosition.y - 1 :  mBugPosition.y; // New pos is one step up, just if is a good move
				mWrongMove = !(mBugPosition.y > 0); // If tries to go out the map, then WrongMove is true
			}
		} else { // prop 1 == prop 3
			if(!mWrongMove){
				tNewPositionY = mBugPosition.y;
				mWrongMove = false;
			}
		}

		mProportion1Greater = (tComparision13 > 0);
		mProportion3Greater = (tComparision13 < 0);

		if(mActualCarte->IsEmptyCell(tNewPositionX,tNewPositionY) ){
			if(!mWrongMove){
				mBugPosition.x = tNewPositionX;
				mBugPosition.y = tNewPositionY;
			}
		}else{ //Wrong Move
			mWrongMove = true;
		}

		// If the new position corresponds to an end, then mark
		// the map as finished
		if(mActualCarte->IsEndCell(mBugPosition.x,mBugPosition.y)){
			mActualCarte->FinishMap();
		}

		// Set 30 frames of feedback (wrong move or fraction greater than other)
		if(mWrongMove) mWrongMovementFrames = 30;
		if(mProportion2Greater || mProportion4Greater)	mProportionFeedbackFrames24 = 30;
		if(mProportion1Greater || mProportion3Greater)	mProportionFeedbackFrames13 = 30;

		// Add one more step.
		mSteps++;
	}


}

/*
 * Resets all the variables
 */
void decorators::KillBugModel::Start(){
	mGameStarted = false;
	mSteps = 0;
	mNewMapFrames = 30;
	mProportion1 = 0.0f;
	mProportion2 = 0.0f;
	mProportion3 = 0.0f;
	mProportion4 = 0.0f;
	mBugPosition.x = mActualCarte->getOriginPoint().x;
	mBugPosition.y = mActualCarte->getOriginPoint().y;
	mActualHint = NULL;

}

/*
 * Check each manipulative, in the following order: (1) Circular,
 * (2) Rectangular, (3) Token (4) Fraction. Then it saves the
 * proportion in the variables. It takes into account the first
 * 4 manipulatives present
 */
void decorators::KillBugModel::FetchProportions(){
	int tProportionNumber = 0;

	ClearProportions();

	//Check each manipulative
	int tCircularActive = CheckCircularManipulative(tProportionNumber);
	tProportionNumber += tCircularActive;

	int tRectangularActive = CheckRectangularManipulative(tProportionNumber);
	tProportionNumber += tRectangularActive;

	int tTokenActive = CheckTokenManipulative(tProportionNumber);
	tProportionNumber += tTokenActive;

	int tFractionActive = CheckFractionManipulative(tProportionNumber);
	tProportionNumber += tFractionActive;

	SaveProportions();
}

/*
 * Set everything to zero
 */
void decorators::KillBugModel::ClearProportions(){
	mProportion1 = 0.0f;
	mProportion2 = 0.0f;
	mProportion3 = 0.0f;
	mProportion4 = 0.0f;

	mProportion1Numerator = 0;
	mProportion1Denominator = 1;
	mProportion2Numerator = 0;
	mProportion2Denominator = 1;
	mProportion3Numerator = 0;
	mProportion3Denominator = 1;
	mProportion4Numerator = 0;
	mProportion4Denominator = 1;
	mProportionNumerator.clear();
	mProportionDenominator.clear();
	mProportion.clear();
}

/*
 * Puts in every vector the corresponding proportion value,
 * numerator or denominator
 */
void decorators::KillBugModel::SaveProportions(){
	mProportion.push_back(mProportion1);
	mProportion.push_back(mProportion2);
	mProportion.push_back(mProportion3);
	mProportion.push_back(mProportion4);

	mProportionNumerator.push_back(mProportion1Numerator);
	mProportionNumerator.push_back(mProportion2Numerator);
	mProportionNumerator.push_back(mProportion3Numerator);
	mProportionNumerator.push_back(mProportion4Numerator);

	mProportionDenominator.push_back(mProportion1Denominator);
	mProportionDenominator.push_back(mProportion2Denominator);
	mProportionDenominator.push_back(mProportion3Denominator);
	mProportionDenominator.push_back(mProportion4Denominator);


}

/*
 * Checks both circular manipulatives to see if they're present.
 * If they are, the numerator and denominator are asigned to a proportion
 */
int decorators::KillBugModel::CheckCircularManipulative(int pProportionNumber){
	int tCircularManipActive = 0;
	if (mCircularModel1->IsPresent() && pProportionNumber + tCircularManipActive < 4){
		tCircularManipActive++;
		SetProportionNumber(mCircularModel1->GetCenter(),mCircularModel1->Numerator(), mCircularModel1->Denominator());
	}if (mCircularModel2->IsPresent() && pProportionNumber + tCircularManipActive< 4){
		tCircularManipActive++;
		SetProportionNumber(mCircularModel2->GetCenter(),mCircularModel2->Numerator(), mCircularModel2->Denominator());
	}
	return tCircularManipActive;
}

/*
 * Checks both rectangular manipulatives to see if they're present.
 * If they are, the numerator and denominator are asigned to a proportion
 */
int decorators::KillBugModel::CheckRectangularManipulative(int pProportionNumber){
	int tRectangularManipActive = 0;
	if (mRectangleModel1->IsPresent() && pProportionNumber + tRectangularManipActive < 4){
		tRectangularManipActive++;
		SetProportionNumber(mRectangleModel1->getMarker().getCenter(),mRectangleModel1->Numerator(), mRectangleModel1->Denominator());
	}if (mRectangleModel2->IsPresent() && pProportionNumber + tRectangularManipActive < 4){
		tRectangularManipActive++;
		SetProportionNumber(mRectangleModel2->getMarker().getCenter(),mRectangleModel2->Numerator(), mRectangleModel2->Denominator());
	}

	return tRectangularManipActive;
}

/*
 * Checks token manipulatives to see if they're present.
 * If they are, the numerator and denominator are asigned to a proportion
 */
int decorators::KillBugModel::CheckTokenManipulative(int pProportionNumber){
	//Checking the token manipulative
	int tTokenManipActive = 0;
	if(mTokenModel->IsPresent() && pProportionNumber + tTokenManipActive < 4){
		if(!mTokenModel->AreTokensSpread()){ // If they're all in the same corner
			tTokenManipActive++;
			int tProportion = GetProportionNumber(mTokenModel->GetPosition());
			SetProportionNumber(tProportion,mTokenModel->GetNumerator(tProportion), mTokenModel->GetDenominator(tProportion)); // Set the proportion number
		}else{ // If they're spread
			if(pProportionNumber + tTokenManipActive < 4 && mTokenModel->IsPresent(1)){ // Check in first quadrant
				tTokenManipActive++;
				SetProportionNumber(1,mTokenModel->GetNumerator(1), mTokenModel->GetDenominator(1));
			}if(pProportionNumber + tTokenManipActive < 4 && mTokenModel->IsPresent(2)){ // Check in second quadrant
				tTokenManipActive++;
				SetProportionNumber(2,mTokenModel->GetNumerator(2), mTokenModel->GetDenominator(2));
			}if(pProportionNumber + tTokenManipActive < 4 && mTokenModel->IsPresent(3)){ // Check in third quadrant
				tTokenManipActive++;
				SetProportionNumber(3,mTokenModel->GetNumerator(3), mTokenModel->GetDenominator(3));
			}if(pProportionNumber + tTokenManipActive < 4 && mTokenModel->IsPresent(4)){ // Check in fourth quadrant
				tTokenManipActive++;
				SetProportionNumber(4,mTokenModel->GetNumerator(4), mTokenModel->GetDenominator(4));
			}
		}
	}
	return tTokenManipActive;
}

/*
 * Check fraction-card manipulatives to see if they're present.
 * If they are, the numerator and denominator are asigned to a proportion
 */
int decorators::KillBugModel::CheckFractionManipulative(int tProportionNumber){
	int tFractionManipActive = 0;
	for(int i = 0 ; i < scFractionCards ; i++){
		if(mFractionCards[i]->IsPresent() && tProportionNumber + tFractionManipActive < 4){
			tFractionManipActive++;
			SetProportionNumber(mFractionCards[i]->GetLocation(), mFractionCards[i]->GetNumerator(),mFractionCards[i]->GetDenominator());
		}
	}
	return tFractionManipActive;
}

/*
 * Given the position of the manipulative, it calculates the proportion (1,2,3 or 4)
 * @param the position
 * @return the value of the fraction: 1, 2, 3 or 4
 */
int decorators::KillBugModel::GetProportionNumber(wykobi::point2d<float> pPosition){
	if(pPosition.x < mDecoratorManager.GetDisplay().GetWidth()/2){
		if(pPosition.y < mDecoratorManager.GetDisplay().GetHeight()/2)	return 2;
		else return 3;
	}else{
		if(pPosition.y < mDecoratorManager.GetDisplay().GetHeight()/2)	return 1;
		else return 4;
	}
}


/*
 * Given the position and proportion, it set the corresponding proportion value
 */
void decorators::KillBugModel::SetProportionNumber(wykobi::point2d<float> pPosition, float pProportion){
	int tProportionNumber = GetProportionNumber(pPosition);

	switch(tProportionNumber){
		case 1: mProportion1 = pProportion; break;
		case 2: mProportion2 = pProportion; break;
		case 3: mProportion3 = pProportion; break;
		case 4: mProportion4 = pProportion; break;
	}
}

/*
 * Given the quadrant and the proportion, it sets the corresponding proportion value
 */
void decorators::KillBugModel:: SetProportionNumber(int pQuadrant, float pProportion){
	switch(pQuadrant){
		case 1: mProportion1 = pProportion;break;
		case 2: mProportion2 = pProportion;break;
		case 3: mProportion3 = pProportion;break;
		case 4: mProportion4 = pProportion;break;
	}
}

/*
 * Given the numerator and denominator, it sets the corresponding
 * numerator and denominator (taking into account the quadrant)
 */
void decorators::KillBugModel::SetProportionNumber(int pQuadrant, int pNumerator, int pDenominator){
	switch(pQuadrant){
		case 1:
			mProportion1Numerator = pNumerator;		mProportion1Denominator = pDenominator;
			SetProportionNumber(1,(pNumerator/(float)pDenominator));
			break;
		case 2:
			mProportion2Numerator = pNumerator;		mProportion2Denominator = pDenominator;
			SetProportionNumber(2,(pNumerator/(float)pDenominator));
			break;
		case 3:
			mProportion3Numerator = pNumerator;		mProportion3Denominator = pDenominator;
			SetProportionNumber(3,(pNumerator/(float)pDenominator));
			break;
		case 4:
			mProportion4Numerator = pNumerator;		mProportion4Denominator = pDenominator;
			SetProportionNumber(4,(pNumerator/(float)pDenominator));
			break;
	}
}

/*
 * Given the position, numerator and denominator; it sets the corresponding
 * value of numerator and denominator (considering the quadrant)
 */
void decorators::KillBugModel::SetProportionNumber(wykobi::point2d<float> pPosition, int pNumerator, int pDenominator){
	int tProportionNumber = GetProportionNumber(pPosition);

	switch(tProportionNumber){
		case 1:
			mProportion1Numerator = pNumerator;
			mProportion1Denominator = pDenominator;
			SetProportionNumber(pPosition,(pNumerator/(float)pDenominator));
			break;
		case 2:
			mProportion2Numerator = pNumerator;
			mProportion2Denominator = pDenominator;
			SetProportionNumber(pPosition,(pNumerator/(float)pDenominator));
			break;
		case 3:
			mProportion3Numerator = pNumerator;
			mProportion3Denominator = pDenominator;
			SetProportionNumber(pPosition,(pNumerator/(float)pDenominator));
			break;
		case 4:
			mProportion4Numerator = pNumerator;
			mProportion4Denominator = pDenominator;
			SetProportionNumber(pPosition,(pNumerator/(float)pDenominator));
			break;
	}
}

/*
 * Makes a move when correspond
 */
void decorators::KillBugModel::DisplayFlipperFeedback(){
	static const long cShotPreparationTime = 6l*1000l;
	long tElapsedTime = Time::MillisTimestamp() - mLastShot;

	// If the time is more than the animation, then make a move and set the time
	// as the last shot done
	if (mActualFlipper->IsFlipped() && tElapsedTime > cShotPreparationTime) {
		MakeMove();
		mLastShot = Time::MillisTimestamp();
	}
}
