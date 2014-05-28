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
			mMapSize(0), mCellDimensionX(0), mCellDimensionY(0), mSteps(0),mGameStarted(false), mActiveManipulatives(0),mLastShot(Time::MillisTimestamp())
{
				mDisplayWidth = mDecoratorManager.GetDisplay().GetWidth();
				mDisplayHeight = mDecoratorManager.GetDisplay().GetHeight();
				mWorkingTriangle = (float)(mDisplayHeight/scTriangleProp);
				mMapHeight = mDisplayHeight - (mWorkingTriangle);
				mMapWidth = mMapHeight; //square
				mBugPosition.x = 0;
				mBugPosition.y = 0;

				mMapPoint1.x = (mDisplayWidth/(float)2) - mMapWidth/(float)2;
				mMapPoint1.y = (mDisplayHeight/(float)2) - mMapHeight/(float)2;
				mMapPoint2.x = mMapPoint1.x + mMapWidth;
				mMapPoint2.y = mMapPoint1.y;
				mMapPoint3.x = mMapPoint2.x;
				mMapPoint3.y = mMapPoint2.y + mMapHeight;
				mMapPoint4.x = mMapPoint1.x;
				mMapPoint4.y = mMapPoint3.y;

				mProportion1Point.x = mDisplayWidth/2;
				mProportion1Point.y = mMapPoint1.y - (mDisplayHeight-mMapHeight)/4;
				mProportion2Point.x = mMapPoint1.x - (mDisplayWidth-mMapWidth)/4;
				mProportion2Point.y = mDisplayHeight/2;
				mProportion3Point.x = mDisplayWidth/2;
				mProportion3Point.y = mMapPoint3.y + (mDisplayHeight-mMapHeight)/4;
				mProportion4Point.x = mMapPoint3.x + (mDisplayWidth-mMapWidth)/4;
				mProportion4Point.y = mDisplayHeight/2;
}

decorators::KillBugModel::~KillBugModel(){ /*Empty*/}

void decorators::KillBugModel::update(){
	if(IsCartePresent()){
		DisplayMap();

		if(mGameStarted){
			FetchProportions();
			DisplayBug();

		}

		if(mFlipper->IsPresent()) DisplayFlipperFeedback();

		if(IsHintPresent())	DisplayProportions(mActualHint->GetHintType());
	}
}

void decorators::KillBugModel::DisplayProportions(){
	RenderProportion(mProportion1, 1);
	RenderProportion(mProportion2, 2);
	RenderProportion(mProportion3, 3);
	RenderProportion(mProportion4, 4);
}

void decorators::KillBugModel::DisplayProportions(int pType){

	switch(pType){
		case 0: DisplayDiscreteHint(); break;
		case 1: DisplayFractionHint(); break;
		case 2: DisplayDecimalHint();break;
		case 3: DisplayRectangularHint(); break;
		case 4: DisplayCircularHint(); break;
		case 5: DisplayIntegerHint(); break;
	}
}

void decorators::KillBugModel::RenderProportion(float pProportion, int pProportionNumber){
	char tProportion[3];
	sprintf(tProportion, "%3.2f", pProportion);
	wykobi::point2d<float> tPosition;
	float tR, tG, tB;

	switch(pProportionNumber){
	case 1:
		tPosition.x = mDisplayWidth/2;
		tPosition.y = mMapPoint1.y - (mDisplayHeight-mMapHeight)/4;
		tR = 0.0f; tG = 0.0f; tB = 1.0f;

		break;
	case 2:
		tPosition.x = mMapPoint1.x - (mDisplayWidth-mMapWidth)/4;
		tPosition.y = mDisplayHeight/2;
		tR = 1.0f; tG = 0.0f; tB = 1.0f;
		break;
	case 3:
		tPosition.x = mDisplayWidth/2;
		tPosition.y = mMapPoint3.y + (mDisplayHeight-mMapHeight)/4;
		tR = 1.0f; tG = 0.0f; tB = 0.0f;
		break;
	case 4:
		tPosition.x = mMapPoint3.x + (mDisplayWidth-mMapWidth)/4;
		tPosition.y = mDisplayHeight/2;
		tR = 0.0f; tG = 1.0f; tB = 0.0f;
		break;
	}

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderText(tProportion,tPosition.x,tPosition.y,2.0f,tR,tG,tB,1.0f);
	mDecoratorManager.GetDisplay().PopTransformation();

}

void decorators::KillBugModel::DisplayMap(){
	mMapSize = mActualCarte->getSize();
	mCellDimensionX = (float)(mMapWidth/mMapSize);
	mCellDimensionY = (float)(mMapHeight/mMapSize);



	float tActualX = mMapPoint1.x;
	float tActualY = mMapPoint1.y;

	wykobi::point2d<float> tBugOrigin = mActualCarte->getOriginPoint();

	if(!mGameStarted){
		mBugPosition.x = (int)tBugOrigin.x;
		mBugPosition.y = (int)tBugOrigin.y;
	}


	float tBugPositionX = mMapPoint1.x + (tBugOrigin.x)*(mCellDimensionX)+(mCellDimensionX/2);
	float tBugPositionY = mMapPoint1.y + (tBugOrigin.y)*(mCellDimensionY)+(mCellDimensionY/2);

	//Grid
	mDecoratorManager.GetDisplay().PushTransformation();
	for(int i = 0 ; i < mMapSize + 1 ; i++){
		mDecoratorManager.GetDisplay().RenderLine(mMapPoint1.x,tActualY,mMapPoint2.x,tActualY,0.0f,0.0f,0.0f,1.0f);
		mDecoratorManager.GetDisplay().RenderLine(tActualX,mMapPoint1.y,tActualX,mMapPoint4.y, 0.0f,0.0f,0.0f,1.0f);
		tActualX += mCellDimensionX;
		tActualY += mCellDimensionY;
	}

	//TODO Make the diagonals

	//Bug Origin Point
	mDecoratorManager.GetDisplay().RenderFilledEllipse(tBugPositionX, tBugPositionY, mCellDimensionX/6,mCellDimensionY/6,1.0f,0.0f,0.0f,1.0f,1);

	//Ending points
	if(mActualCarte->getEndNumber() > 0){
		std::vector<wykobi::point2d<float>> tEnd = mActualCarte->getEndPoint();

		for(std::vector<wykobi::point2d<float>>::iterator it = tEnd.begin(); it != tEnd.end() ; ++it){
			float tEndPositionX = mMapPoint1.x+ (*it).x*(mCellDimensionX)+(mCellDimensionX/2);
			float tEndPositionY = mMapPoint1.y + (*it).y*(mCellDimensionY)+(mCellDimensionY/2);
			mDecoratorManager.GetDisplay().RenderFilledEllipse(tEndPositionX, tEndPositionY, mCellDimensionX/2,mCellDimensionY/2,0.0f,0.0f,0.0f,1.0f,1);

		}
	}

	//Obstacles points
	if(mActualCarte->getObstacleNumber() > 0){
			std::vector<wykobi::point2d<float>> tObstacle = mActualCarte->getObstaclesPoint();

		for(std::vector<wykobi::point2d<float>>::iterator it = tObstacle.begin(); it != tObstacle.end() ; ++it){
			float tEndPositionX = mMapPoint1.x + (*it).x*(mCellDimensionX);
			float tEndPositionY = mMapPoint1.y + (*it).y*(mCellDimensionY);
			mDecoratorManager.GetDisplay().RenderQuadFilled(tEndPositionX,tEndPositionY,tEndPositionX + mCellDimensionX,tEndPositionY,tEndPositionX+mCellDimensionX, tEndPositionY + mCellDimensionY, tEndPositionX, tEndPositionY + mCellDimensionY,0.25f,0.25f,0.25f,1.0f);
		}
	}
	mDecoratorManager.GetDisplay().PopTransformation();
	mGameStarted = true;
}

bool decorators::KillBugModel::IsCartePresent(){
	Carte * tPreviusCarte = mActualCarte;
	for(int i = 0 ; i < scCarteCards ; i++){
		if(mCartes[i]->isPresent() && !mCartes[i]->IsFinished()){
			mActualCarte = mCartes[i];
			if(tPreviusCarte != mActualCarte)	Start();
			return true;
		}
	}
	return false;
}

bool decorators::KillBugModel::IsHintPresent(){
	for(int i = 0 ; i < scHintCards ; i++){
		if(mHints[i]->IsPresent()){
			mActualHint = mHints[i];
			return true;
		}
	}
	return false;
}

void decorators::KillBugModel::MakeMove(){
	int tNewPositionX;
	int tNewPositionY;

	//TODO: Is this the game logic that we want? --> check
	if(mActiveManipulatives == 4){
		if(mProportion2 < mProportion4) tNewPositionX = (mBugPosition.x +1 < mActualCarte->getSize()) ? mBugPosition.x + 1 : mBugPosition.x;
		else if(mProportion2 > mProportion4) tNewPositionX = (mBugPosition.x > 0) ? mBugPosition.x - 1 : mBugPosition.x;
		else if (mProportion2 == mProportion4) tNewPositionX = mBugPosition.x;

		if(mProportion1 < mProportion3) tNewPositionY = (mBugPosition.y +1 < mActualCarte->getSize()) ? mBugPosition.y + 1 : mBugPosition.y;
		else if(mProportion1 > mProportion3) tNewPositionY = (mBugPosition.y > 0) ?  mBugPosition.y - 1 :  mBugPosition.y;
		else if(mProportion1 == mProportion3) tNewPositionY = mBugPosition.y;

		if(mActualCarte->IsEmptyCell(tNewPositionX,tNewPositionY)){
			mBugPosition.x = tNewPositionX;
			mBugPosition.y = tNewPositionY;
		}
		if(mActualCarte->IsEndCell(mBugPosition.x,mBugPosition.y)){
			mActualCarte->FinishMap();
			//TODO: Something with the feedback
		}
		mSteps++;
	}
}

void decorators::KillBugModel::Start(){
	mGameStarted = false;
	mSteps = 0;

	//Debug :) (To be eliminated)
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderText("NEW MAP",100.0f, mDisplayHeight/2, 5.0f,1.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().PushTransformation();

}

void decorators::KillBugModel::DisplayBug(){
	float tBugPositionX = mMapPoint1.x + (mBugPosition.x)*(mCellDimensionX)+(mCellDimensionX/2);
	float tBugPositionY = mMapPoint1.y + (mBugPosition.y)*(mCellDimensionY)+(mCellDimensionY/2);

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderFilledEllipse(tBugPositionX, tBugPositionY, mCellDimensionX/6,mCellDimensionY/6,0.0f,1.0f,0.0f,1.0f,1);
	mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::KillBugModel::FetchProportions(){
	int tProportionNumber = 0;
	mActiveManipulatives = 0;
	mProportion1 = 0.0f;
	mProportion2 = 0.0f;
	mProportion3 = 0.0f;
	mProportion4 = 0.0f;

	//Checking the circular manipulative
	if (mCircularModel1->isPresent() && tProportionNumber < 4){
		tProportionNumber++;
		mActiveManipulatives++;
		SetProportionNumber(mCircularModel1->GetCenter(), mCircularModel1->Proportion());
	}if (mCircularModel2->isPresent() && tProportionNumber < 4){
		tProportionNumber++;
		mActiveManipulatives++;
		SetProportionNumber(mCircularModel2->GetCenter(), mCircularModel2->Proportion());
	}

	//Checking the rectangular manipulative
	if (mRectangleModel1->isPresent() && tProportionNumber < 4){
		tProportionNumber++;
		mActiveManipulatives++;
		SetProportionNumber(mRectangleModel1->getMarker().getCenter(),mRectangleModel1->proportion());
	}if (mRectangleModel2->isPresent() && tProportionNumber < 4){
		tProportionNumber++;
		mActiveManipulatives++;
		SetProportionNumber(mRectangleModel2->getMarker().getCenter(),mRectangleModel2->proportion());
	}

	//Checking the token manipulative
	if(mTokenModel->isPresent() && tProportionNumber < 4){
		if(!mTokenModel->AreTokensSpread()){
			tProportionNumber++;
			mActiveManipulatives++;
			SetProportionNumber(mTokenModel->GetPosition(), mTokenModel->GetProportion());
		}else{
			if(mTokenModel->GetProportion(1) != 0.0f && tProportionNumber < 4){
				tProportionNumber++;
				mActiveManipulatives++;
				SetProportionNumber(1,mTokenModel->GetProportion(1));
			}if(mTokenModel->GetProportion(2) != 0.0f && tProportionNumber < 4){
				tProportionNumber++;
				mActiveManipulatives++;
				SetProportionNumber(2,mTokenModel->GetProportion(2));
			}if(mTokenModel->GetProportion(3) != 0.0f && tProportionNumber < 4){
				tProportionNumber++;
				mActiveManipulatives++;
				SetProportionNumber(3,mTokenModel->GetProportion(3));
			}if(mTokenModel->GetProportion(4) != 0.0f && tProportionNumber < 4){
				tProportionNumber++;
				mActiveManipulatives++;
				SetProportionNumber(4,mTokenModel->GetProportion(4));
			}
		}
	}

	//Checking the cards
	for(int i = 0 ; i < scFractionCards ; i++){
		if(mFractionCards[i]->IsPresent() && tProportionNumber < 4){
			tProportionNumber++;
			mActiveManipulatives++;
			SetProportionNumber(mFractionCards[i]->GetLocation(), mFractionCards[i]->GetValue());
		}
	}
}

int decorators::KillBugModel::GetProportionNumber(wykobi::point2d<float> pPosition){
	if(pPosition.x < mMapWidth/2){
		if(pPosition.y < mMapHeight/2)	return 2;
		else return 3;
	}else{
		if(pPosition.y < mMapHeight/2)	return 1;
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

void decorators::KillBugModel::DisplayCircularHint(){

	//TODO: Change color and size if needed
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderEllipse(mDisplayWidth/2,mMapPoint1.y - (mDisplayHeight-mMapHeight)/4,mWorkingTriangle/4,mWorkingTriangle/4,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth/2,mMapPoint1.y - (mDisplayHeight-mMapHeight)/4,mWorkingTriangle/4,mWorkingTriangle/4,mProportion1*360,90.0f - mProportion1*360,0.0f,0.0f,0.0f,1.0f,1);

	mDecoratorManager.GetDisplay().RenderEllipse(mMapPoint1.x - (mDisplayWidth-mMapWidth)/4,mDisplayHeight/2,mWorkingTriangle/4,mWorkingTriangle/4,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(mMapPoint1.x - (mDisplayWidth-mMapWidth)/4,mDisplayHeight/2,mWorkingTriangle/4,mWorkingTriangle/4,mProportion2*360,90.0f - mProportion2*360,0.0f,0.0f,0.0f,1.0f,1);

	mDecoratorManager.GetDisplay().RenderEllipse(mDisplayWidth/2,mMapPoint3.y + (mDisplayHeight-mMapHeight)/4,mWorkingTriangle/4,mWorkingTriangle/4,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth/2,mMapPoint3.y + (mDisplayHeight-mMapHeight)/4,mWorkingTriangle/4,mWorkingTriangle/4,mProportion3*360,90.0f - mProportion3*360,0.0f,0.0f,0.0f,1.0f,1);

	mDecoratorManager.GetDisplay().RenderEllipse(mMapPoint3.x + (mDisplayWidth-mMapWidth)/4,mDisplayHeight/2,mWorkingTriangle/4,mWorkingTriangle/4,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(mMapPoint3.x + (mDisplayWidth-mMapWidth)/4,mDisplayHeight/2,mWorkingTriangle/4,mWorkingTriangle/4,mProportion4*360,90.0f - mProportion4*360,0.0f,0.0f,0.0f,1.0f,1);
	mDecoratorManager.GetDisplay().PopTransformation();

}
void decorators::KillBugModel::DisplayRectangularHint(){
	//TODO: Change color and size if needed
	float tRecWidth = mWorkingTriangle/1.5;
	float tRecHeight = tRecWidth*0.55;

	mDecoratorManager.GetDisplay().PushTransformation();

	mDecoratorManager.GetDisplay().RenderQuad(mProportion1Point.x-tRecWidth/2, mProportion1Point.y - tRecHeight/2,
			mProportion1Point.x + tRecWidth/2 , mProportion1Point.y - tRecHeight/2,
			mProportion1Point.x + tRecWidth/2, mProportion1Point.y + tRecHeight/2,
			mProportion1Point.x - tRecWidth/2 , mProportion1Point.y + tRecHeight/2, 0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().RenderQuad(mProportion2Point.x-tRecWidth/2, mProportion2Point.y - tRecHeight/2,
				mProportion2Point.x + tRecWidth/2 , mProportion2Point.y - tRecHeight/2,
				mProportion2Point.x + tRecWidth/2, mProportion2Point.y + tRecHeight/2,
				mProportion2Point.x - tRecWidth/2 , mProportion2Point.y + tRecHeight/2, 0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().RenderQuad(mProportion3Point.x-tRecWidth/2, mProportion3Point.y - tRecHeight/2,
				mProportion3Point.x + tRecWidth/2 , mProportion3Point.y - tRecHeight/2,
				mProportion3Point.x + tRecWidth/2, mProportion3Point.y + tRecHeight/2,
				mProportion3Point.x - tRecWidth/2 , mProportion3Point.y + tRecHeight/2, 0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().RenderQuad(mProportion4Point.x-tRecWidth/2, mProportion4Point.y - tRecHeight/2,
				mProportion4Point.x + tRecWidth/2 , mProportion4Point.y - tRecHeight/2,
				mProportion4Point.x + tRecWidth/2, mProportion4Point.y + tRecHeight/2,
				mProportion4Point.x - tRecWidth/2 , mProportion4Point.y + tRecHeight/2, 0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().RenderQuadFilled(mProportion1Point.x-tRecWidth/2, mProportion1Point.y - tRecHeight/2,
				mProportion1*tRecWidth +  mProportion1Point.x-tRecWidth/2, mProportion1Point.y - tRecHeight/2,
				mProportion1*tRecWidth +  mProportion1Point.x-tRecWidth/2, mProportion1Point.y + tRecHeight/2,
				mProportion1Point.x - tRecWidth/2 , mProportion1Point.y + tRecHeight/2, 0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().RenderQuadFilled(mProportion2Point.x-tRecWidth/2, mProportion2Point.y - tRecHeight/2,
				mProportion2*tRecWidth +mProportion2Point.x-tRecWidth/2 , mProportion2Point.y - tRecHeight/2,
				mProportion2*tRecWidth +mProportion2Point.x-tRecWidth/2, mProportion2Point.y + tRecHeight/2,
				mProportion2Point.x - tRecWidth/2 , mProportion2Point.y + tRecHeight/2, 0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().RenderQuadFilled(mProportion3Point.x-tRecWidth/2, mProportion3Point.y - tRecHeight/2,
				mProportion3*tRecWidth +mProportion3Point.x-tRecWidth/2 , mProportion3Point.y - tRecHeight/2,
				mProportion3*tRecWidth +mProportion3Point.x-tRecWidth/2, mProportion3Point.y + tRecHeight/2,
				mProportion3Point.x - tRecWidth/2 , mProportion3Point.y + tRecHeight/2, 0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().RenderQuadFilled(mProportion4Point.x-tRecWidth/2, mProportion4Point.y - tRecHeight/2,
				mProportion4*tRecWidth +mProportion4Point.x-tRecWidth/2 , mProportion4Point.y - tRecHeight/2,
				mProportion4*tRecWidth +mProportion4Point.x-tRecWidth/2, mProportion4Point.y + tRecHeight/2,
				mProportion4Point.x - tRecWidth/2 , mProportion4Point.y + tRecHeight/2, 0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().PopTransformation();
}
void decorators::KillBugModel::DisplayDiscreteHint(){

}
void decorators::KillBugModel::DisplayDecimalHint(){
	//TODO: Change color and size if needed
	char tProportion1[3];
	char tProportion2[3];
	char tProportion3[3];
	char tProportion4[3];

	sprintf(tProportion1, "%3.2f", mProportion1);
	sprintf(tProportion2, "%3.2f", mProportion2);
	sprintf(tProportion3, "%3.2f", mProportion3);
	sprintf(tProportion4, "%3.2f", mProportion4);

	mDecoratorManager.GetDisplay().PushTransformation();

	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion1,
			mDisplayWidth/2,mMapPoint1.y - (mDisplayHeight-mMapHeight)/4,true,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion2,
			mMapPoint1.x - (mDisplayWidth-mMapWidth)/4,mDisplayHeight/2,true,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion3,
			mDisplayWidth/2,mMapPoint3.y + (mDisplayHeight-mMapHeight)/4,true,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion4,
			mMapPoint3.x + (mDisplayWidth-mMapWidth)/4,mDisplayHeight/2,true,1.0f,0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().PopTransformation();
}
void decorators::KillBugModel::DisplayFractionHint(){
	mDecoratorManager.GetDisplay().PushTransformation();

	mDecoratorManager.GetDisplay().PopTransformation();

}
void decorators::KillBugModel::DisplayIntegerHint(){

}

void decorators::KillBugModel::DisplayFlipperFeedback(){

	static const long cShotPreparationTime = 6l*1000l;
	long tElapsedTime = Time::MillisTimestamp() - mLastShot;

	if (mFlipper->IsFlipped() && tElapsedTime > cShotPreparationTime) {
		MakeMove();
		mLastShot = Time::MillisTimestamp();
	}
	if (mFlipper->IsPresent()){
		float tPartialDegree = 360*(tElapsedTime/(float)cShotPreparationTime);
		bool tFull = false;
		if(tPartialDegree >= 360)
		{
			tPartialDegree = 360;
			tFull = true;
		}
		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(*mFlipper->GetCurrentSide(), mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());

		mDecoratorManager.GetDisplay().RenderFilledSector(0.0f,-0.5f,1.5f,1.5f,tPartialDegree,0.0f,0.0f,tFull? 1.0 : 0.0f,tFull ? 0.0 : 1.0f,0.8f,1);

		mDecoratorManager.GetDisplay().RenderCenteredText(tFull?"Prêt!" :"En charge...", -1.0f, 1.8f,true,0.03f, 0.0f, tFull? 1.0f : 0.0f, tFull? 0.0f : 1.0f, 1.0f);
		mDecoratorManager.GetDisplay().PopTransformation();
		}
}
