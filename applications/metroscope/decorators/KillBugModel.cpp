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
		TokenModel *pTokenModel1, Flipper *pGoFlipper, FractionBugHint ** pFractionHints, FractionCard ** pFractionCards, Carte ** pCartes):
			FiducialDecorator(pDecoratorManager, 0),
			mCircularModel1(pAngleModel1), mCircularModel2(pAngleModel2),
			mRectangleModel1(pRectangleModel1), mRectangleModel2(pRectangleModel2),
			mTokenModel(pTokenModel1), mFlipper(pGoFlipper), mHints(pFractionHints), mFractionCards(pFractionCards), mCartes(pCartes),
			mActualCarte(0),mActualHint(0), mProportion1(0.0), mProportion2(0.0), mProportion3(0.0), mProportion4(0.0),
			mMapSize(0), mCellDimensionX(0), mCellDimensionY(0), mSteps(0),mGameStarted(false), mMapFinished(false), mActiveManipulatives(0),mLastShot(Time::MillisTimestamp()),
			mProportion1Numerator(0), mProportion1Denominator(1), mProportion2Numerator(0), mProportion2Denominator(1), mProportion3Numerator(0),
			mProportion3Denominator(1),mProportion4Numerator(0),mProportion4Denominator(1)
{
				mDisplayWidth = mDecoratorManager.GetDisplay().GetWidth();
				mDisplayHeight = mDecoratorManager.GetDisplay().GetHeight();
				mWorkingTriangle = mDisplayHeight/(float)2;
				mMapHeight = mDisplayHeight - (mWorkingTriangle);
				mMapWidth = mMapHeight; //square maps
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

		if(mFlipper->GetCurrentSide() != NULL && mFlipper->IsPresent()){

			DisplayFlipperFeedback();
		}

		if(IsHintPresent())	DisplayProportions(mActualHint->GetHintType());

		UpdateDeviceStatus();
	}
}

void decorators::KillBugModel::UpdateDeviceStatus(){
	//We update the device's networked state
	int distance = CalculateDistanceToTarget(mMapSize, mBugPosition, mActualCarte->getEndPoint(), mActualCarte->getObstaclesPoint());
	stateManager->SetActivityStepsToGo(distance);

	if(IsHintPresent()){
		std::ostringstream convert;   // stream used for the conversion
		convert << mActualHint->GetHintType();      // insert the textual representation of 'Number' in the characters in the stream
		std::string hint = convert.str(); // set 'Result' to the contents of the stream
		stateManager->SetActivityHintPresent(hint);
	}else stateManager->SetActivityHintPresent("");

	stateManager->SetActivityStepsDone(mSteps);
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

	wykobi::point2d<float> tBugOrigin = mActualCarte->getOriginPoint();

	if(!mGameStarted){
		mBugPosition.x = (int)tBugOrigin.x;
		mBugPosition.y = (int)tBugOrigin.y;
	}

	DisplayGrid();
	DisplayWorkingArea();

	//Bug Origin Point
	float tBugPositionX = mMapPoint1.x + (tBugOrigin.x)*(mCellDimensionX)+(mCellDimensionX/2);
	float tBugPositionY = mMapPoint1.y + (tBugOrigin.y)*(mCellDimensionY)+(mCellDimensionY/2);

	DisplayTexture();

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderFilledEllipse(tBugPositionX, tBugPositionY, mCellDimensionX/6,mCellDimensionY/6,1.0f,0.0f,0.0f,1.0f,3);
	mDecoratorManager.GetDisplay().PopTransformation();

	if(mMapFinished){
		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().RenderQuadFilled(0.0f, mDisplayHeight/2 - 20, mDisplayWidth, mDisplayHeight/2 - 20.0f, mDisplayWidth, mDisplayHeight + 20.0f, 0.0f , mDisplayHeight + 20.0f , 1.0f,1.0f,1.0f,1.0f);
		mDecoratorManager.GetDisplay().RenderCenteredText("Carte Fini!", mDisplayWidth/2, mDisplayHeight/2, true, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
		mDecoratorManager.GetDisplay().PopTransformation();
	}

	mGameStarted = true;
}

void decorators::KillBugModel::DisplayGrid(){
	float tActualX = mMapPoint1.x;
	float tActualY = mMapPoint1.y;

	mDecoratorManager.GetDisplay().PushTransformation();
	for(int i = 0 ; i < mMapSize + 1 ; i++){
			mDecoratorManager.GetDisplay().RenderLine(mMapPoint1.x,tActualY,mMapPoint2.x,tActualY,0.0f,0.0f,0.0f,1.0f);
			mDecoratorManager.GetDisplay().RenderLine(tActualX,mMapPoint1.y,tActualX,mMapPoint4.y, 0.0f,0.0f,0.0f,1.0f);
			tActualX += mCellDimensionX;
			tActualY += mCellDimensionY;
	}
	mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::KillBugModel::DisplayWorkingArea(){
	mDecoratorManager.GetDisplay().PushTransformation();

	mDecoratorManager.GetDisplay().RenderQuadFilled(mMapPoint1.x - mWorkingTriangle/2, mDisplayHeight/2,mMapPoint1.x , mMapPoint1.y ,
			mMapPoint4.x , mMapPoint4.y, mMapPoint1.x - mWorkingTriangle/2, mDisplayHeight/2,scProp2R,scProp2G,scProp2B,0.2f); //Prop 2 Space

	mDecoratorManager.GetDisplay().RenderQuadFilled(0.0f,0.0f,0.0f, mDisplayHeight/2, mMapPoint1.x - mWorkingTriangle/2, mDisplayHeight/2,
			mDisplayWidth/2, 0.0f, scProp2R,scProp2G,scProp2B,0.2f);//Prop 2 Working Area

	mDecoratorManager.GetDisplay().RenderQuadFilled(mMapPoint4.x, mMapPoint4.y, mMapPoint3.x, mMapPoint3.y, mDisplayWidth/2, mDisplayHeight,
			mMapPoint4.x, mMapPoint4.y,scProp3R,scProp3G,scProp3B,0.2f);

	mDecoratorManager.GetDisplay().RenderQuadFilled(0.0f, mDisplayHeight/2, mMapPoint1.x - mWorkingTriangle/2, mDisplayHeight/2,
			mDisplayWidth/2, mDisplayHeight, 0.0f, mDisplayHeight,scProp3R,scProp3G,scProp3B,0.2f);

	mDecoratorManager.GetDisplay().RenderQuadFilled(mMapPoint1.x, mMapPoint1.y, mMapPoint2.x, mMapPoint2.y,
				mMapPoint2.x - mWorkingTriangle/2, mMapPoint2.y - mWorkingTriangle/2,mMapPoint1.x, mMapPoint1.y, scProp1R,scProp1G,scProp1B,0.2f );

	mDecoratorManager.GetDisplay().RenderQuadFilled(mDisplayWidth, 0.0f, mDisplayWidth, mDisplayHeight/2,
			mMapPoint2.x + mWorkingTriangle/2, mDisplayHeight/2, mDisplayWidth/2, 0.0f, scProp1R,scProp1G,scProp1B,0.2f);

	mDecoratorManager.GetDisplay().RenderQuadFilled(mDisplayWidth, mDisplayHeight/2, mDisplayWidth, mDisplayHeight,
			mDisplayWidth/2, mDisplayHeight, mMapPoint2.x + mWorkingTriangle/2, mDisplayHeight/2, scProp4R,scProp4G,scProp4B, 0.2f);

	mDecoratorManager.GetDisplay().RenderQuadFilled(mMapPoint3.x, mMapPoint3.y, mMapPoint3.x + mWorkingTriangle/2, mDisplayHeight/2,
			mMapPoint2.x, mMapPoint2.y, mMapPoint3.x, mMapPoint3.y, scProp4R,scProp4G,scProp4B,0.2f);

	for(int i = -1 ; i < 2 ; i++){
		mDecoratorManager.GetDisplay().RenderLine(0.0f,mDisplayHeight/2 + i,mMapPoint1.x-mWorkingTriangle/2 + i,mDisplayHeight/2 + i,
					0.0f,0.0f,0.0f,1.0f);

		mDecoratorManager.GetDisplay().RenderLine(mMapPoint1.x - mWorkingTriangle/2 + i, mDisplayHeight/2 + i,
						mMapPoint1.x + mWorkingTriangle/2 + i, mMapPoint1.y - mWorkingTriangle/2 + i, 0.0f,0.0f,0.0f,1.0f); //Proportion 2

		mDecoratorManager.GetDisplay().RenderLine(mMapPoint1.x - mWorkingTriangle/2 + i, mDisplayHeight/2 + i,
						mMapPoint1.x + mWorkingTriangle/2 + i, mMapPoint3.y + mWorkingTriangle/2 + i, 0.0f,0.0f,0.0f,1.0f); //Proportion 3

		mDecoratorManager.GetDisplay().RenderLine(mMapPoint2.x + mWorkingTriangle/2 + i, mDisplayHeight/2 + i,
						mMapPoint2.x - mWorkingTriangle/2 + i, mMapPoint1.y - mWorkingTriangle/2 + i, 0.0f,0.0f,0.0f,1.0f); //Proportion 1

		mDecoratorManager.GetDisplay().RenderLine(mMapPoint2.x + mWorkingTriangle/2 + i, mDisplayHeight/2 + i,
						mMapPoint2.x - mWorkingTriangle/2 + i, mMapPoint3.y + mWorkingTriangle/2 + i, 0.0f,0.0f,0.0f,1.0f); //Proportion 4

		mDecoratorManager.GetDisplay().RenderLine(mMapPoint2.x + i + mWorkingTriangle/2 + i,mDisplayHeight/2 + i,mDisplayWidth,mDisplayHeight/2 + i,
						0.0f,0.0f,0.0f,1.0f);
	}


	mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::KillBugModel::DisplayEndingPoints(){
	int tEndTextureId = mDecoratorManager.GetDisplay().LoadTexture("./activities/proportions-network/end.jpg");
	std::vector<wykobi::point2d<float>> tEnd = mActualCarte->getEndPoint();

	mDecoratorManager.GetDisplay().PushTransformation();

	for(std::vector<wykobi::point2d<float>>::iterator it = tEnd.begin(); it != tEnd.end() ; ++it){
		mDecoratorManager.GetDisplay().RenderTexture(tEndTextureId,
					mMapPoint1.x + (*it).x*mCellDimensionX,mMapPoint1.y + (*it).y*mCellDimensionY,
					mMapPoint1.x + (*it).x*mCellDimensionX + mCellDimensionX,mMapPoint1.y + (*it).y*mCellDimensionY,
					mMapPoint1.x + (*it).x*mCellDimensionX + mCellDimensionX,mMapPoint1.y + (*it).y*mCellDimensionY + mCellDimensionY,
					mMapPoint1.x + (*it).x*mCellDimensionX,mMapPoint1.y + (*it).y*mCellDimensionY + mCellDimensionY);
	}

	mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::KillBugModel::DisplayObstacles(){
	int tObstacleTextureId = mDecoratorManager.GetDisplay().LoadTexture("./activities/proportions-network/obstacle.jpg");
	std::vector<wykobi::point2d<float>> tObstacle = mActualCarte->getObstaclesPoint();

	mDecoratorManager.GetDisplay().PushTransformation();

	for(std::vector<wykobi::point2d<float>>::iterator it = tObstacle.begin(); it != tObstacle.end() ; ++it){
		mDecoratorManager.GetDisplay().RenderTexture(tObstacleTextureId,
			mMapPoint1.x + (*it).x*mCellDimensionX,mMapPoint1.y + (*it).y*mCellDimensionY,
			mMapPoint1.x + (*it).x*mCellDimensionX + mCellDimensionX,mMapPoint1.y + (*it).y*mCellDimensionY,
			mMapPoint1.x + (*it).x*mCellDimensionX + mCellDimensionX,mMapPoint1.y + (*it).y*mCellDimensionY + mCellDimensionY,
			mMapPoint1.x + (*it).x*mCellDimensionX,mMapPoint1.y + (*it).y*mCellDimensionY + mCellDimensionY);
	}

	mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::KillBugModel::DisplayTexture(){

	DisplayObstacles();
	DisplayEndingPoints();

	int tTextureId = mDecoratorManager.GetDisplay().LoadTexture("./activities/proportions-network/grass.jpg");

	mDecoratorManager.GetDisplay().PushTransformation();

	//Grass Texture
	for (int i = 0 ; i < mMapSize ; i++){
		for (int j = 0 ; j < mMapSize ; j++){
			wykobi::point2d<int> tCell;
			tCell.x = i;
			tCell.y = j;
			if(mActualCarte->IsEmptyCell(i,j) && !mActualCarte->IsEndCell(i,j) && !IsBugPosition(i,j)) {
				mDecoratorManager.GetDisplay().RenderTexture(tTextureId,
						mMapPoint1.x + i*mCellDimensionX,mMapPoint1.y + j*mCellDimensionY,
						mMapPoint1.x + i*mCellDimensionX + mCellDimensionX,mMapPoint1.y + j*mCellDimensionY,
						mMapPoint1.x + i*mCellDimensionX + mCellDimensionX,mMapPoint1.y + j*mCellDimensionY + mCellDimensionY,
						mMapPoint1.x + i*mCellDimensionX,mMapPoint1.y + j*mCellDimensionY + mCellDimensionY);
			}
		}
	}

	mDecoratorManager.GetDisplay().PopTransformation();
}
bool decorators::KillBugModel::IsCartePresent(){
	Carte * tPreviusCarte = mActualCarte;
	for(int i = 0 ; i < scCarteCards ; i++){
		if(mCartes[i]->isPresent() && !mCartes[i]->IsFinished()){
			mActualCarte = mCartes[i];
			mMapFinished = mActualCarte->IsFinished();
			if(tPreviusCarte != mActualCarte)	Start();
			return true;
		}
	}
	return false;
}

bool decorators::KillBugModel::IsBugPosition(int pPositionX, int pPositionY){
	return (mBugPosition.x == pPositionX && mBugPosition.y == pPositionY);
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
	//float tBugPositionX = mMapPoint1.x + (mBugPosition.x)*(mCellDimensionX)+(mCellDimensionX/2);
	//float tBugPositionY = mMapPoint1.y + (mBugPosition.y)*(mCellDimensionY)+(mCellDimensionY/2);

	int tTextureId = mDecoratorManager.GetDisplay().LoadTexture("./activities/proportions-network/ladybug.jpg");

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderTexture(tTextureId,
							mMapPoint1.x + mBugPosition.x*mCellDimensionX,mMapPoint1.y + mBugPosition.y*mCellDimensionY,
							mMapPoint1.x + mBugPosition.x*mCellDimensionX + mCellDimensionX,mMapPoint1.y + mBugPosition.y*mCellDimensionY,
							mMapPoint1.x + mBugPosition.x*mCellDimensionX + mCellDimensionX,mMapPoint1.y + mBugPosition.y*mCellDimensionY + mCellDimensionY,
							mMapPoint1.x + mBugPosition.x*mCellDimensionX,mMapPoint1.y + mBugPosition.y*mCellDimensionY + mCellDimensionY);
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
		SetProportionNumber(mCircularModel1->GetCenter(),mCircularModel1->Numerator(), mCircularModel1->Denominator());
	}if (mCircularModel2->isPresent() && tProportionNumber < 4){
		tProportionNumber++;
		mActiveManipulatives++;
		SetProportionNumber(mCircularModel2->GetCenter(),mCircularModel2->Numerator(), mCircularModel2->Denominator());
	}

	//Checking the rectangular manipulative
	if (mRectangleModel1->isPresent() && tProportionNumber < 4){
		tProportionNumber++;
		mActiveManipulatives++;
		SetProportionNumber(mRectangleModel1->getMarker().getCenter(),mRectangleModel1->Numerator(), mRectangleModel1->Denominator());
	}if (mRectangleModel2->isPresent() && tProportionNumber < 4){
		tProportionNumber++;
		mActiveManipulatives++;
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
				SetProportionNumber(1,mTokenModel->GetNumerator(1), mTokenModel->GetDenominator(1));
			}if(mTokenModel->GetProportion(2) != 0.0f && tProportionNumber < 4){
				tProportionNumber++;
				mActiveManipulatives++;
				SetProportionNumber(2,mTokenModel->GetNumerator(2), mTokenModel->GetDenominator(2));
			}if(mTokenModel->GetProportion(3) != 0.0f && tProportionNumber < 4){
				tProportionNumber++;
				mActiveManipulatives++;
				SetProportionNumber(3,mTokenModel->GetNumerator(3), mTokenModel->GetDenominator(3));
			}if(mTokenModel->GetProportion(4) != 0.0f && tProportionNumber < 4){
				tProportionNumber++;
				mActiveManipulatives++;
				SetProportionNumber(4,mTokenModel->GetNumerator(4), mTokenModel->GetDenominator(4));
			}
		}
	}

	//Checking the cards
	for(int i = 0 ; i < scFractionCards ; i++){
		if(mFractionCards[i]->IsPresent() && tProportionNumber < 4){
			tProportionNumber++;
			mActiveManipulatives++;
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
	//TODO: Referenciate the points with mProportionXPoint
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderEllipse(mProportion1Point.x,mProportion1Point.y,mWorkingTriangle/8,mWorkingTriangle/8,0.0f,0.0f,0.0f,1.0f);
	//mDecoratorManager.GetDisplay().RenderEllipse(mDisplayWidth/2,mMapPoint1.y - (mDisplayHeight-mMapHeight)/4,mWorkingTriangle/4,mWorkingTriangle/4,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(mProportion1Point.x,mProportion1Point.y,mWorkingTriangle/8,mWorkingTriangle/8,mProportion1*360,90.0f - mProportion1*360,scProp1R,scProp1G,scProp1B,1.0f,1);
	//mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth/2,mMapPoint1.y - (mDisplayHeight-mMapHeight)/4,mWorkingTriangle/4,mWorkingTriangle/4,mProportion1*360,90.0f - mProportion1*360,0.0f,0.0f,0.0f,1.0f,1);

	//mDecoratorManager.GetDisplay().RenderEllipse(mMapPoint1.x - (mDisplayWidth-mMapWidth)/4,mDisplayHeight/2,mWorkingTriangle/4,mWorkingTriangle/4,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderEllipse(mProportion2Point.x,mProportion2Point.y,mWorkingTriangle/8,mWorkingTriangle/8,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(mProportion2Point.x,mProportion2Point.y,mWorkingTriangle/8,mWorkingTriangle/8,mProportion2*360,90.0f - mProportion2*360,scProp2R,scProp2G,scProp2B,1.0f,1);
	//mDecoratorManager.GetDisplay().RenderFilledSector(mMapPoint1.x - (mDisplayWidth-mMapWidth)/4,mDisplayHeight/2,mWorkingTriangle/4,mWorkingTriangle/4,mProportion2*360,90.0f - mProportion2*360,0.0f,0.0f,0.0f,1.0f,1);

	mDecoratorManager.GetDisplay().RenderEllipse(mDisplayWidth/2,mMapPoint3.y + (mDisplayHeight-mMapHeight)/4,mWorkingTriangle/4,mWorkingTriangle/4,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth/2,mMapPoint3.y + (mDisplayHeight-mMapHeight)/4,mWorkingTriangle/4,mWorkingTriangle/4,mProportion3*360,90.0f - mProportion3*360,0.0f,0.0f,0.0f,1.0f,1);

	mDecoratorManager.GetDisplay().RenderEllipse(mMapPoint3.x + (mDisplayWidth-mMapWidth)/4,mDisplayHeight/2,mWorkingTriangle/4,mWorkingTriangle/4,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(mMapPoint3.x + (mDisplayWidth-mMapWidth)/4,mDisplayHeight/2,mWorkingTriangle/4,mWorkingTriangle/4,mProportion4*360,90.0f - mProportion4*360,0.0f,0.0f,0.0f,1.0f,1);
	mDecoratorManager.GetDisplay().PopTransformation();

}
void decorators::KillBugModel::DisplayRectangularHint(){
	//TODO: Change color and size if needed
	float tRecWidth = mWorkingTriangle/1.2;
	float tRecHeight = tRecWidth*0.55;

	mDecoratorManager.GetDisplay().PushTransformation();

	//TODO: Referenciate the points with mProportionXPoint
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
	float tBoxWidth = mWorkingTriangle/1.2;
	float tBoxHeight = tBoxWidth*0.55;

	float tCellWidth = tBoxWidth/9;
	float tCellHeight = tBoxHeight/7;

	mDecoratorManager.GetDisplay().PushTransformation();

	//TODO: Do it in a proper way --> void decorators::KillBugModel::DisplayDiscreteHint(int pProportionNumber)
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

	//TODO: Do it in a proper way, like void decorators::KillBugModel::DisplayDecimalHint(int pProportionNumber)
	sprintf(tProportion1, "%3.2f", mProportion1);
	sprintf(tProportion2, "%3.2f", mProportion2);
	sprintf(tProportion3, "%3.2f", mProportion3);
	sprintf(tProportion4, "%3.2f", mProportion4);

	mDecoratorManager.GetDisplay().PushTransformation();

	//TODO: Referenciate them in terms of mProportionXPoint...
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion1,mDisplayWidth/2,mMapPoint1.y - (mDisplayHeight-mMapHeight)/4,true,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion2,mMapPoint1.x - (mDisplayWidth-mMapWidth)/4,mDisplayHeight/2,true,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion3,mDisplayWidth/2,mMapPoint3.y + (mDisplayHeight-mMapHeight)/4,true,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportion4,mMapPoint3.x + (mDisplayWidth-mMapWidth)/4,mDisplayHeight/2,true,1.0f,0.0f,0.0f,0.0f,1.0f);

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

	//TODO: Do it in a proper way, like void decorators::KillBugModel::DisplayFractionHint(int pProportionNumber)
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

			mDecoratorManager.GetDisplay().RenderQuad(-tDistanceX/2,-tDistanceY/2,tDistanceX/2,-tDistanceY/2, tDistanceX/2,tDistanceY/2,-tDistanceX/2,tDistanceY/2,0.0f,0.0f,0.0f,1.0f);
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
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderText("Entro DisplayFlipperFeedback", mDisplayWidth/2, mDisplayHeight/2 + 100.0f, 2.0f, 1.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().PopTransformation();

	static const long cShotPreparationTime = 6l*1000l;
	long tElapsedTime = Time::MillisTimestamp() - mLastShot;

	if (mFlipper->GetCurrentSide() != NULL && mFlipper->IsFlipped() && tElapsedTime > cShotPreparationTime) {
		MakeMove();
		mLastShot = Time::MillisTimestamp();
	}
	if (mFlipper->GetCurrentSide() != NULL && mFlipper->IsPresent()){
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

int decorators::KillBugModel::CalculateDistanceToTarget(int mapsize, wykobi::point2d<int> bugPosition, std::vector<wykobi::point2d<float>> endPoints, std::vector<wykobi::point2d<float>> obstacles){

	static const int scORIGIN = -1;
	static const int scEND = -2;
	static const int scOBSTACLE = -3;

	int distance = 0;
	if(endPoints.size()==0) return distance;//If we have no endpoint, we just return 0


	int map[mapsize][mapsize];//we create and initialize the map

	for(int i=0;i<mapsize;i++){
		for(int j=0;j<mapsize;j++) map[i][j] = 0;
	}

	map[bugPosition.x][bugPosition.y]=scORIGIN;//we set the bug position to be the origin

	for(int i=0;i<endPoints.size();i++){//we set the targets
		map[int(endPoints.at(i).x)][int(endPoints.at(i).y)] = scEND;
	}

	for(int i=0;i<obstacles.size();i++){//we set the obstacles
		map[int(obstacles.at(i).x)][int(obstacles.at(i).y)] = scOBSTACLE;
	}

	//We start the algorithm, from the current position
	int stepCount = 1;
	bool end=false;
	//initialize the flood area to be covered, to the current position
	int initX = bugPosition.x;
	int initY = bugPosition.y;
	int endX = bugPosition.x;
	int endY = bugPosition.y;
	do{
		//calculate the flood area to be covered
		if(initX>0) initX = initX-1;//either one column less, or remain at zero
		if(initY>0) initY = initY-1;//either one row less, or remain at zero
		if(endX<(mapsize-1)) endX = endX+1;//either one column more, or remain at n-1
		if(endY<(mapsize-1)) endY = endY+1;//either one row more, or remain at n-1

		//std::cout << stepCount << ": flooding from " << initX << "," << initY << " to " << endX << "," << endY << std::endl;

		//fill the zeros in the area with stepCount's
		for(int i=initX;i<=endX;i++){
			for(int j=initY;j<=endY;j++){
				int oldCell = map[i][j];//we store the cell, to check at the end
				if(map[i][j]==0 || map[i][j]==scEND){//if this was an unflooded cell (or the goal), we check for its neighbours, and set this cell to the smallest neighbour's+1 (or to 1, if a neighbour is the origin
					int minNeighbour = 100;
					//TODO: implement this decently in a function!
					if(i-1 >= 0 && j-1 >= 0){//i-1,j-1
						if(map[i-1][j-1] == scORIGIN) map[i][j]=1;
						else if(map[i-1][j-1]>0 && map[i-1][j-1]<minNeighbour) minNeighbour = map[i-1][j-1];
					}
					if( j-1 >= 0){//i,j-1
						if(map[i][j-1] == scORIGIN) map[i][j]=1;
						else if(map[i][j-1]>0 && map[i][j-1]<minNeighbour) minNeighbour = map[i][j-1];
					}
					if(i+1 <= mapsize-1 && j-1 >= 0){//i+1,j-1
						if(map[i+1][j-1] == scORIGIN) map[i][j]=1;
						else if(map[i+1][j-1]>0 && map[i+1][j-1]<minNeighbour) minNeighbour = map[i+1][j-1];
					}
					if( i-1 >= 0){//i-1,j
						if(map[i-1][j] == scORIGIN) map[i][j]=1;
						else if(map[i-1][j]>0 && map[i-1][j]<minNeighbour) minNeighbour = map[i-1][j];
					}
					if( i+1 <= mapsize-1){//i+1,j
						if(map[i+1][j] == scORIGIN) map[i][j]=1;
						else if(map[i+1][j]>0 && map[i+1][j]<minNeighbour) minNeighbour = map[i+1][j];
					}
					if(i-1 >=0 && j+1 <= mapsize-1){//i-1,j+1
						if(map[i-1][j+1] == scORIGIN) map[i][j]=1;
						else if(map[i-1][j+1]>0 && map[i-1][j+1]<minNeighbour) minNeighbour = map[i-1][j+1];
					}
					if( j+1 <= mapsize-1){//i,j+1
						if(map[i][j+1] == scORIGIN) map[i][j]=1;
						else if(map[i][j+1]>0 && map[i][j+1]<minNeighbour) minNeighbour = map[i][j+1];
					}
					if(i+1 <= mapsize-1 && j+1 <= mapsize-1){//i+1,j+1
						if(map[i+1][j+1] == scORIGIN) map[i][j]=1;
						else if(map[i+1][j+1]>0 && map[i+1][j+1]<minNeighbour) minNeighbour = map[i+1][j+1];
					}


					if((map[i][j]==0 || map[i][j]==scEND) && minNeighbour!=100) map[i][j]=minNeighbour+1;//if we did not set the cell already, we set it now
					//std::cout << "minimal neighbour " << minNeighbour << ". map cell set to " << map[i][j] << std::endl;
				}
				if(oldCell==scEND){
					if(map[i][j]>0){//we found an end!
						distance = map[i][j];
						end=true;
					}
					else map[i][j] = oldCell;//we put the goal back again
				}
			}
		}

		if(initX==0 && initY==0 && endX==(mapsize-1) && endY==(mapsize-1)) end=true;// We flooded the map! if we did not find anything, there is no goal or the goal is unreachable (we return zero)

		stepCount++;
	}while(!end);


	return distance;
}
