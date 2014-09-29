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

#include "KillBugView.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>
#include <wykobi/wykobi_utilities.hpp>

const std::string decorators::KillBugView::scDecoratorName("KillBugView");
const  DecoratorManager::Registerer decorators::KillBugView::mRegisterer(decorators::KillBugView::scDecoratorName, &decorators::KillBugView::create);

decorators::FiducialDecorator *decorators::KillBugView::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::KillBugView(pDecoratorManager,
				(KillBugModel *) pDecoratorManager.loadDecorator(pSetting["model"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

// TODO: Maintain the order of declaration in KillBugView.hpp
decorators::KillBugView::KillBugView(DecoratorManager &pDecoratorManager,KillBugModel *pKillBugModel):
	FiducialDecorator(pDecoratorManager, 0),
	mKillBugModel(pKillBugModel),
	mActualMap(),
	mActualHint(),
	mProportion1(0.0f),
	mProportion2(0.0f),
	mProportion3(0.0f),
	mProportion4(0.0f),
	mActualFlipper(),
	mMapSize(0),
	mBugPosition(wykobi::make_point(0,0)),
	mSteps(0),
	mGameStarted(false),
	mMapFinished(false),
	mMapNew(false),
	mProportion1Numerator(0),
	mProportion1Denominator(1),
	mProportion2Numerator(0),
	mProportion2Denominator(1),
	mProportion3Numerator(0),
	mProportion3Denominator(1),
	mProportion4Numerator(0),
	mProportion4Denominator(1),
	mWrongMovementFrames(0),
	mNewMapFrames(0),
	mProportionFeedbackFrames13(0), //CHECK
	mProportionFeedbackFrames24(0), //CHECK
	mProportion1Greater(false),
	mProportion2Greater(false),
	mProportion3Greater(false),
	mProportion4Greater(false),
	mBugTrajectory(),
	mCellDimensionX(0.0f),
	mCellDimensionY(0.0f),
	mLastShot(Time::MillisTimestamp())
	{
		mDisplayWidth = mDecoratorManager.GetDisplay().GetWidth();
		mDisplayHeight = mDecoratorManager.GetDisplay().GetHeight();
		mWorkingTriangle = (0.90*mDisplayHeight);
		mMapWidth = mDisplayWidth - mWorkingTriangle;
		mMapHeight = mMapWidth;

		mMapPoint1 = wykobi::make_point((int)(mWorkingTriangle/2.5),(int)(mDisplayHeight/2));
		mMapPoint2 = wykobi::make_point((int)(mDisplayWidth/2),(int)(mWorkingTriangle/2.5 + mDisplayHeight/2 - mDisplayWidth/2));
		mMapPoint3 = wykobi::make_point((int)(mDisplayWidth-mWorkingTriangle/2.5),(int)(mDisplayHeight/2));
		mMapPoint4 = wykobi::make_point((int)(mDisplayWidth/2),(int)(mDisplayHeight/2 - mWorkingTriangle/2.5 + mDisplayWidth/2));

		mProportion1Point = wykobi::make_point((int)((mMapPoint2.x + mMapPoint3.x)/2 + 40.0f),(int)((mMapPoint2.y + mMapPoint3.y)/2 - 40.0f));
		mProportion2Point = wykobi::make_point((int)((mMapPoint2.x + mMapPoint1.x)/2 - 40.0f),(int)((mMapPoint2.y + mMapPoint1.y)/2 - 40.0f));
		mProportion3Point = wykobi::make_point((int)((mMapPoint4.x + mMapPoint1.x)/2 - 40.0f),(int)((mMapPoint4.y + mMapPoint1.y)/2 + 40.0f));
		mProportion4Point = wykobi::make_point((int)((mMapPoint4.x + mMapPoint3.x)/2 + 40.0f),(int)((mMapPoint4.y + mMapPoint3.y)/2 + 40.0f));
	}

decorators::KillBugView::~KillBugView(){

}

/*
 * Is a map is present, then we show the map
 * If a hint is present, then we display the corresponding hint
 * If the flipper is present we display the feedback
 */
void decorators::KillBugView::update() {
	if(mKillBugModel->IsMapPresent() && mKillBugModel->IsCurrentActivity()){
		Carte * tPreviousMap = mActualMap;
		mActualMap = mKillBugModel->GetActualMap();
		if(tPreviousMap!=mActualMap)	RestartTrajectory();
		DisplayMap();
		if(mKillBugModel->IsGameStarted()){
			displayBug();
		}
		if(mKillBugModel->IsAnyHintPresent()){
			mActualHint = mKillBugModel->GetActualHint();
			displayHint();
		}
		if(mKillBugModel->IsFlipperPresent()){
			mActualFlipper = mKillBugModel->GetActualFlipper();
			DisplayFlipperFeedback();
		}
	}
}

/*
 *	Draws the origin, obstacle and ending points
 *	Also initializes the game
 */
void decorators::KillBugView::DisplayMap(){

	//	Get the size of the map to calculate the dimension of each cell:
	mMapSize = mKillBugModel->GetActualMap()->getSize();
	mCellDimensionX = (mMapPoint3.x - mMapPoint1.x)/(2*mMapSize);
	mCellDimensionY = (mMapPoint4.y - mMapPoint2.y)/(2*mMapSize);

	//	Get the origin point of the map
	wykobi::point2d<float> tBugOrigin = mActualMap->getOriginPoint();

	//	If the game is not started, we set the position of the bug
	//	as the origin position of the map
	if(!mKillBugModel->IsGameStarted()){
		mBugPosition.x = (int)tBugOrigin.x;
		mBugPosition.y = (int)tBugOrigin.y;
	}

	//	We interpolate the the real point (x,y) of the bug
	float tBugPositionX = mMapPoint2.x + (tBugOrigin.x - tBugOrigin.y)*mCellDimensionX;
	float tBugPositionY = mMapPoint2.y + (tBugOrigin.x + tBugOrigin.y + 1)*mCellDimensionY;

	//	Draw everything that is necessary: map border, starting point,
	//	ending point(s) and obstacle(s)
	drawGrid();
	drawOriginPoint(tBugPositionX,tBugPositionY);
	drawEndingPoints();
	drawObstaclePoints();

	// 	Check for feedback map finished, wrong moves, new map,
	//	proportions greater than others, etc...
	drawMapStatusFeedback(); // New or finished map
	drawMoveFeedback();

	//	We set the game as started
	mKillBugModel->SetGameStarted(true);
}

/*
 * Draws the corners and the map border
 */
void decorators::KillBugView::drawGrid(){
	mDecoratorManager.GetDisplay().PushTransformation();

	// Map borders
	mDecoratorManager.GetDisplay().RenderQuad(mMapPoint1.x, mMapPoint1.y, mMapPoint2.x, mMapPoint2.y,
			mMapPoint3.x, mMapPoint3.y, mMapPoint4.x, mMapPoint4.y, 0.0f,0.0f,0.0f,1.0f);

	// Corner 1 - Proportion 1:
	mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth, 0.0f, mWorkingTriangle/2,mWorkingTriangle/2,
			90.0f,180.0f,scProp1R,scProp1G,scProp1B,0.1f,1);
	// Corner 2 - Proportion 2:
	mDecoratorManager.GetDisplay().RenderFilledSector(0.0f, 0.0f, mWorkingTriangle/2,mWorkingTriangle/2,
			90.0f,270.0f,scProp2R,scProp2G,scProp2B,0.1f,1);
	// Corner 3 - Proportion 3:
	mDecoratorManager.GetDisplay().RenderFilledSector(0.0f, mDisplayHeight, mWorkingTriangle/2,mWorkingTriangle/2,
			90.0f,360.0f,scProp3R,scProp3G,scProp3B,0.1f,1);
	// Corner 4 - Proportion 4:
	mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth, mDisplayHeight, mWorkingTriangle/2,mWorkingTriangle/2,
			90.0f,90.0f,scProp4R,scProp4G,scProp4B,0.1f,1); //Prop4

	mDecoratorManager.GetDisplay().PopTransformation();

}

/*
 * Draws the origin point as a red point
 */
void decorators::KillBugView::drawOriginPoint(float tPositionX, float tPositionY){
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderFilledEllipse(tPositionX, tPositionY, mCellDimensionX/6,mCellDimensionY/6,1.0f,0.0f,0.0f,1.0f,1);
	mDecoratorManager.GetDisplay().PopTransformation();
}

/*
 *  Draws the ending points of the map
 */
void decorators::KillBugView::drawEndingPoints(){
	if(mActualMap->getEndNumber() > 0){
		std::vector<wykobi::point2d<float>> tEnd = mActualMap->getEndPoint();
		for(std::vector<wykobi::point2d<float>>::iterator it = tEnd.begin(); it != tEnd.end() ; ++it){
			//	Interpolate the position of the ending points
			float tEndPositionX = mMapPoint2.x + ((*it).x - (*it).y)*mCellDimensionX;
			float tEndPositionY = mMapPoint2.y + ((*it).x + (*it).y + 1)*mCellDimensionY;
			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().RenderFilledEllipse(tEndPositionX, tEndPositionY, mCellDimensionX/2,mCellDimensionY/2,0.0f,0.0f,0.0f,1.0f,1);
			mDecoratorManager.GetDisplay().PopTransformation();
		}
	}
}

/*
 * Draws the obstacles of the map
 */
void decorators::KillBugView::drawObstaclePoints(){
	if(mActualMap->getObstacleNumber() > 0){
		int tObstacleTextureId = mDecoratorManager.GetDisplay().LoadTexture("./activities/proportions-network/obstacle-small.jpg");
		std::vector<wykobi::point2d<float>> tObstacle = mActualMap->getObstaclesPoint();
		for(std::vector<wykobi::point2d<float>>::iterator it = tObstacle.begin(); it != tObstacle.end() ; ++it){
			//	Interpolate the position of the obstacle
			float tPoint1X = mMapPoint2.x + ((*it).x - (*it).y)*mCellDimensionX;
			float tPoint1Y = mMapPoint2.y + ((*it).y + (*it).x)*mCellDimensionY;
			float tPoint2X = mMapPoint2.x + ((*it).x - (*it).y + 1)*mCellDimensionX;
			float tPoint2Y = mMapPoint2.y + ((*it).y + (*it).x + 1)*mCellDimensionY;
			float tPoint3X = mMapPoint2.x + ((*it).x - (*it).y )*mCellDimensionX;
			float tPoint3Y = mMapPoint2.y + ((*it).y + (*it).x + 2)*mCellDimensionY;
			float tPoint4X = mMapPoint2.x + ((*it).x - (*it).y - 1)*mCellDimensionX;
			float tPoint4Y = mMapPoint2.y + ((*it).y + (*it).x + 1)*mCellDimensionY;

			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().RenderTexture(tObstacleTextureId,
					tPoint1X, tPoint1Y,tPoint2X, tPoint2Y,tPoint3X, tPoint3Y,tPoint4X, tPoint4Y);
			mDecoratorManager.GetDisplay().PopTransformation();
		}
	}
}

/*
 * Gives the feedback if the map is finished or is a new map
 */
void decorators::KillBugView::drawMapStatusFeedback(){
	mDecoratorManager.GetDisplay().PushTransformation();
	if(mActualMap->IsFinished()){
		mDecoratorManager.GetDisplay().RenderQuadFilled(0.0f, mDisplayHeight/2 - mMapHeight/4,
				mDisplayWidth, mDisplayHeight/2 - mMapHeight/4, mDisplayWidth, mDisplayHeight/2 + mMapHeight/4,
				0.0f, mDisplayHeight/2 + mMapHeight/4, 0.8f,0.8f,0.8f,0.5f);
		mDecoratorManager.GetDisplay().RenderText("Carte finie!", mMapPoint1.x + 130.0f,  mDisplayHeight/2, 2.2f,0.0f,0.0f,0.0f,1.0f);
	}
	if(!mActualMap->IsFinished() && mKillBugModel->GetNewMapFrames() > 0){
		mDecoratorManager.GetDisplay().RenderQuadFilled(0.0f, mDisplayHeight/2 - mMapHeight/4,
			mDisplayWidth, mDisplayHeight/2 - mMapHeight/4, mDisplayWidth, mDisplayHeight/2 + mMapHeight/4,
			0.0f, mDisplayHeight/2 + mMapHeight/4, 0.8f,0.8f,0.8f,0.5f);
		mDecoratorManager.GetDisplay().RenderText("Carte nouvelle!", mMapPoint1.x + 130.0f, mDisplayHeight/2, 2.2f,0.0f,0.0f,0.0f,1.0f);
		mKillBugModel->DecreaseNewMapFrames();
	}
	mDecoratorManager.GetDisplay().PopTransformation();
}

/*
 * Gives the feedback concerning the move, if one of the proportion is
 * greater, if there is a bad move, etc...
 */
void decorators::KillBugView::drawMoveFeedback(){
	mDecoratorManager.GetDisplay().PushTransformation();

	wykobi::point2d<int> tBugPosition = mKillBugModel->GetBugPosition();


	if(mKillBugModel->GetWrongMovementFrames()>0){
		mDecoratorManager.GetDisplay().RenderText("Je ne peux pas y aller...", mBugPosition.x*mCellDimensionX + mMapPoint1.x + 10.0f,
			mBugPosition.y*mCellDimensionY + mMapPoint1.y - 10.0f, 0.95f,0.0f,0.0f,0.0f,1.0f);
		mKillBugModel->DecreaseWrongMovementFrames();
	}

	if(mKillBugModel->IsProportion1Greater() && mKillBugModel->Get13Frames()>0){
		bool tEven = (mKillBugModel->Get13Frames()%2 == 0);
		mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth, 0.0f, mWorkingTriangle/2,mWorkingTriangle/2,
				90.0f,180.0f,scProp1R,scProp1G,scProp1B,tEven? 0.2:0.09f,1); //Prop1
		mDecoratorManager.GetDisplay().RenderLine(mMapPoint2.x + (tBugPosition.x - tBugPosition.y)*mCellDimensionX, mMapPoint2.y + (tBugPosition.y + tBugPosition.x)*mCellDimensionY + mCellDimensionY,
				mMapPoint2.x + (tBugPosition.x - tBugPosition.y + 2)*mCellDimensionX, mMapPoint2.y + (tBugPosition.y + tBugPosition.x - 2)*mCellDimensionY + mCellDimensionY,
				scProp1R,scProp1G,scProp1B,tEven? 0.6:0.2f);
		mKillBugModel->Decrease13Frames();

	} else if(mKillBugModel->IsProportion3Greater() && mKillBugModel->Get13Frames()>0){
		bool tEven = (mKillBugModel->Get13Frames()%2 == 0);
		mDecoratorManager.GetDisplay().RenderFilledSector(0.0f, mDisplayHeight, mWorkingTriangle/2,mWorkingTriangle/2,
				90.0f,360.0f,scProp3R,scProp3G,scProp3B,tEven? 0.2:0.09f,1); //Prop3
		mDecoratorManager.GetDisplay().RenderLine(mMapPoint2.x + (tBugPosition.x - tBugPosition.y)*mCellDimensionX, mMapPoint2.y + (tBugPosition.y + tBugPosition.x)*mCellDimensionY + mCellDimensionY,
				mMapPoint2.x + (tBugPosition.x - tBugPosition.y - 2)*mCellDimensionX, mMapPoint2.y + (tBugPosition.y + tBugPosition.x + 2)*mCellDimensionY + mCellDimensionY,
				scProp3R,scProp3G,scProp3B,tEven? 0.6:0.2f);
		mKillBugModel->Decrease13Frames();
	}

	if(mKillBugModel->IsProportion2Greater() && mKillBugModel->Get24Frames()>0){
		bool tEven = (mKillBugModel->Get24Frames()%2 == 0);
		mDecoratorManager.GetDisplay().RenderFilledSector(0.0f, 0.0f, mWorkingTriangle/2,mWorkingTriangle/2,
				90.0f,270.0f,scProp2R,scProp2G,scProp2B,tEven? 0.2:0.09f,1); //Prop 2
		mDecoratorManager.GetDisplay().RenderLine(mMapPoint2.x + (tBugPosition.x - tBugPosition.y)*mCellDimensionX, mMapPoint2.y + (tBugPosition.y + tBugPosition.x)*mCellDimensionY + mCellDimensionY,
				mMapPoint2.x + (tBugPosition.x - tBugPosition.y - 2)*mCellDimensionX, mMapPoint2.y + (tBugPosition.y + tBugPosition.x - 2)*mCellDimensionY + mCellDimensionY,
				scProp2R,scProp2G,scProp2B,tEven? 0.6:0.2f);
		mKillBugModel->Decrease24Frames();

	} else if(mKillBugModel->IsProportion4Greater() && mKillBugModel->Get24Frames()>0){
		bool tEven = (mKillBugModel->Get24Frames()%2 == 0);
		mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth, mDisplayHeight, mWorkingTriangle/2,mWorkingTriangle/2,
				90.0f,90.0f,scProp4R,scProp4G,scProp4B,tEven? 0.2:0.09f,1); //Prop4
		mDecoratorManager.GetDisplay().RenderLine(mMapPoint2.x + (tBugPosition.x - tBugPosition.y)*mCellDimensionX, mMapPoint2.y + (tBugPosition.y + tBugPosition.x)*mCellDimensionY + mCellDimensionY,
				mMapPoint2.x + (tBugPosition.x - tBugPosition.y + 2)*mCellDimensionX, mMapPoint2.y + (tBugPosition.y + tBugPosition.x + 2)*mCellDimensionY + mCellDimensionY,
				scProp4R,scProp4G,scProp4B,tEven? 0.6:0.2f);
		mKillBugModel->Decrease24Frames();
	}

	mDecoratorManager.GetDisplay().PopTransformation();
}

/*
 * Display the actual position of the bug
 */
void decorators::KillBugView::displayBug(){

	int tTextureId = mDecoratorManager.GetDisplay().LoadTexture("./activities/proportions-network/ladybug-smallwhite.jpg");
	wykobi::point2d<int> tBugPosition = mKillBugModel->GetBugPosition();
	mBugTrajectory.push_back(tBugPosition);

	//Display the image and also the trajectory
	mDecoratorManager.GetDisplay().PushTransformation();

	for(unsigned int i = 1 ; i < mBugTrajectory.size() ; i++){
		wykobi::point2d<int> tPoint1 = wykobi::make_point(mBugTrajectory[i-1].x,mBugTrajectory[i-1].y);
		wykobi::point2d<int> tPoint2 = wykobi::make_point(mBugTrajectory[i].x,mBugTrajectory[i].y);

		mDecoratorManager.GetDisplay().RenderLine(mMapPoint2.x + (tPoint1.x - tPoint1.y)*mCellDimensionX,mMapPoint2.y + (tPoint1.x + tPoint1.y + 1)*mCellDimensionY,
			mMapPoint2.x + (tPoint2.x - tPoint2.y)*mCellDimensionX, mMapPoint2.y + (tPoint2.x + tPoint2.y + 1)*mCellDimensionY, 0.545f, 0.271f, 0.075f, 0.5f);

		mDecoratorManager.GetDisplay().RenderFilledEllipse(mMapPoint2.x + (tPoint1.x - tPoint1.y)*mCellDimensionX, mMapPoint2.y + (tPoint1.x + tPoint1.y + 1)*mCellDimensionY, mCellDimensionX/6,mCellDimensionY/6,0.545f, 0.271f, 0.075f, 0.5f,1);
	}

	mDecoratorManager.GetDisplay().RenderTexture(tTextureId,
		mMapPoint2.x + (tBugPosition.x - tBugPosition.y)*mCellDimensionX - mCellDimensionX/2, mMapPoint2.y + (tBugPosition.y + tBugPosition.x)*mCellDimensionY + mCellDimensionY/2,
		mMapPoint2.x + (tBugPosition.x - tBugPosition.y + 1)*mCellDimensionX - mCellDimensionX/2, mMapPoint2.y + (tBugPosition.y + tBugPosition.x + 1)*mCellDimensionY - mCellDimensionY/2,
		mMapPoint2.x + (tBugPosition.x - tBugPosition.y )*mCellDimensionX + mCellDimensionX/2, mMapPoint2.y + (tBugPosition.y + tBugPosition.x + 2)*mCellDimensionY - mCellDimensionY/2,
		mMapPoint2.x + (tBugPosition.x - tBugPosition.y - 1)*mCellDimensionX + mCellDimensionX/2, mMapPoint2.y + (tBugPosition.y + tBugPosition.x + 1)*mCellDimensionY + mCellDimensionY/2);

	mDecoratorManager.GetDisplay().PopTransformation();


}

/*
 * Display the hint that corresponds
 */
void decorators::KillBugView::displayHint(){
	// First we get the numerators and denominators of each proportion
	std::vector<int> tNumerator = mKillBugModel->GetProportionNumerator();
	std::vector<int> tDenominator= mKillBugModel->GetProportionDenominator();
	std::vector<float> tProportion= mKillBugModel->GetProportionValue();

	// We check the type of hint that is shown
	switch(mActualHint->GetHintType()){
		case 0: displayDiscreteHint(tNumerator,tDenominator);	// Discrete help
			break;
		case 1: displayFractionHint(tNumerator,tDenominator);	// Fraction help
			break;
		case 2: displayDecimalHint(tProportion);	// Decimal help
			break;
		case 3: displayRectangularHint(tProportion);	// Rectangular help
			break;
		case 4: displayCircularHint(tProportion);	// Circular help
			break;
	}
}

/*
 * Display the discrete representation for all the fractions
 */
void decorators::KillBugView::displayDiscreteHint(std::vector<int> pNumerator, std::vector<int> pDenominator){
	for(int i = 0 ; i < 4 ; i++){
		int tNum = pNumerator[i];
		int tDenom = pDenominator[i];
		displayIndividualDiscreteHint(tNum,tDenom, i+1);
	}
}

/*
 * Display the fraction representation of all the fractions
 */
void decorators::KillBugView::displayFractionHint(std::vector<int> pNumerator, std::vector<int> pDenominator){
	for(int i = 0 ; i < 4 ; i++){
		int tNum = pNumerator[i];
		int tDenom = pDenominator[i];
		displayIndividualFractionHint(tNum,tDenom, i+1);
	}
}

/*
 * Display the decimal representation of all the fractions
 */
void decorators::KillBugView::displayDecimalHint(std::vector<float> pProportion){
	for(int i = 0 ; i < 4 ; i++){
		float tProportion = pProportion[i];
		displayIndividualDecimalHint(tProportion, i+1);
	}
}

/*
 * Display the rectangular representation of all the fractions
 */
void decorators::KillBugView::displayRectangularHint(std::vector<float> pProportion){
	for(int i = 0 ; i < 4 ; i++){
		float tProportion = pProportion[i];
		displayIndividualRectangularHint(tProportion, i+1);
	}
}

/*
 * Display the circular representation of all the fractions
 */
void decorators::KillBugView::displayCircularHint(std::vector<float> pProportion){
	for(int i = 0 ; i < 4 ; i++){
		float tProportion = pProportion[i];
		displayIndividualCircularHint(tProportion, i+1);
	}
}

/*
 * Display the discrete hint of one proportion
 */
void decorators::KillBugView::displayIndividualDiscreteHint(int pNumerator, int pDenominator, int pProportionNumber){

	//Define some parameters
	float tBoxWidth = mWorkingTriangle/12; // Width of the whole representation
	float tBoxHeight = tBoxWidth/1.5; // Height of the whole representation

	float tCellWidth = tBoxWidth/9; // Width of a small square
	float tCellHeight = tBoxHeight/7; // Width of a small square

	int tDenom = pDenominator;
	int tNum = pNumerator;

	float tPartialX, tPartialY;


	// Depending of the proportion number (1,2,3 or 4) we have different Y
	switch(pProportionNumber){
		case 1:		tPartialY = mProportion1Point.y - tBoxHeight/2;	break;
		case 2:		tPartialY = mProportion2Point.y - tBoxHeight/2;	break;
		case 3:		tPartialY = mProportion3Point.y - tBoxHeight/2;	break;
		case 4:		tPartialY = mProportion4Point.y - tBoxHeight/2;	break;
	}

	// Display the cells
	mDecoratorManager.GetDisplay().PushTransformation();
	for(int i = 0 ; i < 4 ; i++){
		// Depending of the proportion number (1,2,3 or 4) we have different X (we have to start with the same
		// x in every row)
			switch(pProportionNumber){
				case 1:		tPartialX = mProportion1Point.x - tBoxWidth/2;
					break;
				case 2:		tPartialX = mProportion2Point.x - tBoxWidth/2;
					break;
				case 3:		tPartialX = mProportion3Point.x - tBoxWidth/2;
					break;
				case 4:		tPartialX = mProportion4Point.x - tBoxWidth/2;
					break;
			}
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
 * Display the fraction hint of one proportion
 */
void decorators::KillBugView::displayIndividualFractionHint(int pNumerator, int pDenominator, int pProportionNumber){
	//	Some variables
	int tPosX, tPosY;
	char tProportionNumerator[3];
	char tProportionDenominator[3];

	//	Depending of the proportion number (1,2,3 or 4) we have different X and Y
	switch(pProportionNumber){
		case 1:		tPosX = mProportion1Point.x; 	tPosY = mProportion1Point.y;
			break;
		case 2:		tPosX = mProportion2Point.x; tPosY = mProportion2Point.y;
			break;
		case 3:		tPosX = mProportion3Point.x; tPosY = mProportion3Point.y;
			break;
		case 4:		tPosX = mProportion4Point.x; tPosY = mProportion4Point.y;
			break;
	}

	//	We save in the char* the numerator and denominator
	sprintf(tProportionNumerator, "%d", pNumerator);
	sprintf(tProportionDenominator, "%d",pDenominator);

	//	Display the representation
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportionNumerator,tPosX,tPosY - 15.0f,true,1.0f,0.0f,0.0f,0.0f,1.0f); // Numerator
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportionDenominator,tPosX,tPosY + 15.0f,true,1.0f,0.0f,0.0f,0.0f,1.0f); // Denominator

	mDecoratorManager.GetDisplay().RenderLine(tPosX-15,tPosY,tPosX+15,tPosY,0.0f,0.0f,0.0f,1.0f);	//	Fraction line
	mDecoratorManager.GetDisplay().PopTransformation();
}

/*
 * Display the decimal hint of one proportion
 */
void decorators::KillBugView::displayIndividualDecimalHint(float pProportion, int pProportionNumber){
	// Some variables
	int tPosX, tPosY;
	char tProportionText[3];

	switch(pProportionNumber){
		case 1:		tPosX = mProportion1Point.x; 	tPosY = mProportion1Point.y;
			break;
		case 2:		tPosX = mProportion2Point.x; tPosY = mProportion2Point.y;
			break;
		case 3:		tPosX = mProportion3Point.x; tPosY = mProportion3Point.y;
			break;
		case 4:		tPosX = mProportion4Point.x; tPosY = mProportion4Point.y;
			break;
	}

	//	Save the proportion, with 2 decimals
	sprintf(tProportionText, "%3.2f", pProportion);

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportionText, tPosX,tPosY,true,1.0f,0.0f,0.0f,0.0f,1.0f);	//	Render the value
	mDecoratorManager.GetDisplay().PopTransformation();
}

/*
 * Display the rectangular hint of one proportion
 */
void decorators::KillBugView::displayIndividualRectangularHint(float pProportion, int pProportionNumber){
	float tRecWidth = mWorkingTriangle/12;
	float tRecHeight = tRecWidth/2;

	int tPosX, tPosY;
	float tR, tG, tB;

	switch(pProportionNumber){
	case 1:
		tPosX = mProportion1Point.x; 	tPosY = mProportion1Point.y;
		tR = scProp1R; 	tG = scProp1G; 	tB = scProp1B;
		break;
	case 2:
		tPosX = mProportion2Point.x; tPosY = mProportion2Point.y;
		tR = scProp2R; tG = scProp2G; tB = scProp2B;
		break;
	case 3:
		tPosX = mProportion3Point.x; tPosY = mProportion3Point.y;
		tR = scProp3R; tG = scProp3G; tB = scProp3B;
		break;
	case 4:
		tPosX = mProportion4Point.x; tPosY = mProportion4Point.y;
		tR = scProp4R; tG = scProp4G; tB = scProp4B;
		break;
	}

	mDecoratorManager.GetDisplay().PushTransformation();

	mDecoratorManager.GetDisplay().RenderQuadFilled(tPosX-tRecWidth/2, tPosY - tRecHeight/2,
					pProportion*tRecWidth +  tPosX-tRecWidth/2, tPosY - tRecHeight/2,
					pProportion*tRecWidth +  tPosX-tRecWidth/2, tPosY + tRecHeight/2,
					tPosX - tRecWidth/2 , tPosY + tRecHeight/2, tR,tG,tB,1.0f);

	mDecoratorManager.GetDisplay().RenderQuad(tPosX-tRecWidth/2, tPosY - tRecHeight/2,
			tPosX + tRecWidth/2 , tPosY - tRecHeight/2,
			tPosX + tRecWidth/2 , tPosY + tRecHeight/2,
			tPosX - tRecWidth/2 , tPosY + tRecHeight/2, 0.0f,0.0f,0.0f,1.0f);

		mDecoratorManager.GetDisplay().PopTransformation();
}

/*
 * Display the circular hint of one proportion
 */
void decorators::KillBugView::displayIndividualCircularHint(float pProportion, int pProportionNumber){
	// The parameter
	float tRadius = mWorkingTriangle/18;
	int tPosX, tPosY;
	float tR, tG, tB;

	switch(pProportionNumber){
	case 1:
		tPosX = mProportion1Point.x; 	tPosY = mProportion1Point.y;
		tR = scProp1R; 	tG = scProp1G; 	tB = scProp1B;
		break;
	case 2:
		tPosX = mProportion2Point.x; tPosY = mProportion2Point.y;
		tR = scProp2R; tG = scProp2G; tB = scProp2B;
		break;
	case 3:
		tPosX = mProportion3Point.x; tPosY = mProportion3Point.y;
		tR = scProp3R; tG = scProp3G; tB = scProp3B;
		break;
	case 4:
		tPosX = mProportion4Point.x; tPosY = mProportion4Point.y;
		tR = scProp4R; tG = scProp4G; tB = scProp4B;
		break;
	}

	mDecoratorManager.GetDisplay().PushTransformation();

	mDecoratorManager.GetDisplay().RenderFilledSector(tPosX,tPosY,tRadius,tRadius,pProportion*360,90.0f - pProportion*360,tR,tG,tB,1.0f,1);
	mDecoratorManager.GetDisplay().RenderEllipse(tPosX,tPosY,tRadius,tRadius,0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().PopTransformation();
}

/*
 * Displays feedback in the flipper
 */
void decorators::KillBugView::DisplayFlipperFeedback(){
	char tStepsDone[3] , tStepsToGo[3];		// Steps information
	char tP1Num[3] , tP2Num[3] , tP3Num[3] , tP4Num[3];		//	Numerator information
	char tP1Den[3] , tP2Den[3] , tP3Den[3] , tP4Den[3];		//	Denominator information

	static const long cShotPreparationTime = 6l*1000l;
	//long tElapsedTime = Time::MillisTimestamp() - mLastShot;
	long tElapsedTime = Time::MillisTimestamp() - mKillBugModel->GetLastShot();

		// If the time is more than the animation, then make a move and set the time
		// as the last shot done
		//if (mActualFlipper->IsFlipped() && tElapsedTime > cShotPreparationTime) {
			//MakeMove();
			//mLastShot = Time::MillisTimestamp();
		//}

		// If is present, we calculate the proportion of time that has passed
		// and we represent it as a degree (to be drawn in the circumference
		if (mActualFlipper->IsPresent() && mActualFlipper->GetCurrentSide() != NULL){
			float tPartialDegree = 360*(tElapsedTime/(float)cShotPreparationTime);
			bool tFull = false;
			if(tPartialDegree >= 360)
			{
				tPartialDegree = 360;
				tFull = true;
			}

			sprintf(tStepsDone, "%d", mKillBugModel->StepsDone());
			sprintf(tStepsToGo, "%d", mActualMap->GetStepsToGo(mKillBugModel->GetBugPosition().x,mKillBugModel->GetBugPosition().y));
			sprintf(tP1Num, "%d", mKillBugModel->GetProportionNumerator()[0]);
			sprintf(tP1Den, "%d", mKillBugModel->GetProportionDenominator()[0]);
			sprintf(tP2Num, "%d", mKillBugModel->GetProportionNumerator()[1]);
			sprintf(tP2Den, "%d", mKillBugModel->GetProportionDenominator()[1]);
			sprintf(tP3Num, "%d", mKillBugModel->GetProportionNumerator()[2]);
			sprintf(tP3Den, "%d", mKillBugModel->GetProportionDenominator()[2]);
			sprintf(tP4Num, "%d", mKillBugModel->GetProportionNumerator()[3]);
			sprintf(tP4Den, "%d", mKillBugModel->GetProportionDenominator()[3]);

			// Display the sector of the circunference and then the text
			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(*mActualFlipper->GetCurrentSide(),
				mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
			mDecoratorManager.GetDisplay().RenderFilledSector(1.0f,3.3f,1.5f,1.5f,
				tPartialDegree,0.0f,0.0f,tFull? 1.0 : 0.0f,tFull ? 0.0 : 1.0f,0.8f,1);

			mDecoratorManager.GetDisplay().RenderCenteredText(tFull?"Prêt!" :"En repos ...", 0.5f,5.5f,
				true,0.03f, 0.0f, tFull? 1.0f : 0.0f, tFull? 0.0f : 1.0f, 1.0f);

			if(mActualFlipper->GetType() == 1 && !tFull){ //If is content flipper then we display more feedback
				mDecoratorManager.GetDisplay().RenderQuadFilled(-1.5f,0.0f,-2.5f,-1.5f,-2.5f,1.5f,-1.5f,0.0f,0.896f,0.896f,0.896f,0.9);
				mDecoratorManager.GetDisplay().RenderQuadFilled(-10.5f,-1.5f,-2.5f,-1.5f,-2.5f,8.5f,-10.5f,8.5f,0.896f,0.896f,0.896f,0.9);

				if(!mKillBugModel->IsWrongMove()){
					mDecoratorManager.GetDisplay().RenderCenteredText("Well Done!", -6.0f,0.0f,
								true,0.05f, 0.0f,  0.0f, 0.0f , 1.0f);
					mDecoratorManager.GetDisplay().RenderText("I've done", -9.5f,2.5f,0.05f, 0.0f,  0.0f, 0.0f , 1.0f);
					mDecoratorManager.GetDisplay().RenderText(tStepsDone, -9.5f,4.0f,0.05,0.0f,0.0f,0.0f,1.0f);
					mDecoratorManager.GetDisplay().RenderText(" steps...", -8.5f,4.0f,0.05,0.0f,0.0f,0.0f,1.0f);
					mDecoratorManager.GetDisplay().RenderText(tStepsToGo, -9.5f,5.5f,0.05,0.0f,0.0f,0.0f,1.0f);
					mDecoratorManager.GetDisplay().RenderText(" more", -8.0f,5.5f,0.05,0.0f,0.0f,0.0f,1.0f);
					mDecoratorManager.GetDisplay().RenderText("to finish", -9.5f,7.0f,0.05,0.0f,0.0f,0.0f,1.0f);

				}else{
					mDecoratorManager.GetDisplay().RenderCenteredText("Oh no!", -6.0f,0.0f, true,0.05f, 0.0f,  0.0f, 0.0f , 1.0f);
					mDecoratorManager.GetDisplay().RenderText("Remember:", -10.0f,2.5f,0.05f, 0.0f,  0.0f, 0.0f , 1.0f);
					mDecoratorManager.GetDisplay().RenderText(tP1Num, -8.5f,4.0f,0.05,0.0f,0.0f,0.0f,1.0f);
					mDecoratorManager.GetDisplay().RenderText(tP3Num, -5.5f,4.0f,0.05,0.0f,0.0f,0.0f,1.0f);
					mDecoratorManager.GetDisplay().RenderText((mKillBugModel->IsProportion1Greater())? ">" : (mKillBugModel->IsProportion3Greater())? "<" : "=", -7.5f,5.0f,0.08,0.0f,0.0f,0.0f,1.0f);
					mDecoratorManager.GetDisplay().RenderText(tP1Den, -8.5f,5.5f,0.05,0.0f,0.0f,0.0f,1.0f);
					mDecoratorManager.GetDisplay().RenderText(tP3Den, -5.5f,5.5f,0.05,0.0f,0.0f,0.0f,1.0f);
					mDecoratorManager.GetDisplay().RenderText(tP2Num, -8.5f,7.0f,0.05,0.0f,0.0f,0.0f,1.0f);
					mDecoratorManager.GetDisplay().RenderText(tP4Num, -5.5f,7.0f,0.05,0.0f,0.0f,0.0f,1.0f);
					mDecoratorManager.GetDisplay().RenderText((mKillBugModel->IsProportion2Greater())? ">" : (mKillBugModel->IsProportion4Greater())? "<" : "=", -7.5f,8.0f,0.08,0.0f,0.0f,0.0f,1.0f);
					mDecoratorManager.GetDisplay().RenderText(tP2Den, -8.5f,8.5f,0.05,0.0f,0.0f,0.0f,1.0f);
					mDecoratorManager.GetDisplay().RenderText(tP4Den, -5.5f,8.5f,0.05,0.0f,0.0f,0.0f,1.0f);
					mDecoratorManager.GetDisplay().RenderLine(-8.5f,4.45f,-7.3f,4.5,0.0f,0.0f,0.0f,1.0f);
					mDecoratorManager.GetDisplay().RenderLine(-5.5f,4.45f,-4.3f,4.5,0.0f,0.0f,0.0f,1.0f);
					mDecoratorManager.GetDisplay().RenderLine(-8.5f,7.45f,-7.3f,7.5,0.0f,0.0f,0.0f,1.0f);
					mDecoratorManager.GetDisplay().RenderLine(-5.5f,7.45f,-4.3f,7.5,0.0f,0.0f,0.0f,1.0f);

				}
			}
			mDecoratorManager.GetDisplay().PopTransformation();
		}
}
