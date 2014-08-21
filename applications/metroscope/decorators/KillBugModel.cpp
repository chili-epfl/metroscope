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

		return new decorators::KillBugModel (pDecoratorManager,
				(CircularFractionModel *) pDecoratorManager.loadDecorator(pSetting["circular_1"]),
				(CircularFractionModel *) pDecoratorManager.loadDecorator(pSetting["circular_2"]),
				(RectangleFractionModel *) pDecoratorManager.loadDecorator(pSetting["rectangle_1"]),
				(RectangleFractionModel *) pDecoratorManager.loadDecorator(pSetting["rectangle_2"]),
				(TokenModel *) pDecoratorManager.loadDecorator(pSetting["token_1"]),
				(Flipper *) pDecoratorManager.loadDecorator(pSetting["go_flipper"]),
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
		TokenModel *pTokenModel1, Flipper *pGoFlipper, FractionBugHint ** pFractionHints,
		FractionCard ** pFractionCards, Carte ** pCartes):
			FiducialDecorator(pDecoratorManager, 0),
			mCircularModel1(pAngleModel1), mCircularModel2(pAngleModel2),
			mRectangleModel1(pRectangleModel1), mRectangleModel2(pRectangleModel2),
			mTokenModel(pTokenModel1), mFlipper(pGoFlipper), mHints(pFractionHints), mFractionCards(pFractionCards), mCartes(pCartes),
			mActualCarte(0),mActualHint(0), mProportion1(0.0), mProportion2(0.0), mProportion3(0.0), mProportion4(0.0),
			mMapSize(0), mCellDimensionX(0), mCellDimensionY(0), mSteps(0),mGameStarted(false), mMapFinished(false),mMapNew(false),  mActiveManipulatives(0),mLastShot(Time::MillisTimestamp()),
			mProportion1Numerator(0), mProportion1Denominator(1), mProportion2Numerator(0), mProportion2Denominator(1), mProportion3Numerator(0),
			mProportion3Denominator(1),mProportion4Numerator(0),mProportion4Denominator(1),mWrongMove(false), mWrongMovementFrames(0),
			mNewMapFrames(0),mProportionFeedbackFrames13(0),mProportionFeedbackFrames24(0),mProportion1Greater(false),mProportion2Greater(false),mProportion3Greater(false),mProportion4Greater(false)
{
				//TODO: Put all this in the class KillBugView


				mDisplayWidth = mDecoratorManager.GetDisplay().GetWidth();
				mDisplayHeight = mDecoratorManager.GetDisplay().GetHeight();

				mWorkingTriangle = (mDisplayHeight);
				mMapWidth = mDisplayWidth - mWorkingTriangle;
				mMapHeight = mMapWidth;
				mBugPosition.x = 0;
				mBugPosition.y = 0;

				mMapPoint1.x = mWorkingTriangle/2.5;
				mMapPoint1.y = mDisplayHeight/2;
				mMapPoint2.x = mDisplayWidth/2;
				mMapPoint2.y = mWorkingTriangle/2.5 + mDisplayHeight/2 - mDisplayWidth/2;
				mMapPoint3.x = mDisplayWidth-mWorkingTriangle/2.5;
				mMapPoint3.y = mDisplayHeight/2;
				mMapPoint4.x = mDisplayWidth/2;
				mMapPoint4.y = mDisplayHeight/2 - mWorkingTriangle/2.5 + mDisplayWidth/2;

				mProportion1Point.x = (mMapPoint2.x + mMapPoint3.x)/2 + 40.0f;
				mProportion1Point.y = (mMapPoint2.y + mMapPoint3.y)/2 - 40.0f;
				mProportion2Point.x = (mMapPoint2.x + mMapPoint1.x)/2 - 40.0f;
				mProportion2Point.y = (mMapPoint2.y + mMapPoint1.y)/2 - 40.0f;
				mProportion3Point.x = (mMapPoint4.x + mMapPoint1.x)/2 - 40.0f;
				mProportion3Point.y = (mMapPoint4.y + mMapPoint1.y)/2 + 40.0f;
				mProportion4Point.x = (mMapPoint4.x + mMapPoint3.x)/2 + 40.0f;
				mProportion4Point.y = (mMapPoint4.y + mMapPoint3.y)/2 + 40.0f;
}

decorators::KillBugModel::~KillBugModel(){ /*Empty*/}

void decorators::KillBugModel::update(){
	if(IsCartePresent()){
		if(mGameStarted)	FetchProportions();

		if(mFlipper->IsPresent()) DisplayFlipperFeedback();

		checkHintPresent();
	}
}

/*
 * Returns true if one of the maps has been showed, the map doesn't need to be
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
 * Returns true if one of the hints has been showed, it doesn't need to persist
 * (just to have less cards in the table)
 */
bool decorators::KillBugModel::IsHintPresent(){
	return (mActualHint!=NULL);
}

/*
 * Check each hint card and see which one is shown
 */
void decorators::KillBugModel::checkHintPresent(){
	for(int i = 0 ; i < scHintCards ; i++){
		if(mHints[i]->IsPresent()){
			mActualHint = mHints[i];
		}
	}
}

void decorators::KillBugModel::MakeMove(){
	int tNewPositionX;
	int tNewPositionY;

	mProportion1Greater = false;
	mProportion3Greater = false;
	mProportion2Greater = false;
	mProportion4Greater = false;

	// wrongMove is: 1) try to go to an obstacle 2) try to go out the map
	// First check the proportion 2 and 4 (Upper-Left and Bottom-Right)
	float tComparision24 = mProportion2 - mProportion4;

	if(tComparision24 < 0 ){ //if prop 2 < prop 4
		tNewPositionX = (mBugPosition.x + 1 < mActualCarte->getSize()) ? mBugPosition.x + 1 : mBugPosition.x; //New pos is one step to the right, just if is a good move
		mWrongMove = !(mBugPosition.x +1 < mActualCarte->getSize()); // If tries to go out the map, then WrngMove is true
		//mProportion4Greater = (tComparision < 0); //true
		//mProportion2Greater = !mProportion4Greater; //false

	} else if(tComparision24 > 0){ //if prop 2 > prop 4
		tNewPositionX = (mBugPosition.x > 0) ? mBugPosition.x - 1 : mBugPosition.x; //New pos is one step to the left, just if is a good move
		mWrongMove = !(mBugPosition.x > 0); //Try to go out the map
		//mProportion2Greater = (tComparision > 0); //true
		//mProportion4Greater = !mProportion2Greater; //false

	} else { // prop 2 = prop4
		tNewPositionX = mBugPosition.x;
		mWrongMove = false;
		//mProportion2Greater = false;
		//mProportion4Greater = false;
	}

	mProportion4Greater = (tComparision24 < 0);
	mProportion2Greater = (tComparision24 > 0);

	// Now check the proportion 1 and 3
	float tComparision13 = mProportion1 - mProportion3;

	if(tComparision13 < 0){ // prop 1 < prop 3
		if(!mWrongMove){
			tNewPositionY = (mBugPosition.y +1 < mActualCarte->getSize()) ? mBugPosition.y + 1 : mBugPosition.y;
			mWrongMove = !(mBugPosition.y +1 < mActualCarte->getSize()); //Try to go out the map
		}
		//mProportion1Greater = false;
		//mProportion3Greater = true;

	} else if(tComparision13 > 0){ // prop 1 > prop 3
		if(!mWrongMove){
			tNewPositionY = (mBugPosition.y > 0) ?  mBugPosition.y - 1 :  mBugPosition.y;
			mWrongMove = !(mBugPosition.y > 0);
		}
		//mProportion1Greater = true;
		//mProportion3Greater = false;

	} else { // prop 1 == prop 3
		if(!mWrongMove){
			tNewPositionY = mBugPosition.y;
			mWrongMove = false;
		}
		//mProportion1Greater = false;
		//mProportion3Greater = false;
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

	if(mActualCarte->IsEndCell(mBugPosition.x,mBugPosition.y)){
		mActualCarte->FinishMap();
	}

	if(mWrongMove) mWrongMovementFrames = 30;
	if(mProportion2Greater || mProportion4Greater)	mProportionFeedbackFrames24 = 30;
	if(mProportion1Greater || mProportion3Greater)	mProportionFeedbackFrames13 = 30;
	mSteps++;

}

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

}


void decorators::KillBugModel::FetchProportions(){
	int tProportionNumber = 0;

	clearProportions();

	//Check each manipulative
	int tCircularActive = CheckCircularManipulative(tProportionNumber);
	tProportionNumber += tCircularActive;

	int tRectangularActive = CheckRectangularManipulative(tProportionNumber);
	tProportionNumber += tRectangularActive;

	int tTokenActive = CheckTokenManipulative(tProportionNumber);
	tProportionNumber += tTokenActive;

	int tFractionActive = CheckFractionManipulative(tProportionNumber);
	tProportionNumber += tFractionActive;

	saveProportions();
}

void decorators::KillBugModel::clearProportions(){
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

void decorators::KillBugModel::saveProportions(){
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

int decorators::KillBugModel::CheckCircularManipulative(int pProportionNumber){
	int tCircularManipActive = 0;
	if (mCircularModel1->isPresent() && pProportionNumber + tCircularManipActive < 4){
		tCircularManipActive++;
		SetProportionNumber(mCircularModel1->GetCenter(),mCircularModel1->Numerator(), mCircularModel1->Denominator());
	}if (mCircularModel2->isPresent() && pProportionNumber + tCircularManipActive< 4){
		tCircularManipActive++;
		SetProportionNumber(mCircularModel2->GetCenter(),mCircularModel2->Numerator(), mCircularModel2->Denominator());
	}
	return tCircularManipActive;
}
int decorators::KillBugModel::CheckRectangularManipulative(int pProportionNumber){
	int tRectangularManipActive = 0;
	if (mRectangleModel1->isPresent() && pProportionNumber + tRectangularManipActive < 4){
		tRectangularManipActive++;
		SetProportionNumber(mRectangleModel1->getMarker().getCenter(),mRectangleModel1->Numerator(), mRectangleModel1->Denominator());
	}if (mRectangleModel2->isPresent() && pProportionNumber + tRectangularManipActive < 4){
		tRectangularManipActive++;
		SetProportionNumber(mRectangleModel2->getMarker().getCenter(),mRectangleModel2->Numerator(), mRectangleModel2->Denominator());
	}

	return tRectangularManipActive;
}
int decorators::KillBugModel::CheckTokenManipulative(int pProportionNumber){
	//Checking the token manipulative
	int tTokenManipActive = 0;
	if(mTokenModel->isPresent() && pProportionNumber + tTokenManipActive < 4){
		if(!mTokenModel->AreTokensSpread()){
			tTokenManipActive++;
			int tProportion = GetProportionNumber(mTokenModel->GetPosition());
			SetProportionNumber(tProportion,mTokenModel->GetNumerator(tProportion), mTokenModel->GetDenominator(tProportion));
		}else{
			if(pProportionNumber + tTokenManipActive < 4 && mTokenModel->isPresent(1)){
				tTokenManipActive++;
				SetProportionNumber(1,mTokenModel->GetNumerator(1), mTokenModel->GetDenominator(1));
			}if(pProportionNumber + tTokenManipActive < 4 && mTokenModel->isPresent(2)){
				tTokenManipActive++;
				SetProportionNumber(2,mTokenModel->GetNumerator(2), mTokenModel->GetDenominator(2));
			}if(pProportionNumber + tTokenManipActive < 4 && mTokenModel->isPresent(3)){
				tTokenManipActive++;
				SetProportionNumber(3,mTokenModel->GetNumerator(3), mTokenModel->GetDenominator(3));
			}if(pProportionNumber + tTokenManipActive < 4 && mTokenModel->isPresent(4)){
				tTokenManipActive++;
				SetProportionNumber(4,mTokenModel->GetNumerator(4), mTokenModel->GetDenominator(4));
			}
		}
	}

	return tTokenManipActive;
}
int decorators::KillBugModel::CheckFractionManipulative(int tProportionNumber){
	int tFractionManipActive = 0;
	for(int i = 0 ; i < scFractionCards ; i++){
		if(mFractionCards[i]->IsPresent() && tProportionNumber + tFractionManipActive< 4){
			tFractionManipActive++;
			SetProportionNumber(mFractionCards[i]->GetLocation(), mFractionCards[i]->GetNumerator(),mFractionCards[i]->GetDenominator());
		}
	}
	return tFractionManipActive;
}

int decorators::KillBugModel::GetProportionNumber(wykobi::point2d<float> pPosition){
	if(pPosition.x < mDisplayWidth/2){
		if(pPosition.y < mDisplayHeight/2)	return 2;
		else return 3;
	}else{
		if(pPosition.y < mDisplayHeight/2)	return 1;
		else return 4;
	}
}

void decorators::KillBugModel::SetProportionNumber(wykobi::point2d<float> pPosition, float pProportion){
	int tProportionNumber = GetProportionNumber(pPosition);

	switch(tProportionNumber){
		case 1: mProportion1 = pProportion; break;
		case 2: mProportion2 = pProportion; break;
		case 3: mProportion3 = pProportion; break;
		case 4: mProportion4 = pProportion; break;
	}
}

void decorators::KillBugModel:: SetProportionNumber(int pCuadrant, float pProportion){
	switch(pCuadrant){
		case 1: mProportion1 = pProportion; break;
		case 2: mProportion2 = pProportion; break;
		case 3: mProportion3 = pProportion; break;
		case 4: mProportion4 = pProportion; break;
	}
}

void decorators::KillBugModel::SetProportionNumber(int pCuadrant, int pNumerator, int pDenominator){
	switch(pCuadrant){
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
 * Display the feedback in the flipper card
 */
void decorators::KillBugModel::DisplayFlipperFeedback(){
	static const long cShotPreparationTime = 6l*1000l;
	long tElapsedTime = Time::MillisTimestamp() - mLastShot;

	// If the time is more than the animation, then make a move and set the time
	// as the last shot done
	if (mFlipper->IsFlipped() && tElapsedTime > cShotPreparationTime) {
		MakeMove();
		mLastShot = Time::MillisTimestamp();
	}

	// If is present, we calculate the proportion of time that has been
	// past, and we represent it as a degree (to be drawn in the circunference
	if (mFlipper->IsPresent() && mFlipper->GetCurrentSide() != NULL){
		float tPartialDegree = 360*(tElapsedTime/(float)cShotPreparationTime);
		bool tFull = false;
		if(tPartialDegree >= 360)
		{
			tPartialDegree = 360;
			tFull = true;
		}

	// Display the sector of the circunference and then the text
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(*mFlipper->GetCurrentSide(),
		mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
	mDecoratorManager.GetDisplay().RenderFilledSector(1.0f,3.3f,1.5f,1.5f,
		tPartialDegree,0.0f,0.0f,tFull? 1.0 : 0.0f,tFull ? 0.0 : 1.0f,0.8f,1);

	mDecoratorManager.GetDisplay().RenderCenteredText(tFull?"Prêt!" :"En repos ...", 0.5f,5.5f,
		true,0.03f, 0.0f, tFull? 1.0f : 0.0f, tFull? 0.0f : 1.0f, 1.0f);

	mDecoratorManager.GetDisplay().PopTransformation();
	}
}
