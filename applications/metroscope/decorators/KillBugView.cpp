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

decorators::KillBugView::KillBugView(DecoratorManager &pDecoratorManager,KillBugModel *pKillBugModel):
FiducialDecorator(pDecoratorManager, 0),
mKillBugModel(pKillBugModel), mBugTrayectory(),mActualMap()
{
	mDisplayWidth = mDecoratorManager.GetDisplay().GetWidth();
	mDisplayHeight = mDecoratorManager.GetDisplay().GetHeight();
	mWorkingTriangle = (0.90*mDisplayHeight);
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

	mGameStarted = false;
	mProportion1Greater = false;
	mProportion2Greater = false;
	mProportion3Greater = false;
	mProportion4Greater = false;
	mMapFinished = false;
	mMapNew = false;
	mProportionFeedbackFrames13 = 15; //CHECK
	mProportionFeedbackFrames24 = 15; //CHECK
	mProportion1Numerator = 0;
	mProportion1Denominator = 1;
	mProportion2Numerator = 0;
	mProportion2Denominator = 1;
	mProportion3Numerator = 0;
	mProportion3Denominator = 1;
	mProportion4Numerator = 0;
	mProportion4Denominator = 1;
	mSteps = 0;
	mProportion1 = 0.0f;
	mProportion2 = 0.0f;
	mProportion3 = 0.0f;
	mProportion4 = 0.0f;
	mWrongMovementFrames = 15; //CHECK
	mNewMapFrames = 15; //CHECK
	mLastShot = 0.0f;
	mMapSize = 0;
	mCellDimensionX = 0.0f;
	mCellDimensionY = 0.0f;

}

decorators::KillBugView::~KillBugView(){

}

void decorators::KillBugView::update() {
	if(mKillBugModel->isMapPresent()){
		mActualMap = mKillBugModel->getActualMap();
		displayMap();
		if(mKillBugModel->isGameStarted()){
			displayBug();
		}
		if(mKillBugModel->isHintPresent()){
			mActualHint = mKillBugModel->getActualHint();
			displayHint();
		}
	}
}

void decorators::KillBugView::displayMap(){

	mMapSize = mKillBugModel->getActualMap()->getSize();

	mCellDimensionX = (mMapPoint3.x - mMapPoint1.x)/(2*mMapSize);
	mCellDimensionY = (mMapPoint4.y - mMapPoint2.y)/(2*mMapSize);

	wykobi::point2d<float> tBugOrigin = mActualMap->getOriginPoint();

	if(!mKillBugModel->isGameStarted()){
		mBugPosition.x = (int)tBugOrigin.x;
		mBugPosition.y = (int)tBugOrigin.y;
	}

	float tBugPositionX = mMapPoint2.x + (tBugOrigin.x - tBugOrigin.y)*mCellDimensionX;
	float tBugPositionY = mMapPoint2.y + (tBugOrigin.x + tBugOrigin.y + 1)*mCellDimensionY;

	// Draw everything that is necessary: map border, starting point, ending point(s) and obstacle(s)
	drawGrid();
	drawOriginPoint(tBugPositionX,tBugPositionY);
	drawEndingPoints();
	drawObstaclePoints();

	// Check for feedback map finished, wrong moves, new map, proportions greater than others, etc...
	drawMapStatusFeedback(); // New or finished map
	drawMoveFeedback();

	mKillBugModel->setGameStarted(true);
}

/*
 * Draws the corners and the map border
 */
void decorators::KillBugView::drawGrid(){
	mDecoratorManager.GetDisplay().PushTransformation();

	// Map borders
	mDecoratorManager.GetDisplay().RenderQuad(mMapPoint1.x, mMapPoint1.y, mMapPoint2.x, mMapPoint2.y,
			mMapPoint3.x, mMapPoint3.y, mMapPoint4.x, mMapPoint4.y, 0.0f,0.0f,0.0f,1.0f);

	// Corner 1:
	mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth, 0.0f, mWorkingTriangle/2,mWorkingTriangle/2,
			90.0f,180.0f,scProp1R,scProp1G,scProp1B,0.1f,1);
	// Corner 2:
	mDecoratorManager.GetDisplay().RenderFilledSector(0.0f, 0.0f, mWorkingTriangle/2,mWorkingTriangle/2,
			90.0f,270.0f,scProp2R,scProp2G,scProp2B,0.1f,1);
	// Corner 3:
	mDecoratorManager.GetDisplay().RenderFilledSector(0.0f, mDisplayHeight, mWorkingTriangle/2,mWorkingTriangle/2,
			90.0f,360.0f,scProp3R,scProp3G,scProp3B,0.1f,1);
	// Corner 4:
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
 * Gives the feedback if the map is finished
 */
void decorators::KillBugView::drawMapStatusFeedback(){
	mDecoratorManager.GetDisplay().PushTransformation();
	if(mActualMap->IsFinished()){
		mDecoratorManager.GetDisplay().RenderQuadFilled(mMapPoint1.x, mDisplayHeight/2 - mMapHeight/4,
			mMapPoint2.x, mDisplayHeight/2 - mMapHeight/4, mMapPoint2.x, mDisplayHeight/2 + mMapHeight/4,
			mMapPoint1.x, mDisplayHeight/2 + mMapHeight/4, 0.8f,0.8f,0.8f,0.5f);
		mDecoratorManager.GetDisplay().RenderText("Carte finie!", mMapPoint1.x,  mDisplayHeight/2, 1.2f,0.0f,0.0f,0.0f,1.0f);
	}
	if(mKillBugModel->getNewMapFrames() > 0){
		mDecoratorManager.GetDisplay().RenderQuadFilled(mMapPoint1.x, mDisplayHeight/2 - mMapHeight/4,
			mMapPoint2.x, mDisplayHeight/2 - mMapHeight/4, mMapPoint2.x, mDisplayHeight/2 + mMapHeight/4,
			mMapPoint1.x, mDisplayHeight/2 + mMapHeight/4, 0.8f,0.8f,0.8f,0.5f);
		mDecoratorManager.GetDisplay().RenderText("Carte nouvelle!", mMapPoint1.x, mDisplayHeight/2, 1.2f,0.0f,0.0f,0.0f,1.0f);
		mKillBugModel->decreaseNewMapFrames();
	}
	mDecoratorManager.GetDisplay().PopTransformation();
}

/*
 * Gives the feedback concerning the move, if one of the proportion is
 * greater, if there is a bad move, etc...
 */
void decorators::KillBugView::drawMoveFeedback(){
	mDecoratorManager.GetDisplay().PushTransformation();

	if(mKillBugModel->getWrongMovementFrames()>0){
		mDecoratorManager.GetDisplay().RenderText("Je ne peux pas y aller...", mBugPosition.x*mCellDimensionX + mMapPoint1.x + 10.0f,
			mBugPosition.y*mCellDimensionY + mMapPoint1.y - 10.0f, 0.95f,0.0f,0.0f,0.0f,1.0f);
		mKillBugModel->decreaseWrongMovementFrames();
	}

	if(mKillBugModel->isProportion1Greater() && mKillBugModel->get13Frames()>0){
		bool tEven = (mKillBugModel->get13Frames()%2 == 0);
		mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth, 0.0f, mWorkingTriangle/2,mWorkingTriangle/2,
				90.0f,180.0f,scProp1R,scProp1G,scProp1B,tEven? 0.2:0.09f,1); //Prop1
		mKillBugModel->decrease13Frames();

	} else if(mKillBugModel->isProportion3Greater() && mKillBugModel->get13Frames()>0){
		bool tEven = (mKillBugModel->get13Frames()%2 == 0);
		mDecoratorManager.GetDisplay().RenderFilledSector(0.0f, mDisplayHeight, mWorkingTriangle/2,mWorkingTriangle/2,
				90.0f,360.0f,scProp3R,scProp3G,scProp3B,tEven? 0.2:0.09f,1); //Prop3
		mKillBugModel->decrease13Frames();
	}

	if(mKillBugModel->isProportion2Greater() && mKillBugModel->get24Frames()>0){
		bool tEven = (mKillBugModel->get24Frames()%2 == 0);
		mDecoratorManager.GetDisplay().RenderFilledSector(0.0f, 0.0f, mWorkingTriangle/2,mWorkingTriangle/2,
				90.0f,270.0f,scProp2R,scProp2G,scProp2B,tEven? 0.2:0.09f,1); //Prop 2
		mKillBugModel->decrease24Frames();

	} else if(mKillBugModel->isProportion4Greater() && mKillBugModel->get24Frames()>0){
		bool tEven = (mKillBugModel->get24Frames()%2 == 0);
		mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth, mDisplayHeight, mWorkingTriangle/2,mWorkingTriangle/2,
				90.0f,90.0f,scProp4R,scProp4G,scProp4B,tEven? 0.2:0.09f,1); //Prop4
		mKillBugModel->decrease24Frames();
	}

	mDecoratorManager.GetDisplay().PopTransformation();
}

/*
 * Display the actual position of the bug
 */
void decorators::KillBugView::displayBug(){

	int tTextureId = mDecoratorManager.GetDisplay().LoadTexture("./activities/proportions-network/ladybug-smallwhite.jpg");
	wykobi::point2d<int> tBugPosition = mKillBugModel->getBugPosition();

	//Display the image
	mDecoratorManager.GetDisplay().PushTransformation();


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
	std::vector<int> tNumerator = mKillBugModel->getProportionNumerator();
	std::vector<int> tDenominator= mKillBugModel->getProportionDenominator();
	std::vector<float> tProportion= mKillBugModel->getProportionValue();

	// We check the type of hint that is shown
	switch(mActualHint->GetHintType()){

	case 0: // Discrete help
		displayDiscreteHint(tNumerator,tDenominator);
		break;

	case 1: // Fraction help
		displayFractionHint(tNumerator,tDenominator);
		break;

	case 2: // Decimal help
		displayDecimalHint(tProportion);
		break;

	case 3: // Rectangular help
		displayRectangularHint(tProportion);
		break;

	case 4: // Circular help
		displayCircularHint(tProportion);
		break;
	}
}

void decorators::KillBugView::displayDiscreteHint(std::vector<int> pNumerator, std::vector<int> pDenominator){
	for(int i = 0 ; i < 4 ; i++){
		int tNum = pNumerator[i];
		int tDenom = pDenominator[i];
		displayIndividualDiscreteHint(tNum,tDenom, i+1);
	}
}

void decorators::KillBugView::displayFractionHint(std::vector<int> pNumerator, std::vector<int> pDenominator){
	for(int i = 0 ; i < 4 ; i++){
		int tNum = pNumerator[i];
		int tDenom = pDenominator[i];
		displayIndividualFractionHint(tNum,tDenom, i+1);
	}
}

void decorators::KillBugView::displayDecimalHint(std::vector<float> pProportion){
	for(int i = 0 ; i < 4 ; i++){
		float tProportion = pProportion[i];
		displayIndividualDecimalHint(tProportion, i+1);
	}
}

void decorators::KillBugView::displayRectangularHint(std::vector<float> pProportion){
	for(int i = 0 ; i < 4 ; i++){
		float tProportion = pProportion[i];
		displayIndividualRectangularHint(tProportion, i+1);
	}
}

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

	// Depending of the proportion number (1,2,3 or 4) we have different X and Y
	switch(pProportionNumber){
	case 1:		tPartialX = mProportion1Point.x - tBoxWidth/2;	tPartialY = mProportion1Point.y - tBoxHeight/2;
		break;
	case 2:		tPartialX = mProportion2Point.x - tBoxWidth/2;	tPartialY = mProportion2Point.y - tBoxHeight/2;
		break;
	case 3:		tPartialX = mProportion3Point.x - tBoxWidth/2;	tPartialY = mProportion3Point.y - tBoxHeight/2;
		break;
	case 4:		tPartialX = mProportion4Point.x - tBoxWidth/2;	tPartialY = mProportion4Point.y - tBoxHeight/2;
		break;
	}

	mDecoratorManager.GetDisplay().PushTransformation();
	for(int i = 0 ; i < 4 ; i++){
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
	int tPosX, tPosY;

	switch(pProportionNumber){
	case 1:
		tPosX = mProportion1Point.x; 	tPosY = mProportion1Point.y;
		break;
	case 2:
		tPosX = mProportion2Point.x; tPosY = mProportion2Point.y;
		break;
	case 3:
		tPosX = mProportion3Point.x; tPosY = mProportion3Point.y;
		break;
	case 4:
		tPosX = mProportion4Point.x; tPosY = mProportion4Point.y;
		break;
	}

	char tProportionNumerator[3];
	char tProportionDenominator[3];

	sprintf(tProportionNumerator, "%d", pNumerator);
	sprintf(tProportionDenominator, "%d",pDenominator);

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportionNumerator,tPosX,tPosY - 15.0f,true,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportionDenominator,tPosX,tPosY + 15.0f,true,1.0f,0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().RenderLine(tPosX-15,tPosY,tPosX+15,tPosY,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().PopTransformation();
}

/*
 * Display the decimal hint of one proportion
 */
void decorators::KillBugView::displayIndividualDecimalHint(float pProportion, int pProportionNumber){

	int tPosX, tPosY;

	switch(pProportionNumber){
	case 1:
		tPosX = mProportion1Point.x; 	tPosY = mProportion1Point.y;
		break;
	case 2:
		tPosX = mProportion2Point.x; tPosY = mProportion2Point.y;
		break;
	case 3:
		tPosX = mProportion3Point.x; tPosY = mProportion3Point.y;
		break;
	case 4:
		tPosX = mProportion4Point.x; tPosY = mProportion4Point.y;
		break;
	}

	char tProportionText[3];
	sprintf(tProportionText, "%3.2f", pProportion);

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportionText, tPosX,tPosY,true,1.0f,0.0f,0.0f,0.0f,1.0f);
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
