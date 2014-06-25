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
		return new decorators::KillBugView(pDecoratorManager);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::KillBugView::KillBugView(DecoratorManager &pDecoratorManager):
FiducialDecorator(pDecoratorManager, 0)
{

}

decorators::KillBugView::~KillBugView(){
	mDisplayWidth = mDecoratorManager.GetDisplay().GetWidth();
	mDisplayHeight = mDecoratorManager.GetDisplay().GetHeight();
	mWorkingTriangle = (0.90*mDisplayHeight);
	mMapWidth = mDisplayWidth - mWorkingTriangle;
	mMapHeight = mMapWidth;
	mBugPosition.x = 0;
	mBugPosition.y = 0;

	mMapPoint1.x = mWorkingTriangle/4;
	mMapPoint1.y = mDisplayHeight/2;
	mMapPoint2.x = mDisplayWidth/2;
	mMapPoint2.y = mWorkingTriangle/4 + mDisplayHeight/2 - mDisplayWidth/2;
	mMapPoint3.x = mDisplayWidth-mWorkingTriangle/4;
	mMapPoint3.y = mDisplayHeight/2;
	mMapPoint4.x = mDisplayWidth/2;
	mMapPoint4.y = mDisplayHeight/2 - mWorkingTriangle/4 + mDisplayWidth/2;

	mProportion1Point.x = mDisplayWidth/2 ;
	mProportion1Point.y = mMapPoint1.y - mWorkingTriangle/12;
	mProportion2Point.x = mMapPoint1.x - mWorkingTriangle/12;
	mProportion2Point.y = mDisplayHeight/2;
	mProportion3Point.x = mDisplayWidth/2;
	mProportion3Point.y = mMapPoint3.y + mWorkingTriangle/12;
	mProportion4Point.x = mMapPoint3.x + mWorkingTriangle/12;
	mProportion4Point.y = mDisplayHeight/2;
}

void decorators::KillBugView::update() {

}

void decorators::KillBugView::DisplayMap(int pMapSize, wykobi::point2d<float> pBugOrigin, bool pGameStarted, bool pMapFinished, bool pNewMap){
	mMapSize = pMapSize;
	mCellDimension.x = (mMapPoint3.x - mMapPoint1.x)/(2*mMapSize);
	mCellDimension.y = (mMapPoint4.y - mMapPoint2.y)/(2*mMapSize);
	wykobi::point2d<float> tBugOrigin = pBugOrigin;

	if(!pGameStarted){
		mBugPosition.x = (int)pBugOrigin.x;
		mBugPosition.y = (int)pBugOrigin.y;
	}

	float tBugPositionX = mMapPoint2.x + (pBugOrigin.x - pBugOrigin.y)*mCellDimension.x;
	float tBugPositionY = mMapPoint2.y + (pBugOrigin.x + pBugOrigin.y + 1)*mCellDimension.y;

	mDecoratorManager.GetDisplay().PushTransformation();

		mDecoratorManager.GetDisplay().RenderQuadFilled(mMapPoint1.x, mMapPoint1.y, mMapPoint2.x, mMapPoint2.y,
			mMapPoint3.x, mMapPoint3.y, mMapPoint4.x, mMapPoint4.y, 1.0f,1.0f,1.0f,1.0f);

		mDecoratorManager.GetDisplay().RenderQuad(mMapPoint1.x, mMapPoint1.y, mMapPoint2.x, mMapPoint2.y, mMapPoint3.x, mMapPoint3.y, mMapPoint4.x, mMapPoint4.y, 0.0f,0.0f,0.0f,1.0f);
		mDecoratorManager.GetDisplay().RenderFilledSector(0.0f, 0.0f, mWorkingTriangle/2,mWorkingTriangle/2,90.0f,270.0f,scProp2R,scProp2G,scProp2B,0.1f,1); //Prop 2
		mDecoratorManager.GetDisplay().RenderFilledSector(0.0f, mDisplayHeight, mWorkingTriangle/2,mWorkingTriangle/2,90.0f,360.0f,scProp3R,scProp3G,scProp3B,0.1f,1); //Prop3
		mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth, 0.0f, mWorkingTriangle/2,mWorkingTriangle/2,90.0f,180.0f,scProp1R,scProp1G,scProp1B,0.1f,1); //Prop1
		mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth, mDisplayHeight, mWorkingTriangle/2,mWorkingTriangle/2,90.0f,90.0f,scProp4R,scProp4G,scProp4B,0.1f,1); //Prop4

	mDecoratorManager.GetDisplay().PopTransformation();

}


/*
 *
 * 		float mProportion1,mProportion2,mProportion3,mProportion4;

		wykobi::point2d<float> mCellDimension;
		float mMapWidth, mMapHeight;
		wykobi::point2d<int> mBugPosition;
		int mSteps;
		bool mGameStarted;
		bool mMapFinished;
		bool mMapNew;
		wykobi::point2d<int> mMapPoint1, mMapPoint2, mMapPoint3, mMapPoint4;
		wykobi::point2d<int> mProportion1Point, mProportion2Point, mProportion3Point, mProportion4Point;
		int mProportion1Numerator, mProportion1Denominator;
		int mProportion2Numerator, mProportion2Denominator;
		int mProportion3Numerator, mProportion3Denominator;
		int mProportion4Numerator, mProportion4Denominator;
		int mWrongMovementFrames;
		int mNewMapFrames;
		int mProportionFeedbackFrames13, mProportionFeedbackFrames24;
		bool mProportion1Greater, mProportion2Greater, mProportion3Greater, mProportion4Greater;
		std::vector<wykobi::point2d<int>> mBugTrayectory;



	}


	//Grid
	mDecoratorManager.GetDisplay().PushTransformation();

	mDecoratorManager.GetDisplay().RenderQuadFilled(mMapPoint1.x, mMapPoint1.y, mMapPoint2.x, mMapPoint2.y,
		mMapPoint3.x, mMapPoint3.y, mMapPoint4.x, mMapPoint4.y, 1.0f,1.0f,1.0f,1.0f);

	mDecoratorManager.GetDisplay().RenderQuad(mMapPoint1.x, mMapPoint1.y, mMapPoint2.x, mMapPoint2.y, mMapPoint3.x, mMapPoint3.y, mMapPoint4.x, mMapPoint4.y, 0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderFilledSector(0.0f, 0.0f, mWorkingTriangle/2,mWorkingTriangle/2,90.0f,270.0f,scProp2R,scProp2G,scProp2B,0.1f,1); //Prop 2
	mDecoratorManager.GetDisplay().RenderFilledSector(0.0f, mDisplayHeight, mWorkingTriangle/2,mWorkingTriangle/2,90.0f,360.0f,scProp3R,scProp3G,scProp3B,0.1f,1); //Prop3
	mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth, 0.0f, mWorkingTriangle/2,mWorkingTriangle/2,90.0f,180.0f,scProp1R,scProp1G,scProp1B,0.1f,1); //Prop1
	mDecoratorManager.GetDisplay().RenderFilledSector(mDisplayWidth, mDisplayHeight, mWorkingTriangle/2,mWorkingTriangle/2,90.0f,90.0f,scProp4R,scProp4G,scProp4B,0.1f,1); //Prop4


	//Bug Origin Point
	mDecoratorManager.GetDisplay().RenderFilledEllipse(tBugPositionX, tBugPositionY, mCellDimensionX/6,mCellDimensionY/6,1.0f,0.0f,0.0f,1.0f,1);

	//Ending points
	if(mActualCarte->getEndNumber() > 0){
		std::vector<wykobi::point2d<float>> tEnd = mActualCarte->getEndPoint();

		for(std::vector<wykobi::point2d<float>>::iterator it = tEnd.begin(); it != tEnd.end() ; ++it){
			float tEndPositionX = mMapPoint2.x + ((*it).x - (*it).y)*mCellDimensionX;
			float tEndPositionY = mMapPoint2.y + ((*it).x + (*it).y + 1)*mCellDimensionY;
			mDecoratorManager.GetDisplay().RenderFilledEllipse(tEndPositionX, tEndPositionY, mCellDimensionX/2,mCellDimensionY/2,0.0f,0.0f,0.0f,1.0f,1);

		}
	}

	//Obstacles points
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
 */
