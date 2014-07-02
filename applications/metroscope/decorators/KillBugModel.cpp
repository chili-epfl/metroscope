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
		DisplayMap(); //TODO: Put this as KillBugView.DisplayMap()

		if(mGameStarted){
			FetchProportions();
			DisplayBug(); //TODO: Put this as KillBugView.DisplayBug()
		}

		if(mFlipper->IsPresent()) DisplayFlipperFeedback(); //TODO: Put this as KillBugView.DisplayFlipperFeedback()

		if(IsHintPresent())	DisplayProportions(mActualHint->GetHintType()); //TODO: Put this as KillBugView.DisplayProportions()
	}
}

/*
 * TODO: Eliminate this method from this class, put it in KillBugView
 */
void decorators::KillBugModel::DisplayProportions(){
	RenderProportion(mProportion1, 1);
	RenderProportion(mProportion2, 2);
	RenderProportion(mProportion3, 3);
	RenderProportion(mProportion4, 4);
}

/*
 * TODO: Eliminate this method from this class, put it in KillBugView
 */
void decorators::KillBugModel::DisplayProportions(int pType){
	switch(pType){
		case 0: DisplayDiscreteHint(); break;
		case 1: DisplayFractionHint(); break;
		case 2: DisplayDecimalHint();break;
		case 3: DisplayRectangularHint(); break;
		case 4: DisplayCircularHint(); break;
	}
}

/*
 * TODO: Eliminate this method from this class, put it in KillBugView
 */
void decorators::KillBugModel::RenderProportion(float pProportion, int pProportionNumber){
	char tProportion[3];
	sprintf(tProportion, "%3.2f", pProportion);
	wykobi::point2d<float> tPosition;
	float tR, tG, tB;

	switch(pProportionNumber){
	case 1:

		tPosition.x = mProportion1Point.x;
		tPosition.y = mProportion1Point.y;
		tR = 0.0f; tG = 0.0f; tB = 1.0f;

		break;
	case 2:

		tPosition.x = mProportion2Point.x;
		tPosition.y = mProportion2Point.y;

		tR = 1.0f; tG = 0.0f; tB = 1.0f;
		break;
	case 3:
		tPosition.x = mProportion3Point.x;
		tPosition.y = mProportion3Point.y;
		tR = 1.0f; tG = 0.0f; tB = 0.0f;
		break;
	case 4:
		tPosition.x = mProportion4Point.x;
		tPosition.y = mProportion4Point.y;
		tR = 0.0f; tG = 1.0f; tB = 0.0f;
		break;
	}

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderText(tProportion,tPosition.x,tPosition.y,1.0f,tR,tG,tB,1.0f);
	mDecoratorManager.GetDisplay().PopTransformation();

}

/*
 * TODO: Eliminate this method from this class, put it in KillBugView
 */
void decorators::KillBugModel::DisplayMap(){


	mMapSize = mActualCarte->getSize();

	mCellDimensionX = (mMapPoint3.x - mMapPoint1.x)/(2*mMapSize);
	mCellDimensionY = (mMapPoint4.y - mMapPoint2.y)/(2*mMapSize);

	wykobi::point2d<float> tBugOrigin = mActualCarte->getOriginPoint();

	if(!mGameStarted){
		mBugPosition.x = (int)tBugOrigin.x;
		mBugPosition.y = (int)tBugOrigin.y;
	}

	float tBugPositionX = mMapPoint2.x + (tBugOrigin.x - tBugOrigin.y)*mCellDimensionX;
	float tBugPositionY = mMapPoint2.y + (tBugOrigin.x + tBugOrigin.y + 1)*mCellDimensionY;

	//Grid
	//TODO: Make a method
	mDecoratorManager.GetDisplay().PushTransformation();

	mDecoratorManager.GetDisplay().RenderQuadFilled(mMapPoint1.x, mMapPoint1.y, mMapPoint2.x, mMapPoint2.y,
		mMapPoint3.x, mMapPoint3.y, mMapPoint4.x, mMapPoint4.y, 1.0f,1.0f,1.0f,1.0f);

	mDecoratorManager.GetDisplay().RenderQuad(mMapPoint1.x, mMapPoint1.y, mMapPoint2.x, mMapPoint2.y, mMapPoint3.x, mMapPoint3.y, mMapPoint4.x, mMapPoint4.y, 0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(0.0f, 0.0f, mWorkingTriangle/2,mWorkingTriangle/2,90.0f,270.0f,scProp2R,scProp2G,scProp2B,0.1f,1); //Prop 2
	mDecoratorManager.GetDisplay().RenderFilledSector(0.0f, mDisplayHeight, mWorkingTriangle/2,mWorkingTriangle/2,90.0f,360.0f,scProp3R,scProp3G,scProp3B,0.1f,1); //Prop3
	mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth, 0.0f, mWorkingTriangle/2,mWorkingTriangle/2,90.0f,180.0f,scProp1R,scProp1G,scProp1B,0.1f,1); //Prop1
	mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth, mDisplayHeight, mWorkingTriangle/2,mWorkingTriangle/2,90.0f,90.0f,scProp4R,scProp4G,scProp4B,0.1f,1); //Prop4


	//Bug Origin Point
	//TODO: Make a method
	mDecoratorManager.GetDisplay().RenderFilledEllipse(tBugPositionX, tBugPositionY, mCellDimensionX/6,mCellDimensionY/6,1.0f,0.0f,0.0f,1.0f,1);

	//Ending points
	//TODO: Make a method
	if(mActualCarte->getEndNumber() > 0){
		std::vector<wykobi::point2d<float>> tEnd = mActualCarte->getEndPoint();

		for(std::vector<wykobi::point2d<float>>::iterator it = tEnd.begin(); it != tEnd.end() ; ++it){
			float tEndPositionX = mMapPoint2.x + ((*it).x - (*it).y)*mCellDimensionX;
			float tEndPositionY = mMapPoint2.y + ((*it).x + (*it).y + 1)*mCellDimensionY;
			mDecoratorManager.GetDisplay().RenderFilledEllipse(tEndPositionX, tEndPositionY, mCellDimensionX/2,mCellDimensionY/2,0.0f,0.0f,0.0f,1.0f,1);

		}
	}

	//Obstacles points
	//TODO: make a method
	if(mActualCarte->getObstacleNumber() > 0){
	int tObstacleTextureId = mDecoratorManager.GetDisplay().LoadTexture("./activities/proportions-network/obstacle-small.jpg");
		std::vector<wykobi::point2d<float>> tObstacle = mActualCarte->getObstaclesPoint();
		for(std::vector<wykobi::point2d<float>>::iterator it = tObstacle.begin(); it != tObstacle.end() ; ++it){

			float tPoint1X = mMapPoint2.x + ((*it).x - (*it).y)*mCellDimensionX;
			float tPoint1Y = mMapPoint2.y + ((*it).y + (*it).x)*mCellDimensionY;
			float tPoint2X = mMapPoint2.x + ((*it).x - (*it).y + 1)*mCellDimensionX;
			float tPoint2Y = mMapPoint2.y + ((*it).y + (*it).x + 1)*mCellDimensionY;
			float tPoint3X = mMapPoint2.x + ((*it).x - (*it).y )*mCellDimensionX;
			float tPoint3Y = mMapPoint2.y + ((*it).y + (*it).x + 2)*mCellDimensionY;
			float tPoint4X = mMapPoint2.x + ((*it).x - (*it).y - 1)*mCellDimensionX;
			float tPoint4Y = mMapPoint2.y + ((*it).y + (*it).x + 1)*mCellDimensionY;

			mDecoratorManager.GetDisplay().RenderTexture(tObstacleTextureId,
					tPoint1X, tPoint1Y,tPoint2X, tPoint2Y,tPoint3X, tPoint3Y,tPoint4X, tPoint4Y);
		}
	}

	if(mMapFinished){
		mDecoratorManager.GetDisplay().RenderQuadFilled(mMapPoint1.x, mDisplayHeight/2 - mMapHeight/4,
				mMapPoint2.x, mDisplayHeight/2 - mMapHeight/4, mMapPoint2.x, mDisplayHeight/2 + mMapHeight/4,
				mMapPoint1.x, mDisplayHeight/2 + mMapHeight/4, 0.8f,0.8f,0.8f,0.5f);
		mDecoratorManager.GetDisplay().RenderText("Carte finie!", mMapPoint1.x,  mDisplayHeight/2, 1.2f,0.0f,0.0f,0.0f,1.0f);
	}

	if(mWrongMovementFrames>0){
		mDecoratorManager.GetDisplay().RenderText("Je ne peux pas y aller...", mBugPosition.x*mCellDimensionX + mMapPoint1.x + 10.0f,
				mBugPosition.y*mCellDimensionY + mMapPoint1.y - 10.0f, 0.95f,0.0f,0.0f,0.0f,1.0f);
		mWrongMovementFrames--;
	}
	if(mNewMapFrames>0){
		mDecoratorManager.GetDisplay().RenderQuadFilled(mMapPoint1.x, mDisplayHeight/2 - mMapHeight/4,
						mMapPoint2.x, mDisplayHeight/2 - mMapHeight/4, mMapPoint2.x, mDisplayHeight/2 + mMapHeight/4,
						mMapPoint1.x, mDisplayHeight/2 + mMapHeight/4, 0.8f,0.8f,0.8f,0.5f);
		mDecoratorManager.GetDisplay().RenderText("Carte nouvelle!", mMapPoint1.x, mDisplayHeight/2, 1.2f,0.0f,0.0f,0.0f,1.0f);
		mNewMapFrames--;
	}


	//TODO: make a method
	if(mProportion1Greater && mProportionFeedbackFrames13>0){
		bool tEven = (mProportionFeedbackFrames13%2 == 0);
		mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth, 0.0f, mWorkingTriangle/2,mWorkingTriangle/2,90.0f,180.0f,scProp1R,scProp1G,scProp1B,tEven? 0.2:0.09f,1); //Prop1
		mProportionFeedbackFrames13--;
	} else if(mProportion3Greater && mProportionFeedbackFrames13>0){
		bool tEven = (mProportionFeedbackFrames13%2 == 0);
		mDecoratorManager.GetDisplay().RenderFilledSector(0.0f, mDisplayHeight, mWorkingTriangle/2,mWorkingTriangle/2,90.0f,360.0f,scProp3R,scProp3G,scProp3B,tEven? 0.2:0.09f,1); //Prop3
		mProportionFeedbackFrames13--;
	}

	if(mProportion2Greater && mProportionFeedbackFrames24>0){
		bool tEven = (mProportionFeedbackFrames24%2 == 0);
		mDecoratorManager.GetDisplay().RenderFilledSector(0.0f, 0.0f, mWorkingTriangle/2,mWorkingTriangle/2,90.0f,270.0f,scProp2R,scProp2G,scProp2B,tEven? 0.2:0.09f,1); //Prop 2
		mProportionFeedbackFrames24--;
	} else if(mProportion4Greater && mProportionFeedbackFrames24>0){
		bool tEven = (mProportionFeedbackFrames24%2 == 0);
		mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth, mDisplayHeight, mWorkingTriangle/2,mWorkingTriangle/2,90.0f,90.0f,scProp4R,scProp4G,scProp4B,tEven? 0.2:0.09f,1); //Prop4
		mProportionFeedbackFrames24--;
	}

	mDecoratorManager.GetDisplay().PopTransformation();
	mGameStarted = true;

}

/*
 * Returns true if one of the maps has been showed, the map doesn't need to be all the time in the table (just to have less cards in the table)
 */
bool decorators::KillBugModel::IsCartePresent(){
	Carte * tPreviusCarte = mActualCarte;
	for(int i = 0 ; i < scCarteCards ; i++){
		if(mCartes[i]->isPresent()){
			mActualCarte = mCartes[i];

			if(tPreviusCarte != mActualCarte)	Start();
		}
	}
	mMapFinished = mActualCarte->IsFinished();
	return (mActualCarte!=NULL);
}

/*
 * Returns true if one of the hints has been showed, it doesn't need to persist (just to have less cards in the table)
 */
bool decorators::KillBugModel::IsHintPresent(){
	for(int i = 0 ; i < scHintCards ; i++){
		if(mHints[i]->IsPresent()){
			mActualHint = mHints[i];
		}
	}
	return (mActualHint!=NULL);
}

void decorators::KillBugModel::MakeMove(){
	int tNewPositionX;
	int tNewPositionY;

	mProportion1Greater = false;
	mProportion3Greater = false;
	mProportion2Greater = false;
	mProportion4Greater = false;

	//bool wrongMove is: 1) try to go to an obstacle 2) try to go out the map
		if(mProportion2 < mProportion4){
			tNewPositionX = (mBugPosition.x +1 < mActualCarte->getSize()) ? mBugPosition.x + 1 : mBugPosition.x;
			mWrongMove = !(mBugPosition.x +1 < mActualCarte->getSize()); //Try to go out the map
			mProportion2Greater = false;
			mProportion4Greater = true;
		} else if(mProportion2 > mProportion4){
			tNewPositionX = (mBugPosition.x > 0) ? mBugPosition.x - 1 : mBugPosition.x;
			mWrongMove = !(mBugPosition.x > 0); //Try to go out the map
			mProportion2Greater = true;
			mProportion4Greater = false;
		} else if (mProportion2 == mProportion4){
			tNewPositionX = mBugPosition.x;
			mWrongMove = false;
			mProportion2Greater = false;
			mProportion4Greater = false;
		}

		if(mProportion1 < mProportion3){
			if(!mWrongMove){
				tNewPositionY = (mBugPosition.y +1 < mActualCarte->getSize()) ? mBugPosition.y + 1 : mBugPosition.y;
				mWrongMove = !(mBugPosition.y +1 < mActualCarte->getSize()); //Try to go out the map
			}
			mProportion1Greater = false;
			mProportion3Greater = true;
		} else if(mProportion1 > mProportion3 ){
			if(!mWrongMove){
				tNewPositionY = (mBugPosition.y > 0) ?  mBugPosition.y - 1 :  mBugPosition.y;
				mWrongMove = !(mBugPosition.y > 0);
			}
			mProportion1Greater = true;
			mProportion3Greater = false;
		} else if(mProportion1 == mProportion3){
			if(!mWrongMove){
				tNewPositionY = mBugPosition.y;
				mWrongMove = false;
			}
			mProportion1Greater = false;
			mProportion3Greater = false;
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
		}

		if(mWrongMove) mWrongMovementFrames = 30;
		if(mProportion2Greater || mProportion4Greater)	mProportionFeedbackFrames24 = 30;
		if(mProportion1Greater || mProportion3Greater)	mProportionFeedbackFrames13 = 30;
		mSteps++;
	//}
}

void decorators::KillBugModel::Start(){
	mGameStarted = false;
	mSteps = 0;
	mNewMapFrames = 30;
	mProportion1 = 0.0f;
	mProportion2 = 0.0f;
	mProportion3 = 0.0f;
	mProportion4 = 0.0f;
}

/*
 * TODO: Eliminate this method from this class, put it in KillBugView
 */
void decorators::KillBugModel::DisplayBug(){

	int tTextureId = mDecoratorManager.GetDisplay().LoadTexture("./activities/proportions-network/ladybug-smallwhite.jpg");

	//Display the image
	mDecoratorManager.GetDisplay().PushTransformation();

	mDecoratorManager.GetDisplay().RenderTexture(tTextureId,
			mMapPoint2.x + (mBugPosition.x - mBugPosition.y)*mCellDimensionX,mMapPoint2.y + (mBugPosition.y + mBugPosition.x)*mCellDimensionY,
				mMapPoint2.x + (mBugPosition.x - mBugPosition.y + 1)*mCellDimensionX,mMapPoint2.y + (mBugPosition.y + mBugPosition.x + 1)*mCellDimensionY,
				mMapPoint2.x + (mBugPosition.x - mBugPosition.y )*mCellDimensionX,mMapPoint2.y + (mBugPosition.y + mBugPosition.x + 2)*mCellDimensionY,
				mMapPoint2.x + (mBugPosition.x - mBugPosition.y - 1)*mCellDimensionX,mMapPoint2.y + (mBugPosition.y + mBugPosition.x + 1)*mCellDimensionY);

	mDecoratorManager.GetDisplay().PopTransformation();

}

void decorators::KillBugModel::FetchProportions(){
	int tProportionNumber = 0;
	mProportion1 = 0.0f;
	mProportion2 = 0.0f;
	mProportion3 = 0.0f;
	mProportion4 = 0.0f;

	//Checking the circular manipulative
	if (mCircularModel1->isPresent() && tProportionNumber < 4){
		tProportionNumber++;
		SetProportionNumber(mCircularModel1->GetCenter(),mCircularModel1->Numerator(), mCircularModel1->Denominator());
	}if (mCircularModel2->isPresent() && tProportionNumber < 4){
		tProportionNumber++;
		SetProportionNumber(mCircularModel2->GetCenter(),mCircularModel2->Numerator(), mCircularModel2->Denominator());
	}

	//Checking the rectangular manipulative
	if (mRectangleModel1->isPresent() && tProportionNumber < 4){
		tProportionNumber++;
		SetProportionNumber(mRectangleModel1->getMarker().getCenter(),mRectangleModel1->Numerator(), mRectangleModel1->Denominator());
	}if (mRectangleModel2->isPresent() && tProportionNumber < 4){
		tProportionNumber++;
		SetProportionNumber(mRectangleModel2->getMarker().getCenter(),mRectangleModel2->Numerator(), mRectangleModel2->Denominator());
	}

	//Checking the token manipulative
	if(mTokenModel->isPresent() && tProportionNumber < 4){
		if(!mTokenModel->AreTokensSpread()){
			tProportionNumber++;
			int tProportionNumber = GetProportionNumber(mTokenModel->GetPosition());
			SetProportionNumber(tProportionNumber,mTokenModel->GetNumerator(tProportionNumber), mTokenModel->GetDenominator(tProportionNumber));
		}else{
			if(tProportionNumber < 4 && mTokenModel->isPresent(1)){
				tProportionNumber++;
				SetProportionNumber(1,mTokenModel->GetNumerator(1), mTokenModel->GetDenominator(1));
			}if(tProportionNumber < 4 && mTokenModel->isPresent(2)){
				tProportionNumber++;
				SetProportionNumber(2,mTokenModel->GetNumerator(2), mTokenModel->GetDenominator(2));
			}if(tProportionNumber < 4 && mTokenModel->isPresent(3)){
				tProportionNumber++;
				SetProportionNumber(3,mTokenModel->GetNumerator(3), mTokenModel->GetDenominator(3));
			}if(tProportionNumber < 4 && mTokenModel->isPresent(4)){
				tProportionNumber++;
				SetProportionNumber(4,mTokenModel->GetNumerator(4), mTokenModel->GetDenominator(4));
			}
		}
	}

	//Checking the cards
	for(int i = 0 ; i < scFractionCards ; i++){
		if(mFractionCards[i]->IsPresent() && tProportionNumber < 4){
			tProportionNumber++;
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

/*
 * TODO: Eliminate this method from this class, put it in KillBugView
 */
void decorators::KillBugModel::DisplayCircularHint(){

	float tRadius = mWorkingTriangle/18;
	//TODO: Change color and size if needed
	mDecoratorManager.GetDisplay().PushTransformation();

	mDecoratorManager.GetDisplay().RenderEllipse(mProportion1Point.x,mProportion1Point.y,tRadius,tRadius,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(mProportion1Point.x,mProportion1Point.y,tRadius,tRadius,mProportion1*360,90.0f - mProportion1*360,scProp1R,scProp1G,scProp1B,1.0f,1);

	mDecoratorManager.GetDisplay().RenderEllipse(mProportion2Point.x,mProportion2Point.y,tRadius,tRadius,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(mProportion2Point.x,mProportion2Point.y,tRadius,tRadius,mProportion2*360,90.0f - mProportion2*360,scProp2R,scProp2G,scProp2B,1.0f,1);

	mDecoratorManager.GetDisplay().RenderEllipse(mProportion3Point.x,mProportion3Point.y,tRadius,tRadius,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(mProportion3Point.x,mProportion3Point.y,tRadius,tRadius,mProportion3*360,90.0f - mProportion3*360,scProp3R,scProp3G,scProp3B,1.0f,1);

	mDecoratorManager.GetDisplay().RenderEllipse(mProportion4Point.x,mProportion4Point.y,tRadius,tRadius,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(mProportion4Point.x,mProportion4Point.y,tRadius,tRadius,mProportion4*360,90.0f - mProportion4*360,scProp4R,scProp4G,scProp4B,1.0f,1);

	mDecoratorManager.GetDisplay().PopTransformation();

}

/*
 * TODO: Eliminate this method from this class, put it in KillBugView
 */
void decorators::KillBugModel::DisplayRectangularHint(){
	//TODO: Change color and size if needed
	float tRecWidth = mWorkingTriangle/12;
	float tRecHeight = tRecWidth/2;

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

/*
 * TODO: Eliminate this method from this class, put it in KillBugView
 */
void decorators::KillBugModel::DisplayDiscreteHint(){
	float tBoxWidth = mWorkingTriangle/12;
	float tBoxHeight = tBoxWidth/1.5;

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
				mDecoratorManager.GetDisplay().RenderQuad(tPartialX,tPartialY,tPartialX+tCellWidth,tPartialY,
										tPartialX+tCellWidth,tPartialY+tCellHeight,tPartialX,tPartialY+tCellHeight,
										0.0f, 0.0f, 0.0f,1.0f);
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
					mDecoratorManager.GetDisplay().RenderQuad(tPartialX,tPartialY,tPartialX+tCellWidth,tPartialY,
											tPartialX+tCellWidth,tPartialY+tCellHeight,tPartialX,tPartialY+tCellHeight,
											0.0f, 0.0f, 0.0f,1.0f);
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
						mDecoratorManager.GetDisplay().RenderQuad(tPartialX,tPartialY,tPartialX+tCellWidth,tPartialY,
												tPartialX+tCellWidth,tPartialY+tCellHeight,tPartialX,tPartialY+tCellHeight,
												0.0f, 0.0f, 0.0f,1.0f);
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
							mDecoratorManager.GetDisplay().RenderQuad(tPartialX,tPartialY,tPartialX+tCellWidth,tPartialY,
													tPartialX+tCellWidth,tPartialY+tCellHeight,tPartialX,tPartialY+tCellHeight,
													0.0f, 0.0f, 0.0f,1.0f);
							tPartialX += 2*tCellWidth;
							tDenom--;
							tNum--;
						}
					}
					tPartialY += 2*tCellHeight;
				}

	mDecoratorManager.GetDisplay().PopTransformation();

}

/*
 * TODO: Eliminate this method from this class, put it in KillBugView
 */
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
			mProportion1Point.x,mProportion1Point.y,true,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion2,
			mProportion2Point.x,mProportion2Point.y,true,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion3,
			mProportion3Point.x,mProportion3Point.y,true,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion4,
			mProportion4Point.x,mProportion4Point.y,true,1.0f,0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().PopTransformation();
}

/*
 * TODO: Eliminate this method from this class, put it in KillBugView
 */
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
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion1Numerator,mProportion1Point.x,mProportion1Point.y - 15.0f,true,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion1Denominator,mProportion1Point.x,mProportion1Point.y + 15.0f,true,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion2Numerator,mProportion2Point.x,mProportion2Point.y - 15.0f,true,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion2Denominator,mProportion2Point.x,mProportion2Point.y + 15.0f,true,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion3Numerator,mProportion3Point.x,mProportion3Point.y - 15.0f,true,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion3Denominator,mProportion3Point.x,mProportion3Point.y + 15.0f,true,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion4Numerator,mProportion4Point.x,mProportion4Point.y - 15.0f,true,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion4Denominator,mProportion4Point.x,mProportion4Point.y + 15.0f,true,1.0f,0.0f,0.0f,0.0f,1.0f);


	mDecoratorManager.GetDisplay().RenderLine(mProportion1Point.x-15,mProportion1Point.y,mProportion1Point.x+15,mProportion1Point.y,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(mProportion2Point.x-15,mProportion2Point.y,mProportion2Point.x+15,mProportion2Point.y,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(mProportion3Point.x-15,mProportion3Point.y,mProportion3Point.x+15,mProportion3Point.y,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(mProportion4Point.x-15,mProportion4Point.y,mProportion4Point.x+15,mProportion4Point.y,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().PopTransformation();

}

/*
 * TODO: Eliminate this method from this class, put it in KillBugView
 */
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

/*
 * TODO: Eliminate this method from this class, put it in KillBugView
 */
void decorators::KillBugModel::DivideCircunferenceManipulatives(int pParts){
	float pRadio = 14;

	mDecoratorManager.GetDisplay().PushTransformation();

	if(mCircularModel1->isPresent()){

		wykobi::quadix<float ,2> tMarkerCorners = mCircularModel1->getMarker().getCorners();
		wykobi::point2d<float> tXPoint = wykobi::centroid(tMarkerCorners[0],tMarkerCorners[1]);
		wykobi::point2d<float> tYPoint = wykobi::centroid(tMarkerCorners[0],tMarkerCorners[3]);

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

/*
 * TODO: Eliminate this method from this class, put it in KillBugView
 */
void decorators::KillBugModel::DisplayFlipperFeedback(){
	static const long cShotPreparationTime = 6l*1000l;
	long tElapsedTime = Time::MillisTimestamp() - mLastShot;

	if (mFlipper->IsFlipped() && tElapsedTime > cShotPreparationTime) {
		MakeMove();
		mLastShot = Time::MillisTimestamp();
	}
	if (mFlipper->IsPresent() && mFlipper->GetCurrentSide() != NULL){
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
