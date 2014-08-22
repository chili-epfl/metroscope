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

#ifndef KILLBUGVIEW_HPP_
#define KILLBUGVIEW_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "../KillBugConstant.hpp"
#include <fstream> // (?)
#include "KillBugModel.hpp"
#include "Carte.hpp"
#include "FractionBugHint.hpp"
#include "FlipperKillBug.hpp"

namespace decorators{
class KillBugView : public FiducialDecorator
{
	public:
		static FiducialDecorator *create (libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		KillBugView(DecoratorManager &pDecoratorManager, KillBugModel *pKillBugModel);
		~KillBugView();

		void displayMap();
		void drawGrid();
		void drawOriginPoint(float tPositionX, float tPositionY);
		void drawEndingPoints();
		void drawObstaclePoints();
		void drawMapStatusFeedback();
		void drawMoveFeedback();
		void displayBug();
		void displayHint();
		void displayDiscreteHint(std::vector<int> pNumerator, std::vector<int> pDenominator);
		void displayFractionHint(std::vector<int> pNumerator, std::vector<int> pDenominator);
		void displayDecimalHint(std::vector<float> pProportion);
		void displayRectangularHint(std::vector<float> pProportion);
		void displayCircularHint(std::vector<float> pProportion);
		void displayIndividualDiscreteHint(int pNumerator, int pDenominator, int pProportionNumber);
		void displayIndividualFractionHint(int pNumerator, int pDenominator, int pProportionNumber);
		void displayIndividualDecimalHint(float pProportion, int pProportionNumber);
		void displayIndividualRectangularHint(float pProportion, int pProportionNumber);
		void displayIndividualCircularHint(float pProportion, int pProportionNumber);
		void DisplayFlipperFeedback();

		void SetProportion(int pProportionNumber, int pNumerator, int pDenominator);
		void SetDisplayWidth(int pWidth){mDisplayWidth = pWidth;}
		void SetDisplayHeight(int pHeight){mDisplayHeight = pHeight;}
		void SetWrongMovementFrames(int pFrames);
		void SetNewMapFrames(int pFrames);
		void SetProportionFeedbackFrames(int pProportionFrameNumber, int pFrames);
		void SetProportionGreater (int pProportionNumber, bool pProportion);
		void DisplayBug();
		void DisplayProportions();
		void RenderProportion(float pProportion, int pProportionNumber);
		void DisplayProportions(int pHintType);
		void DisplayCircularHint();
		void DisplayRectangularHint();
		void DisplayDiscreteHint();
		void DisplayDecimalHint();
		void DisplayFractionHint();
		//void DisplayFlipperFeedback();
		void DivideCircunferenceManipulatives(int pParts);
		void DivideRectangleManipulatives(int pParts);

	protected:
		void update();


		long mLastShot;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		KillBugModel *mKillBugModel;
		Carte *mActualMap;
		FractionBugHint *mActualHint;
		float mProportion1,mProportion2,mProportion3,mProportion4;
		FlipperKillBug *mActualFlipper;

		int mDisplayWidth, mDisplayHeight;
		float mWorkingTriangle;
		int mMapSize; //TODO: Is is necessary to have it here?
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
		float mCellDimensionX;
		float mCellDimensionY;
		//long mLastShot;
};
}



#endif /* KILLBUGVIEW_HPP_ */

