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



#ifndef FRACTIONCOMPARISONVIEW_HPP_
#define FRACTIONCOMPARISONVIEW_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "CircularFractionModel.hpp"
#include "RectangleFractionModel.hpp"
#include "TokenModel.hpp"
#include <fstream>
#include <map>
#include <utility>
#include "../KillBugConstant.hpp"
#include "FractionCard.hpp"
#include "FractionBugHint.hpp"

namespace decorators {

class FractionComparisonView : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		FractionComparisonView(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pActivityMarker,
						CircularFractionModel *pAngleModel1, CircularFractionModel *pAngleModel2,
						RectangleFractionModel *pRectangleModel1, RectangleFractionModel *pRectangleModel2, TokenModel *pTokenModel1,
						FractionCard ** pFractionCards, FractionBugHint ** pFractionHints, int pOrder); //pOrder: 0-> greater than, 1-> smaller than, 2-> equal/equivalent

		void SetCurrentActivity(bool pIsCurrentActivity) {mIsCurrentActivity = pIsCurrentActivity;}
		bool IsPresent() {return (mActivityMarker->isPresent() || mMarker->isPresent());}
		bool IsHintPresent();
		void ShowHintFeedback();
		void DisplayDiscreteHint();
		void DisplayFractionHint();
		void DisplayDecimalHint();
		void DisplayRectangularHint();
		void DisplayCircularHint();
		void DisplayIndividualDiscreteHint(int pNumerator, int pDenominator, wykobi::point2d<float> pPosition);
		void DisplayIndividualFractionHint(int pNumerator, int pDenominator, wykobi::point2d<float> pPosition);
		void DisplayIndividualDecimalHint(float pProportion, wykobi::point2d<float> pPosition);
		void DisplayIndividualRectangularHint(float pProportion, wykobi::point2d<float> pPosition);
		void DisplayIndividualCircularHint(float pProportion, wykobi::point2d<float> pPosition);
		bool IsActivityPresent();
		bool IsMarkerPresent() {return mActivityMarker->isPresent();}

	protected:
		void update();
		FiducialMarker *mActivityMarker;
		CircularFractionModel *mAngleModel1;
		CircularFractionModel *mAngleModel2;
		RectangleFractionModel *mRectangleModel1;
		RectangleFractionModel *mRectangleModel2;
		TokenModel *mTokenModel;
		FractionCard **mFractionCards;
		FractionBugHint ** mFractionHints;
		void CheckManipulativesPresent();
		int mActiveManipulatives;
		void CheckOrder();
		int mOrder;
		bool mCorrectOrder;

		int mActivityPresent;
		void CheckAnswer();
		void ShowGrid();
		void ShowCardFeedback();
		float mRectangleWidth;
		std::map<float,float> mFractions; //Key is the position in X, so it will be always be orderer from left to right
		std::map<float,int> mNumerator;
		std::map<float,int> mDenominator;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		bool mIsCurrentActivity;
		FractionBugHint *mActualHint;

};
}

#endif /* FRACTIONCOMPARISONVIEW_HPP_ */

