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

#ifndef KILLBUGMODEL_HPP_
#define KILLBUGMODEL_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "CircularFractionModel.hpp"
#include "RectangleFractionModel.hpp"
#include "TokenModel.hpp"
#include "FractionCard.hpp"
#include "FlipperKillBug.hpp"
#include "FractionBugHint.hpp"
#include "Carte.hpp"
#include "../KillBugConstant.hpp"
#include <fstream>
#include <string>




namespace decorators{

class KillBugModel : public FiducialDecorator
{
	public:
		static FiducialDecorator *create (libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		KillBugModel (DecoratorManager &pDecoratorManager,
				CircularFractionModel *pAngleModel1, CircularFractionModel *pAngleModel2,
				RectangleFractionModel *pRectangleModel1, RectangleFractionModel *pRectangleModel2,
				TokenModel *pTokenModel1,
				FlipperKillBug ** pFlipper,
				FractionBugHint ** pFractionHints,
				FractionCard ** pFractionCards,
				Carte ** pCartes);
		~KillBugModel();


		bool IsMapPresent() {return IsCartePresent();}
		Carte * GetActualMap() {return mActualCarte;}
		bool IsGameStarted() {return mGameStarted;}
		int GetNewMapFrames() {return mNewMapFrames;}
		void DecreaseNewMapFrames() {mNewMapFrames--;}
		int GetWrongMovementFrames() {return mWrongMovementFrames;}
		void DecreaseWrongMovementFrames() {mWrongMovementFrames--;}
		int Get13Frames(){return mProportionFeedbackFrames13;}
		int Get24Frames(){return mProportionFeedbackFrames24;}
		void Decrease13Frames(){mProportionFeedbackFrames13--;}
		void Decrease24Frames(){mProportionFeedbackFrames24--;}
		bool IsProportion1Greater() {return mProportion1Greater;}
		bool IsProportion2Greater() {return mProportion2Greater;}
		bool IsProportion3Greater() {return mProportion3Greater;}
		bool IsProportion4Greater() {return mProportion4Greater;}
		void SetGameStarted(bool pNewState) {mGameStarted = pNewState;}
		wykobi::point2d<int> & GetBugPosition() {return mBugPosition;}
		bool IsWrongMove() {return mWrongMove;}
		bool IsNeutralMove() {return mNeutralMove;}
		int StepsDone() {return mSteps;}
		long GetLastShot() {return mLastShot;}
		bool IsAnyHintPresent() {return IsHintPresent();}
		FractionBugHint * GetActualHint() {return mActualHint;}
		FlipperKillBug * GetActualFlipper() {return mActualFlipper;}
		std::vector<int> GetProportionNumerator() {return mProportionNumerator;}
		std::vector<int> GetProportionDenominator()	{return mProportionDenominator;}
		std::vector<float> GetProportionValue(){return mProportion;}
		bool IsPresent() {return IsMapPresent();}
		void SetCurrentActivity(bool pIsCurrentActivity) {mIsCurrentActivity = pIsCurrentActivity;}
		bool IsCurrentActivity() {return mIsCurrentActivity;}
		bool IsFlipperPresent();
		std::string GetStringRepresentation();

	protected:
		void update();
		void MakeMove();
		void Start();
		void CheckHints();
		bool IsCartePresent();
		void FetchProportions();
		int GetProportionNumber(wykobi::point2d<float> pPosition);
		void SetProportionNumber(wykobi::point2d<float> pPosition, float pProportion);
		void SetProportionNumber(int pQuadrant, float pProportion);
		bool IsHintPresent();
		void SetProportionNumber(int pQuadrant, int pNumerator, int pDenominator);
		void SetProportionNumber(wykobi::point2d<float> pPosition, int pNumerator, int pDenominator);
		int CheckCircularManipulative(int pProportionNumber);
		int CheckRectangularManipulative(int pProportionNumber);
		int CheckTokenManipulative(int pProportionNumber);
		int CheckFractionManipulative(int pProportionNumber);
		void ClearProportions();
		void SaveProportions();
		void DisplayFlipperFeedback();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		CircularFractionModel *mCircularModel1;
		CircularFractionModel *mCircularModel2;
		RectangleFractionModel *mRectangleModel1;
		RectangleFractionModel *mRectangleModel2;
		TokenModel *mTokenModel;
		FractionBugHint **mHints;
		FractionCard ** mFractionCards;
		Carte ** mCartes;
		FlipperKillBug ** mFlipper;
		FlipperKillBug *mActualFlipper;

		Carte *mActualCarte;
		FractionBugHint *mActualHint;
		float mProportion1,mProportion2,mProportion3,mProportion4;

		wykobi::point2d<int> mBugPosition;

		int mSteps;

		bool mGameStarted;
		bool mMapFinished;
		bool mMapNew;

		int mActiveManipulatives;

		long mLastShot;

		int mProportion1Numerator;
		int mProportion1Denominator;
		int mProportion2Numerator;
		int mProportion2Denominator;
		int mProportion3Numerator;
		int mProportion3Denominator;
		int mProportion4Numerator;
		int mProportion4Denominator;
		std::vector<int> mProportionNumerator;
		std::vector<int> mProportionDenominator;
		std::vector<float> mProportion;

		bool mWrongMove;
		bool mNeutralMove;

		int mWrongMovementFrames;
		int mNewMapFrames;
		int mProportionFeedbackFrames13;
		int mProportionFeedbackFrames24;
		bool mProportion1Greater;
		bool mProportion2Greater;
		bool mProportion3Greater;
		bool mProportion4Greater;

		bool mIsCurrentActivity;

};
}

#endif /* KILLBUGMODEL_HPP_ */

