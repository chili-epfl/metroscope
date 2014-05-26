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
				(AngleModel *) pDecoratorManager.loadDecorator(pSetting["circular_1"]),
				(AngleModel *) pDecoratorManager.loadDecorator(pSetting["circular_2"]),
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

decorators::KillBugModel::KillBugModel(DecoratorManager &pDecoratorManager, AngleModel *pAngleModel1,
		AngleModel *pAngleModel2, RectangleFractionModel *pRectangleModel1, RectangleFractionModel *pRectangleModel2,
		TokenModel *pTokenModel1, Flipper *pGoFlipper, FractionBugHint ** pFractionHints,
		FractionCard ** pFractionCards, Carte ** pCartes):
			FiducialDecorator(pDecoratorManager, 0),
			mCircularModel1(pAngleModel1), mCircularModel2(pAngleModel2),
			mRectangleModel1(pRectangleModel1), mRectangleModel2(pRectangleModel2),
			mTokenModel1(pTokenModel1), mFlipper(pGoFlipper), mHints(pFractionHints), mFractionCards(pFractionCards), mCartes(pCartes),
			mActualCarte(0), mProportion1(0.0), mProportion2(0.0), mProportion3(0.0), mProportion4(0.0),
			mMapSize(0), mCellDimensionX(0), mCellDimensionY(0), mSteps(0),mGameStarted(false), mActiveManipulatives(0)
{
				mDisplayWidth = mDecoratorManager.GetDisplay().GetWidth();
				mDisplayHeight = mDecoratorManager.GetDisplay().GetHeight();
				mWorkingTriangle = (float)(mDisplayHeight/scTriangleProp);
				mMapHeight = mDisplayHeight - (mWorkingTriangle);
				mMapWidth = mMapHeight; //square
				//mMapWidth = mMapHeight*(mDisplayWidth/(float)mDisplayHeight); //not square
				mBugPosition.x = 0;
				mBugPosition.y = 0;
				mMapPoint1.x = 0;
				mMapPoint1.y = 0;
				mMapPoint2.x = 0;
				mMapPoint2.y = 0;
				mMapPoint3.x = 0;
				mMapPoint3.y = 0;
				mMapPoint4.x = 0;
				mMapPoint4.y = 0;



}

decorators::KillBugModel::~KillBugModel(){ /*Empty*/}

void decorators::KillBugModel::update(){
	if(IsCartePresent()){
		DisplayMap();

		if(mGameStarted){
			FetchProportions();
			DisplayBug();

		}
		if(mActiveManipulatives > 0) DisplayProportions();

		if(mFlipper->IsFlipped())	MakeMove();
	}
}

void decorators::KillBugModel::DisplayProportions(){
	RenderProportion(mProportion1, 1);
	RenderProportion(mProportion2, 2);
	RenderProportion(mProportion3, 3);
	RenderProportion(mProportion4, 4);
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
		tR = 0.0f;
		tG = 0.0f;
		tB = 1.0f;

		break;
	case 2:
		tPosition.x = mMapPoint1.x - (mDisplayWidth-mMapWidth)/4;
		tPosition.y = mDisplayHeight/2;
		tR = 1.0f;
		tG = 0.0f;
		tB = 1.0f;
		break;
	case 3:
		tPosition.x = mDisplayWidth/2;
		tPosition.y = mMapPoint3.y + (mDisplayHeight-mMapHeight)/4;
		tR = 1.0f;
		tG = 0.0f;
		tB = 0.0f;
		break;
	case 4:
		tPosition.x = mMapPoint3.x + (mDisplayWidth-mMapWidth)/4;
		tPosition.y = mDisplayHeight/2;
		tR = 0.0f;
		tG = 1.0f;
		tB = 0.0f;
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
	//float tActualX = (mDisplayWidth/(float)2) - mMapWidth/(float)2;
	//float tActualY = (mDisplayHeight/(float)2) - mMapHeight/(float)2;

	mMapPoint1.x = (mDisplayWidth/(float)2) - mMapWidth/(float)2;
	mMapPoint1.y = (mDisplayHeight/(float)2) - mMapHeight/(float)2;
	mMapPoint2.x = mMapPoint1.x + mMapWidth;
	mMapPoint2.y = mMapPoint1.y;
	mMapPoint3.x = mMapPoint2.x;
	mMapPoint3.y = mMapPoint2.y + mMapHeight;
	mMapPoint4.x = mMapPoint1.x;
	mMapPoint4.y = mMapPoint3.y;

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

void decorators::KillBugModel::MakeMove(){
	int tNewPositionX;
	int tNewPositionY;

	if(mActiveManipulatives == 4){
		if(mProportion1 < mProportion3 && mBugPosition.x +1 < mActualCarte->getSize())	tNewPositionX = mBugPosition.x + 1;
		else if(mProportion1 > mProportion3 && mBugPosition.x > 0) tNewPositionX = mBugPosition.x - 1;

		if(mProportion2 < mProportion4 && mBugPosition.y +1 < mActualCarte->getSize())	tNewPositionY = mBugPosition.y + 1;
		else if(mProportion2 > mProportion4 && mBugPosition.y > 0) tNewPositionY = mBugPosition.y - 1;

		if(mActualCarte->IsEmptyCell(tNewPositionX,tNewPositionY)){
			mBugPosition.x = tNewPositionX;
			mBugPosition.y = tNewPositionY;
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
	/*
	if (mCircularModel1->isPresent() && tProportionNumber < 5){
		tProportionNumber++;
		switch(GetProportionNumber(mCircularModel1->getMarker().getCenter())){
			case 1: mProportion1 = 1.0; break;
			case 2: mProportion2 = 2.0f; break;
			case 3: mProportion3 = 3.0f; break;
			case 4: mProportion4 = 4.0f; break;
		}
	}if (mCircularModel2->isPresent() && tProportionNumber < 4){
		tProportionNumber++;
		switch(GetProportionNumber(mCircularModel2->getMarker().getCenter())){
			case 1: mProportion1 = 1.0; break;
			case 2: mProportion2 = 2.0f; break;
			case 3: mProportion3 = 3.0f; break;
			case 4: mProportion4 = 4.0f; break;
		}
	}

	//Checking the rectangular manipulative
	if (mRectangleModel1->isPresent() && tProportionNumber < 4){
		tProportionNumber++;
		switch(GetProportionNumber(mRectangleModel1->getMarker().getCenter())){
			case 1: mProportion1 = 1.0; break;
			case 2: mProportion2 = 2.0f; break;
			case 3: mProportion3 = 3.0f; break;
			case 4: mProportion4 = 4.0f; break;
		}
	}if (mRectangleModel2->isPresent() && tProportionNumber < 4){
		tProportionNumber++;
		switch(GetProportionNumber(mRectangleModel2->getMarker().getCenter())){
			case 1: mProportion1 = 1.0; break;
			case 2: mProportion2 = 2.0f; break;
			case 3: mProportion3 = 3.0f; break;
			case 4: mProportion4 = 4.0f; break;
		}
	}
*/
	//Checking the token manipulative
	if(mTokenModel1->isPresent() && tProportionNumber < 4){
		mActiveManipulatives++;
		if(!mTokenModel1->AreTokensSpread()){
			tProportionNumber++;
			SetProportionNumber(mTokenModel1->GetPosition(), mTokenModel1->GetProportion());
		}else{

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
	int tProportionNumber;

	if(pPosition.x < mMapWidth/2 + mWorkingTriangle/4){ //1 or 4
		if(pPosition.y < mMapHeight/2 + mWorkingTriangle/4)	tProportionNumber = 1;
		else tProportionNumber = 4;
	}else{ //2 or 3
		if(pPosition.y < mMapHeight/2 + mWorkingTriangle/4)	tProportionNumber = 2;
		else tProportionNumber = 3;
	}

	switch(tProportionNumber){
	case 1: mProportion1 = pProportion; break;
	case 2: mProportion2 = pProportion; break;
	case 3: mProportion3 = pProportion; break;
	case 4: mProportion4 = pProportion; break;
	}
}
