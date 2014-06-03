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
			mNewMapFrames(0)
{
				mDisplayWidth = mDecoratorManager.GetDisplay().GetWidth();
				mDisplayHeight = mDecoratorManager.GetDisplay().GetHeight();
				mWorkingTriangle = (mDisplayHeight/(float)2);
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

				mProportion1Point.x = mDisplayWidth/2 + mWorkingTriangle/6 ;
				mProportion1Point.y = mMapPoint1.y - mWorkingTriangle/6;
				mProportion2Point.x = mMapPoint1.x - mWorkingTriangle/6;
				mProportion2Point.y = mDisplayHeight/2 - mWorkingTriangle/6 ;
				mProportion3Point.x = mDisplayWidth/2 - mWorkingTriangle/6;
				mProportion3Point.y = mMapPoint3.y + mWorkingTriangle/6;
				mProportion4Point.x = mMapPoint3.x + mWorkingTriangle/6;
				mProportion4Point.y = mDisplayHeight/2 + mWorkingTriangle/6;
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
	}
}

void decorators::KillBugModel::RenderProportion(float pProportion, int pProportionNumber){
	char tProportion[3];
	sprintf(tProportion, "%3.2f", pProportion);
	wykobi::point2d<float> tPosition;
	float tR, tG, tB;

	switch(pProportionNumber){
	case 1:
		//tPosition.x = mDisplayWidth/2;
		//tPosition.y = mMapPoint1.y - (mDisplayHeight-mMapHeight)/4;

		tPosition.x = mProportion1Point.x;
		tPosition.y = mProportion1Point.y;
		tR = 0.0f; tG = 0.0f; tB = 1.0f;

		break;
	case 2:
		//tPosition.x = mMapPoint1.x - (mDisplayWidth-mMapWidth)/4;
		//tPosition.y = mDisplayHeight/2;

		tPosition.x = mProportion2Point.x;
		tPosition.y = mProportion2Point.y;

		tR = 1.0f; tG = 0.0f; tB = 1.0f;
		break;
	case 3:
		tPosition.x = mProportion3Point.x;
		tPosition.y = mProportion3Point.y;
		//tPosition.x = mDisplayWidth/2;
		//tPosition.y = mMapPoint3.y + (mDisplayHeight-mMapHeight)/4;
		tR = 1.0f; tG = 0.0f; tB = 0.0f;
		break;
	case 4:
		tPosition.x = mProportion4Point.x;
		tPosition.y = mProportion4Point.y;
		//tPosition.x = mMapPoint3.x + (mDisplayWidth-mMapWidth)/4;
		//tPosition.y = mDisplayHeight/2;
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

	mDecoratorManager.GetDisplay().RenderQuadFilled(mMapPoint1.x, mMapPoint1.y, mMapPoint2.x, mMapPoint2.y,
			mMapPoint3.x, mMapPoint3.y, mMapPoint4.x, mMapPoint4.y, 1.0f,1.0f,1.0f,0.0f);
	/*for(int i = 0 ; i < mMapSize + 1 ; i++){
		mDecoratorManager.GetDisplay().RenderLine(mMapPoint1.x,tActualY,mMapPoint2.x,tActualY,0.0f,0.0f,0.0f,1.0f);
		mDecoratorManager.GetDisplay().RenderLine(tActualX,mMapPoint1.y,tActualX,mMapPoint4.y, 0.0f,0.0f,0.0f,1.0f);
		tActualX += mCellDimensionX;
		tActualY += mCellDimensionY;
	}*/

	//TODO Make the diagonals
	mDecoratorManager.GetDisplay().RenderQuadFilled(0.0f,0.0f,mDisplayWidth/2,0.0f,
			mDisplayWidth/2,mWorkingTriangle/2, 0.0f,mWorkingTriangle/2, scProp2R, scProp2G, scProp2B, 0.2);//Prop2, box1
	mDecoratorManager.GetDisplay().RenderQuadFilled(0.0f,mWorkingTriangle/2,mMapPoint1.x,mWorkingTriangle/2,
			mMapPoint1.x,mDisplayHeight/2, 0.0f,mDisplayHeight/2, scProp2R, scProp2G, scProp2B, 0.2);//Prop2, box2

	mDecoratorManager.GetDisplay().RenderQuadFilled(0.0f,mDisplayHeight/2, mMapPoint1.x,mDisplayHeight/2,
			mMapPoint4.x, mMapPoint4.y, 0.0f, mMapPoint4.y, scProp3R, scProp3G, scProp3B, 0.2); //P3b3
	mDecoratorManager.GetDisplay().RenderQuadFilled(0.0f, mMapPoint4.y, mDisplayWidth/2, mMapPoint4.y,
			mDisplayWidth/2,mDisplayHeight, 0.0f, mDisplayHeight , scProp3R, scProp3G, scProp3B, 0.2); //P3b4

	mDecoratorManager.GetDisplay().RenderQuadFilled(mDisplayWidth/2, mMapPoint4.y, mDisplayWidth, mMapPoint4.y,
			mDisplayWidth, mDisplayHeight, mDisplayWidth/2, mDisplayHeight, scProp4R, scProp4G, scProp4B, 0.2);//P4b5
	mDecoratorManager.GetDisplay().RenderQuadFilled(mMapPoint3.x, mDisplayHeight/2, mDisplayWidth, mDisplayHeight/2,
			mDisplayWidth, mMapPoint3.y, mMapPoint3.x, mMapPoint3.y, scProp4R, scProp4G, scProp4B, 0.2);

	mDecoratorManager.GetDisplay().RenderQuadFilled(mMapPoint2.x, mMapPoint2.y, mDisplayWidth, mMapPoint2.y,
			mDisplayWidth, mDisplayHeight/2, mMapPoint2.x, mDisplayHeight/2,scProp1R, scProp1G, scProp1B, 0.2);
	mDecoratorManager.GetDisplay().RenderQuadFilled(mDisplayWidth/2, 0.0f, mDisplayWidth, 0.0f,
			mDisplayWidth, mWorkingTriangle/2,mDisplayWidth/2, mWorkingTriangle/2, scProp1R, scProp1G, scProp1B, 0.2);


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
	/*if(mActualCarte->getObstacleNumber() > 0){
			std::vector<wykobi::point2d<float>> tObstacle = mActualCarte->getObstaclesPoint();

		for(std::vector<wykobi::point2d<float>>::iterator it = tObstacle.begin(); it != tObstacle.end() ; ++it){
			float tEndPositionX = mMapPoint1.x + (*it).x*(mCellDimensionX);
			float tEndPositionY = mMapPoint1.y + (*it).y*(mCellDimensionY);
			mDecoratorManager.GetDisplay().RenderQuadFilled(tEndPositionX,tEndPositionY,tEndPositionX + mCellDimensionX,tEndPositionY,tEndPositionX+mCellDimensionX, tEndPositionY + mCellDimensionY, tEndPositionX, tEndPositionY + mCellDimensionY,0.25f,0.25f,0.25f,1.0f);
		}
	}*/

	int tObstacleTextureId = mDecoratorManager.GetDisplay().LoadTexture("./activities/proportions-network/obstacle-small.jpg");
	std::vector<wykobi::point2d<float>> tObstacle = mActualCarte->getObstaclesPoint();


	for(std::vector<wykobi::point2d<float>>::iterator it = tObstacle.begin(); it != tObstacle.end() ; ++it){
		mDecoratorManager.GetDisplay().RenderTexture(tObstacleTextureId,
				mMapPoint1.x + (*it).x*mCellDimensionX,mMapPoint1.y + (*it).y*mCellDimensionY,
				mMapPoint1.x + (*it).x*mCellDimensionX + mCellDimensionX,mMapPoint1.y + (*it).y*mCellDimensionY,
				mMapPoint1.x + (*it).x*mCellDimensionX + mCellDimensionX,mMapPoint1.y + (*it).y*mCellDimensionY + mCellDimensionY,
				mMapPoint1.x + (*it).x*mCellDimensionX,mMapPoint1.y + (*it).y*mCellDimensionY + mCellDimensionY);
	}


	if(mMapFinished){
		mDecoratorManager.GetDisplay().RenderQuadFilled(mMapPoint1.x, mDisplayHeight/2 - mWorkingTriangle/4,
				mMapPoint2.x, mDisplayHeight/2 - mWorkingTriangle/4, mMapPoint2.x, mDisplayHeight/2 + mWorkingTriangle/4,
				mMapPoint1.x, mDisplayHeight/2 + mWorkingTriangle/4, 0.8f,0.8f,0.8f,0.5f);
		mDecoratorManager.GetDisplay().RenderText("Carte finie!", mMapPoint1.x + 60.0f,  mDisplayHeight/2, 2.0f,0.0f,0.0f,0.0f,1.0f);
	}

	if(mWrongMovementFrames>0){
		mDecoratorManager.GetDisplay().RenderText("Je ne peux pas y aller...", mBugPosition.x*mCellDimensionX + mMapPoint1.x + 10.0f,
				mBugPosition.y*mCellDimensionY + mMapPoint1.y - 10.0f, 0.95f,0.0f,0.0f,0.0f,1.0f);
		mWrongMovementFrames--;
	}
	if(mNewMapFrames>0){
		mDecoratorManager.GetDisplay().RenderQuadFilled(mMapPoint1.x, mDisplayHeight/2 - mWorkingTriangle/4,
						mMapPoint2.x, mDisplayHeight/2 - mWorkingTriangle/4, mMapPoint2.x, mDisplayHeight/2 + mWorkingTriangle/4,
						mMapPoint1.x, mDisplayHeight/2 + mWorkingTriangle/4, 0.8f,0.8f,0.8f,0.5f);
		mDecoratorManager.GetDisplay().RenderText("Carte nouvelle!", mMapPoint1.x + 50.0f, mDisplayHeight/2, 2.0f,0.0f,0.0f,0.0f,1.0f);
		mNewMapFrames--;
	}

	mDecoratorManager.GetDisplay().PopTransformation();
	mGameStarted = true;
}

bool decorators::KillBugModel::IsCartePresent(){
	Carte * tPreviusCarte = mActualCarte;
	for(int i = 0 ; i < scCarteCards ; i++){
		//if(mCartes[i]->isPresent() && !mCartes[i]->IsFinished()){
		if(mCartes[i]->isPresent()){
			mActualCarte = mCartes[i];
			mMapFinished = mActualCarte->IsFinished();
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

	//bool wrongMove is: 1) try to go to an obstacle 2) try to go out the map
	if(mActiveManipulatives == 4){
		if(mProportion2 < mProportion4){
			tNewPositionX = (mBugPosition.x +1 < mActualCarte->getSize()) ? mBugPosition.x + 1 : mBugPosition.x;
			mWrongMove = !(mBugPosition.x +1 < mActualCarte->getSize()); //Try to go out the map
		} else if(mProportion2 > mProportion4){
			tNewPositionX = (mBugPosition.x > 0) ? mBugPosition.x - 1 : mBugPosition.x;
			mWrongMove = !(mBugPosition.x > 0); //Try to go out the map
		} else if (mProportion2 == mProportion4){
			tNewPositionX = mBugPosition.x;
			mWrongMove = false;
		}

		if(mProportion1 < mProportion3 && !mWrongMove){
			tNewPositionY = (mBugPosition.y +1 < mActualCarte->getSize()) ? mBugPosition.y + 1 : mBugPosition.y;
			mWrongMove = !(mBugPosition.y +1 < mActualCarte->getSize()); //Try to go out the map
		} else if(mProportion1 > mProportion3 && !mWrongMove){
			tNewPositionY = (mBugPosition.y > 0) ?  mBugPosition.y - 1 :  mBugPosition.y;
			mWrongMove = !(mBugPosition.y > 0);
		} else if(mProportion1 == mProportion3 && !mWrongMove){
			tNewPositionY = mBugPosition.y;
			mWrongMove = false;
		}

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
			//mMapFinished;
			//TODO: Something with the feedback
		}

		if(mWrongMove) mWrongMovementFrames = 30;
		mSteps++;
	}
}

void decorators::KillBugModel::Start(){
	mGameStarted = false;
	mSteps = 0;
	mNewMapFrames = 30;
}

void decorators::KillBugModel::DisplayBug(){
	float tBugPositionX = mMapPoint1.x + (mBugPosition.x)*(mCellDimensionX)+(mCellDimensionX/2);
	float tBugPositionY = mMapPoint1.y + (mBugPosition.y)*(mCellDimensionY)+(mCellDimensionY/2);

	int tTextureId = mDecoratorManager.GetDisplay().LoadTexture("./activities/proportions-network/ladybug-smallwhite.jpg");

	//Display the image
	mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().RenderTexture(tTextureId,
				mMapPoint1.x + mBugPosition.x*mCellDimensionX,mMapPoint1.y + mBugPosition.y*mCellDimensionY,
				mMapPoint1.x + mBugPosition.x*mCellDimensionX + mCellDimensionX,mMapPoint1.y + mBugPosition.y*mCellDimensionY,
				mMapPoint1.x + mBugPosition.x*mCellDimensionX + mCellDimensionX,mMapPoint1.y + mBugPosition.y*mCellDimensionY + mCellDimensionY,
				mMapPoint1.x + mBugPosition.x*mCellDimensionX,mMapPoint1.y + mBugPosition.y*mCellDimensionY + mCellDimensionY);
	mDecoratorManager.GetDisplay().PopTransformation();

	//mDecoratorManager.GetDisplay().PushTransformation();
	//mDecoratorManager.GetDisplay().RenderFilledEllipse(tBugPositionX, tBugPositionY, mCellDimensionX/6,mCellDimensionY/6,0.0f,1.0f,0.0f,1.0f,1);
	//mDecoratorManager.GetDisplay().PopTransformation();
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
		SetProportionNumber(mCircularModel1->GetCenter(),mCircularModel1->Numerator(), mCircularModel1->Denominator());
		//SetProportionNumber(mCircularModel1->GetCenter(), mCircularModel1->Proportion());
	}if (mCircularModel2->isPresent() && tProportionNumber < 4){
		tProportionNumber++;
		mActiveManipulatives++;
		SetProportionNumber(mCircularModel2->GetCenter(),mCircularModel2->Numerator(), mCircularModel2->Denominator());
		//SetProportionNumber(mCircularModel2->GetCenter(), mCircularModel2->Proportion());
		//SetProportionNumber(mCircularModel2->GetCenter(),mCircularModel2->Numerator(), mCircularModel2->Denominator());
	}

	//Checking the rectangular manipulative
	if (mRectangleModel1->isPresent() && tProportionNumber < 4){
		tProportionNumber++;
		mActiveManipulatives++;
		//SetProportionNumber(mRectangleModel1->getMarker().getCenter(),mRectangleModel1->proportion());
		SetProportionNumber(mRectangleModel1->getMarker().getCenter(),mRectangleModel1->Numerator(), mRectangleModel1->Denominator());
	}if (mRectangleModel2->isPresent() && tProportionNumber < 4){
		tProportionNumber++;
		mActiveManipulatives++;
		//SetProportionNumber(mRectangleModel2->getMarker().getCenter(),mRectangleModel2->proportion());
		SetProportionNumber(mRectangleModel2->getMarker().getCenter(),mRectangleModel2->Numerator(), mRectangleModel2->Denominator());
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
				//SetProportionNumber(1,mTokenModel->GetProportion(1));
				SetProportionNumber(1,mTokenModel->GetNumerator(1), mTokenModel->GetDenominator(1));
			}if(mTokenModel->GetProportion(2) != 0.0f && tProportionNumber < 4){
				tProportionNumber++;
				mActiveManipulatives++;
				//SetProportionNumber(2,mTokenModel->GetProportion(2));
				SetProportionNumber(2,mTokenModel->GetNumerator(2), mTokenModel->GetDenominator(2));
			}if(mTokenModel->GetProportion(3) != 0.0f && tProportionNumber < 4){
				tProportionNumber++;
				mActiveManipulatives++;
				//SetProportionNumber(3,mTokenModel->GetProportion(3));
				SetProportionNumber(3,mTokenModel->GetNumerator(3), mTokenModel->GetDenominator(3));
			}if(mTokenModel->GetProportion(4) != 0.0f && tProportionNumber < 4){
				tProportionNumber++;
				mActiveManipulatives++;
				//SetProportionNumber(4,mTokenModel->GetProportion(4));
				SetProportionNumber(4,mTokenModel->GetNumerator(4), mTokenModel->GetDenominator(4));
			}
		}
	}

	//Checking the cards
	for(int i = 0 ; i < scFractionCards ; i++){
		if(mFractionCards[i]->IsPresent() && tProportionNumber < 4){
			tProportionNumber++;
			mActiveManipulatives++;
			//SetProportionNumber(mFractionCards[i]->GetLocation(), mFractionCards[i]->GetValue());
			SetProportionNumber(mFractionCards[i]->GetLocation(), mFractionCards[i]->GetNumerator(),mFractionCards[i]->GetDenominator());
		}
	}
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
			mProportion1Numerator = pNumerator;
			mProportion1Denominator = pDenominator;
			SetProportionNumber(1,(pNumerator/(float)pDenominator));
			break;
		case 2:
			mProportion2Numerator = pNumerator;
			mProportion2Denominator = pDenominator;
			SetProportionNumber(2,(pNumerator/(float)pDenominator));
			break;
		case 3:
			mProportion3Numerator = pNumerator;
			mProportion3Denominator = pDenominator;
			SetProportionNumber(3,(pNumerator/(float)pDenominator));
			break;
		case 4:
			mProportion4Numerator = pNumerator;
			mProportion4Denominator = pDenominator;
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

void decorators::KillBugModel::DisplayCircularHint(){

	//TODO: Change color and size if needed
	mDecoratorManager.GetDisplay().PushTransformation();

	mDecoratorManager.GetDisplay().RenderEllipse(mProportion1Point.x,mProportion1Point.y,mWorkingTriangle/8,mWorkingTriangle/8,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(mProportion1Point.x,mProportion1Point.y,mWorkingTriangle/8,mWorkingTriangle/8,mProportion1*360,90.0f - mProportion1*360,scProp1R,scProp1G,scProp1B,1.0f,1);

	mDecoratorManager.GetDisplay().RenderEllipse(mProportion2Point.x,mProportion2Point.y,mWorkingTriangle/8,mWorkingTriangle/8,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(mProportion2Point.x,mProportion2Point.y,mWorkingTriangle/8,mWorkingTriangle/8,mProportion2*360,90.0f - mProportion2*360,scProp2R,scProp2G,scProp2B,1.0f,1);

	mDecoratorManager.GetDisplay().RenderEllipse(mProportion3Point.x,mProportion3Point.y,mWorkingTriangle/8,mWorkingTriangle/8,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(mProportion3Point.x,mProportion3Point.y,mWorkingTriangle/8,mWorkingTriangle/8,mProportion3*360,90.0f - mProportion3*360,scProp3R,scProp3G,scProp3B,1.0f,1);

	mDecoratorManager.GetDisplay().RenderEllipse(mProportion4Point.x,mProportion4Point.y,mWorkingTriangle/8,mWorkingTriangle/8,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(mProportion4Point.x,mProportion4Point.y,mWorkingTriangle/8,mWorkingTriangle/8,mProportion4*360,90.0f - mProportion4*360,scProp4R,scProp4G,scProp4B,1.0f,1);

	mDecoratorManager.GetDisplay().PopTransformation();

}
void decorators::KillBugModel::DisplayRectangularHint(){
	//TODO: Change color and size if needed
	float tRecWidth = mWorkingTriangle*0.25;
	float tRecHeight = tRecWidth/2;

	mDecoratorManager.GetDisplay().PushTransformation();

	mDecoratorManager.GetDisplay().RenderQuad(mProportion1Point.x-tRecWidth/2, mProportion1Point.y - tRecHeight/2,
			mProportion1Point.x + tRecWidth/2 , mProportion1Point.y - tRecHeight/2,
			mProportion1Point.x + tRecWidth/2, mProportion1Point.y + tRecHeight/2,
			mProportion1Point.x - tRecWidth/2 , mProportion1Point.y + tRecHeight/2, scProp1R,scProp1G,scProp1B,1.0f);

	mDecoratorManager.GetDisplay().RenderQuad(mProportion2Point.x-tRecWidth/2, mProportion2Point.y - tRecHeight/2,
				mProportion2Point.x + tRecWidth/2 , mProportion2Point.y - tRecHeight/2,
				mProportion2Point.x + tRecWidth/2, mProportion2Point.y + tRecHeight/2,
				mProportion2Point.x - tRecWidth/2 , mProportion2Point.y + tRecHeight/2, scProp2R,scProp2G,scProp2B,1.0f);

	mDecoratorManager.GetDisplay().RenderQuad(mProportion3Point.x-tRecWidth/2, mProportion3Point.y - tRecHeight/2,
				mProportion3Point.x + tRecWidth/2 , mProportion3Point.y - tRecHeight/2,
				mProportion3Point.x + tRecWidth/2, mProportion3Point.y + tRecHeight/2,
				mProportion3Point.x - tRecWidth/2 , mProportion3Point.y + tRecHeight/2, scProp3R,scProp3G,scProp3B,1.0f);

	mDecoratorManager.GetDisplay().RenderQuad(mProportion4Point.x-tRecWidth/2, mProportion4Point.y - tRecHeight/2,
				mProportion4Point.x + tRecWidth/2 , mProportion4Point.y - tRecHeight/2,
				mProportion4Point.x + tRecWidth/2, mProportion4Point.y + tRecHeight/2,
				mProportion4Point.x - tRecWidth/2 , mProportion4Point.y + tRecHeight/2, scProp4R,scProp4G,scProp4B,1.0f);

	mDecoratorManager.GetDisplay().RenderQuadFilled(mProportion1Point.x-tRecWidth/2, mProportion1Point.y - tRecHeight/2,
				mProportion1*tRecWidth +  mProportion1Point.x-tRecWidth/2, mProportion1Point.y - tRecHeight/2,
				mProportion1*tRecWidth +  mProportion1Point.x-tRecWidth/2, mProportion1Point.y + tRecHeight/2,
				mProportion1Point.x - tRecWidth/2 , mProportion1Point.y + tRecHeight/2, scProp1R,scProp1G,scProp1B,1.0f);

	mDecoratorManager.GetDisplay().RenderQuadFilled(mProportion2Point.x-tRecWidth/2, mProportion2Point.y - tRecHeight/2,
				mProportion2*tRecWidth +mProportion2Point.x-tRecWidth/2 , mProportion2Point.y - tRecHeight/2,
				mProportion2*tRecWidth +mProportion2Point.x-tRecWidth/2, mProportion2Point.y + tRecHeight/2,
				mProportion2Point.x - tRecWidth/2 , mProportion2Point.y + tRecHeight/2, scProp2R,scProp2G,scProp2B,1.0f);

	mDecoratorManager.GetDisplay().RenderQuadFilled(mProportion3Point.x-tRecWidth/2, mProportion3Point.y - tRecHeight/2,
				mProportion3*tRecWidth +mProportion3Point.x-tRecWidth/2 , mProportion3Point.y - tRecHeight/2,
				mProportion3*tRecWidth +mProportion3Point.x-tRecWidth/2, mProportion3Point.y + tRecHeight/2,
				mProportion3Point.x - tRecWidth/2 , mProportion3Point.y + tRecHeight/2, scProp3R,scProp3G,scProp3B,1.0f);

	mDecoratorManager.GetDisplay().RenderQuadFilled(mProportion4Point.x-tRecWidth/2, mProportion4Point.y - tRecHeight/2,
				mProportion4*tRecWidth +mProportion4Point.x-tRecWidth/2 , mProportion4Point.y - tRecHeight/2,
				mProportion4*tRecWidth +mProportion4Point.x-tRecWidth/2, mProportion4Point.y + tRecHeight/2,
				mProportion4Point.x - tRecWidth/2 , mProportion4Point.y + tRecHeight/2, scProp4R,scProp4G,scProp4B,1.0f);

	mDecoratorManager.GetDisplay().PopTransformation();
}
void decorators::KillBugModel::DisplayDiscreteHint(){
	float tBoxWidth = mWorkingTriangle*0.25;
	float tBoxHeight = tBoxWidth/2;

	float tCellWidth = tBoxWidth/9;
	float tCellHeight = tBoxHeight/7;

	mDecoratorManager.GetDisplay().PushTransformation();

	float tPartialX = mProportion1Point.x - tBoxWidth/2;
	float tPartialY = mProportion1Point.y - tBoxHeight/2;
	int tDenom = mProportion1Denominator;
	int tNum = mProportion1Numerator;

	for(int i = 0 ; i < 4 ; i++){
		tPartialX = mProportion1Point.x - tBoxWidth/2;
		for(int j = 0 ; j < 5 ; j++){
			if(tDenom > 0) {
				mDecoratorManager.GetDisplay().RenderQuadFilled(tPartialX,tPartialY,tPartialX+tCellWidth,tPartialY,
						tPartialX+tCellWidth,tPartialY+tCellHeight,tPartialX,tPartialY+tCellHeight,
						tNum > 0 ? 1.0f : 0.0f,tNum > 0 ? 1.0f : 0.0f,tNum > 0 ? 0.0f : 1.0f,1.0f);
				tPartialX += 2*tCellWidth;
				tDenom--;
				tNum--;
			}
		}
		tPartialY += 2*tCellHeight;
	}

	tPartialX = mProportion2Point.x - tBoxWidth/2;
	tPartialY = mProportion2Point.y - tBoxHeight/2;
	tDenom = mProportion2Denominator;
	tNum = mProportion2Numerator;

		for(int i = 0 ; i < 4 ; i++){
			tPartialX = mProportion2Point.x - tBoxWidth/2;
			for(int j = 0 ; j < 5 ; j++){
				if(tDenom > 0) {
					mDecoratorManager.GetDisplay().RenderQuadFilled(tPartialX,tPartialY,tPartialX+tCellWidth,tPartialY,
							tPartialX+tCellWidth,tPartialY+tCellHeight,tPartialX,tPartialY+tCellHeight,
							tNum > 0 ? 1.0f : 0.0f,tNum > 0 ? 1.0f : 0.0f,tNum > 0 ? 0.0f : 1.0f,1.0f);
					tPartialX += 2*tCellWidth;
					tDenom--;
					tNum--;
				}
			}
			tPartialY += 2*tCellHeight;
		}

	tPartialX = mProportion3Point.x - tBoxWidth/2;
	tPartialY = mProportion3Point.y - tBoxHeight/2;
	tDenom = mProportion3Denominator;
	tNum = mProportion3Numerator;

			for(int i = 0 ; i < 4 ; i++){
				tPartialX = mProportion3Point.x - tBoxWidth/2;
				for(int j = 0 ; j < 5 ; j++){
					if(tDenom > 0) {
						mDecoratorManager.GetDisplay().RenderQuadFilled(tPartialX,tPartialY,tPartialX+tCellWidth,tPartialY,
								tPartialX+tCellWidth,tPartialY+tCellHeight,tPartialX,tPartialY+tCellHeight,
								tNum > 0 ? 1.0f : 0.0f,tNum > 0 ? 1.0f : 0.0f,tNum > 0 ? 0.0f : 1.0f,1.0f);
						tPartialX += 2*tCellWidth;
						tDenom--;
						tNum--;
					}
				}
				tPartialY += 2*tCellHeight;
			}

	tPartialX = mProportion4Point.x - tBoxWidth/2;
	tPartialY = mProportion4Point.y - tBoxHeight/2;
	tDenom = mProportion4Denominator;
	tNum = mProportion4Numerator;

				for(int i = 0 ; i < 4 ; i++){
					tPartialX = mProportion4Point.x - tBoxWidth/2;
					for(int j = 0 ; j < 5 ; j++){
						if(tDenom > 0) {
							mDecoratorManager.GetDisplay().RenderQuadFilled(tPartialX,tPartialY,tPartialX+tCellWidth,tPartialY,
									tPartialX+tCellWidth,tPartialY+tCellHeight,tPartialX,tPartialY+tCellHeight,
									tNum > 0 ? 1.0f : 0.0f,tNum > 0 ? 1.0f : 0.0f,tNum > 0 ? 0.0f : 1.0f,1.0f);
							tPartialX += 2*tCellWidth;
							tDenom--;
							tNum--;
						}
					}
					tPartialY += 2*tCellHeight;
				}

	mDecoratorManager.GetDisplay().PopTransformation();

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
			mProportion1Point.x,mProportion1Point.y,true,2.0f,scProp1R,scProp1G,scProp1B,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion2,
			mProportion2Point.x,mProportion2Point.y,true,2.0f,scProp2R,scProp2G,scProp2B,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion3,
			mProportion3Point.x,mProportion3Point.y,true,2.0f,scProp3R,scProp3G,scProp3B,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion4,
			mProportion4Point.x,mProportion4Point.y,true,2.0f,scProp4R,scProp4G,scProp4B,1.0f);

	mDecoratorManager.GetDisplay().PopTransformation();
}
void decorators::KillBugModel::DisplayFractionHint(){
	char tProportion1Numerator[3];
	char tProportion2Numerator[3];
	char tProportion3Numerator[3];
	char tProportion4Numerator[3];
	char tProportion1Denominator[3];
	char tProportion2Denominator[3];
	char tProportion3Denominator[3];
	char tProportion4Denominator[3];

	sprintf(tProportion1Numerator, "%d", mProportion1Numerator);
	sprintf(tProportion2Numerator, "%d", mProportion2Numerator);
	sprintf(tProportion3Numerator, "%d", mProportion3Numerator);
	sprintf(tProportion4Numerator, "%d", mProportion4Numerator);
	sprintf(tProportion1Denominator, "%d",mProportion1Denominator);
	sprintf(tProportion2Denominator, "%d", mProportion2Denominator);
	sprintf(tProportion3Denominator, "%d", mProportion3Denominator);
	sprintf(tProportion4Denominator, "%d", mProportion4Denominator);

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion1Numerator,mProportion1Point.x,mProportion1Point.y - 15.0f,true,1.0f,scProp1R,scProp1G,scProp1B,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion1Denominator,mProportion1Point.x,mProportion1Point.y + 15.0f,true,1.0f,scProp1R,scProp1G,scProp1B,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion2Numerator,mProportion2Point.x,mProportion2Point.y - 15.0f,true,1.0f,scProp2R,scProp2G,scProp2B,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion2Denominator,mProportion2Point.x,mProportion2Point.y + 15.0f,true,1.0f,scProp2R,scProp2G,scProp2B,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion3Numerator,mProportion3Point.x,mProportion3Point.y - 15.0f,true,1.0f,scProp3R,scProp3G,scProp3B,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion3Denominator,mProportion3Point.x,mProportion3Point.y + 15.0f,true,1.0f,scProp3R,scProp3G,scProp3B,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion4Numerator,mProportion4Point.x,mProportion4Point.y - 15.0f,true,1.0f,scProp4R,scProp4G,scProp4B,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion4Denominator,mProportion4Point.x,mProportion4Point.y + 15.0f,true,1.0f,scProp4R,scProp4G,scProp4B,1.0f);


	mDecoratorManager.GetDisplay().RenderLine(mProportion1Point.x-15,mProportion1Point.y,mProportion1Point.x+15,mProportion1Point.y,scProp1R,scProp1G,scProp1B,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(mProportion2Point.x-15,mProportion2Point.y,mProportion2Point.x+15,mProportion2Point.y,scProp2R,scProp2G,scProp2B,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(mProportion3Point.x-15,mProportion3Point.y,mProportion3Point.x+15,mProportion3Point.y,scProp3R,scProp3G,scProp3B,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(mProportion4Point.x-15,mProportion4Point.y,mProportion4Point.x+15,mProportion4Point.y,scProp4R,scProp4G,scProp4B,1.0f);
	mDecoratorManager.GetDisplay().PopTransformation();

	//if(mCircularModel1->isPresent() || mCircularModel2->isPresent())	DivideCircunferenceManipulatives(10);
	//if(mRectangleModel1->isPresent() || mRectangleModel2->isPresent())	DivideRectangleManipulatives(10);
}

void decorators::KillBugModel::DivideRectangleManipulatives(int pParts){
	float tDistanceX = 8.0f;
	float tDistanceY = 4.0f;
	double tRectangleLength = (double)(tDistanceX/pParts);
	double tPartialX = tRectangleLength;

	if(mRectangleModel1->isPresent()){
		for(int i = 0 ; i < pParts ; i++){

			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(mRectangleModel1->getMarker(),mDecoratorManager.GetCam2World(),mDecoratorManager.GetWorld2Proj());

			tPartialX += tRectangleLength;

			mDecoratorManager.GetDisplay().RenderQuad(-tDistanceX/2,-tDistanceY/2,tDistanceX/2,-tDistanceY/2,
					tDistanceX/2,tDistanceY/2,-tDistanceX/2,tDistanceY/2,0.0f,0.0f,0.0f,1.0f);

			mDecoratorManager.GetDisplay().RenderLine(tPartialX,-tDistanceY/2,tPartialX,tDistanceY/2,0.0f,0.0f,0.0f,1.0f);

			mDecoratorManager.GetDisplay().PopTransformation();
		}
	}

	if(mRectangleModel2->isPresent()){
		for(int i = 0 ; i < pParts ; i++){

			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(mRectangleModel1->getMarker(),mDecoratorManager.GetCam2World(),mDecoratorManager.GetWorld2Proj());

			tPartialX += tRectangleLength;

			mDecoratorManager.GetDisplay().RenderQuad(-tDistanceX/2,-tDistanceY/2,tDistanceX/2,-tDistanceY/2,
						tDistanceX/2,tDistanceY/2,-tDistanceX/2,tDistanceY/2,0.0f,0.0f,0.0f,1.0f);

			mDecoratorManager.GetDisplay().RenderLine(tPartialX,-tDistanceY/2,tPartialX,tDistanceY/2,0.0f,0.0f,0.0f,1.0f);

			mDecoratorManager.GetDisplay().PopTransformation();
		}
	}
}

void decorators::KillBugModel::DivideCircunferenceManipulatives(int pParts){
	float pRadio = 14;
	double tAngle = (double)(360.0/pParts);
	double tPartialAngle = 0.0f;

	mDecoratorManager.GetDisplay().PushTransformation();

	if(mCircularModel1->isPresent()){

		wykobi::quadix<float ,2> tMarkerCorners = mCircularModel1->getMarker().getCorners();
		wykobi::point2d<float> tXPoint = wykobi::centroid(tMarkerCorners[0],tMarkerCorners[1]);
		wykobi::point2d<float> tYPoint = wykobi::centroid(tMarkerCorners[0],tMarkerCorners[3]);

		float tDistance = wykobi::distance(tMarkerCorners[0].x,tMarkerCorners[0].y,tMarkerCorners[1].x,tMarkerCorners[1].y);
		float tDistance2 = wykobi::distance(tXPoint.x,tYPoint.y,tXPoint.x,tXPoint.y);
		//mDecoratorManager.GetCam2World().InterpolatedMap(tXPoint);
		//mDecoratorManager.GetCam2World().InterpolatedMap(tYPoint);


		mDecoratorManager.GetDisplay().RenderEllipse(tXPoint.x,tYPoint.y,pRadio,pRadio,0.0f,0.0f,0.0f,1.0f,1);


		/*
		 * wykobi::point2d<float> mOriginPoint = mAngleModel->OriginPoint();
		wykobi::point2d<float> mEndPoint = mAngleModel->EndPoint();

		wykobi::quadix<float ,2> tMarkerCorners = mAngleModel->getMarker().getCorners();
		wykobi::point2d<float> mStartPoint = wykobi::centroid(tMarkerCorners[0],tMarkerCorners[1]);

		wykobi::point2d<float> tWorldOriginPoint = mOriginPoint;
		wykobi::point2d<float> tWorldStartPoint = mStartPoint;
		wykobi::point2d<float> tWorldEndPoint = mEndPoint;

		mDecoratorManager.GetCam2World().InterpolatedMap(tWorldOriginPoint);
		mDecoratorManager.GetCam2World().InterpolatedMap(tWorldStartPoint);
		mDecoratorManager.GetCam2World().InterpolatedMap(tWorldEndPoint);

		float tAngle = wykobi::oriented_vertex_angle(tWorldStartPoint, tWorldOriginPoint, tWorldEndPoint, wykobi::Clockwise);

		mProportion = tAngle/360.0f;
		mDenominator = 10;
		mNumerator = ((int)(mProportion*100)%10 >= 5) ? ceil((mProportion*100)/10) : floor((mProportion*100)/10);

		 */
		/*mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(mCircularModel1->getMarker(),mDecoratorManager.GetCam2World(),mDecoratorManager.GetWorld2Proj());
		mDecoratorManager.GetDisplay().RenderEllipse(0.0f,0.0f,pRadio,pRadio,0.0f,0.0f,0.0f,1.0f,1);
		for(int i = 0 ; i < pParts ; i++){

			tPartialAngle += tAngle;

			float tPartialX = pRadio*wykobi::sin(tPartialAngle*wykobi::PI/180);
			float tPartialY = -pRadio*wykobi::cos(tPartialAngle*wykobi::PI/180);
			mDecoratorManager.GetDisplay().RenderLine(0.0f,0.0f,tPartialX,tPartialY,0.0f,0.0f,0.0f,1.0f);


		}*/
	}
	mDecoratorManager.GetDisplay().PopTransformation();
	if(mCircularModel2->isPresent()){
			/*for(int i = 0 ; i < pParts ; i++){

				mDecoratorManager.GetDisplay().PushTransformation();
				mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(mCircularModel2->getMarker(),mDecoratorManager.GetCam2World(),mDecoratorManager.GetWorld2Proj());


				tPartialAngle += tAngle;

				float tPartialX = pRadio*wykobi::sin(tPartialAngle*wykobi::PI/180);
				float tPartialY = -pRadio*wykobi::cos(tPartialAngle*wykobi::PI/180);
				mDecoratorManager.GetDisplay().RenderLine(0.0f,0.0f,tPartialX,tPartialY,0.0f,0.0f,0.0f,1.0f);
				mDecoratorManager.GetDisplay().RenderEllipse(0.0f,0.0f,pRadio,pRadio,0.0f,0.0f,0.0f,1.0f,1);
				mDecoratorManager.GetDisplay().PopTransformation();
			}*/
		}
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

		mDecoratorManager.GetDisplay().RenderFilledSector(1.0f,3.3f,1.5f,1.5f,tPartialDegree,0.0f,0.0f,tFull? 1.0 : 0.0f,tFull ? 0.0 : 1.0f,0.8f,1);

		mDecoratorManager.GetDisplay().RenderCenteredText(tFull?"Prêt!" :"En repos ...", 0.0f,5.5f,true,0.03f, 0.0f, tFull? 1.0f : 0.0f, tFull? 0.0f : 1.0f, 1.0f);
		mDecoratorManager.GetDisplay().PopTransformation();
		}
}
