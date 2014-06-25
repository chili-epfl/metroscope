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



#ifndef FRACTIONCOMPARISIONVIEW_HPP_
#define FRACTIONCOMPARISIONVIEW_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "CircularFractionModel.hpp"
#include "RectangleFractionModel.hpp"
#include "TokenModel.hpp"
#include <fstream>
#include <map>
#include <utility>
#include "../KillBugConstant.hpp"
#include "FractionCard.hpp"

namespace decorators {

class FractionComparisionView : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		FractionComparisionView(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pActivityMarker,
						CircularFractionModel *pAngleModel1, CircularFractionModel *pAngleModel2,
						RectangleFractionModel *pRectangleModel1, RectangleFractionModel *pRectangleModel2, TokenModel *pTokenModel1,
						FractionCard ** pFractionCards, int pOrder); //pOrder: 0-> greater than, 1-> smaller than

	protected:
		void update();
		FiducialMarker *mActivityMarker;
		CircularFractionModel *mAngleModel1;
		CircularFractionModel *mAngleModel2;
		RectangleFractionModel *mRectangleModel1;
		RectangleFractionModel *mRectangleModel2;
		TokenModel *mTokenModel;
		FractionCard **mFractionCards;
		void CheckManipulativesPresent();
		int mActiveManipulatives;
		void CheckOrder();
		int mOrder;
		bool mCorrectOrder;
		bool IsActivityPresent();
		int mActivityPresent;
		void IdentifyFractions();
		void CheckAnswer();
		float mRectangleWidth;
		std::map<float,float> mFractions; //Key is the position in X, so it will be always be orderer from left to right

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
};
}

#endif /* FRACTIONCOMPARISIONVIEW_HPP_ */

