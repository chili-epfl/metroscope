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

#ifndef FRACTIONVIEWMEASURE_HPP_
#define FRACTIONVIEWMEASURE_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "CircularFractionModel.hpp"
#include "RectangleFractionModel.hpp"
#include "TokenModel.hpp"
#include <fstream>
#include "../KillBugConstant.hpp"

namespace decorators {

class FractionViewMeasure : public FiducialDecorator
{
	public:
		static FiducialDecorator *create (libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		FractionViewMeasure (DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, CircularFractionModel *pAngleModel1, CircularFractionModel *pAngleModel2,
				RectangleFractionModel *pRectangleModel1, RectangleFractionModel *pRectangleModel2, TokenModel *pTokenModel1,
				int pExpectedNumerator, int pExpectedDenominator);

		void SetCurrentActivity(bool pIsCurrentActivity) {mIsCurrentActivity = pIsCurrentActivity;}
		bool IsPresent() {return mMarker->isPresent();}

	protected:
		void update();
		CircularFractionModel *mAngleModel1;
		CircularFractionModel *mAngleModel2;
		RectangleFractionModel *mRectangleModel1;
		RectangleFractionModel *mRectangleModel2;
		TokenModel *mTokenModel;
		int mExpectedNumerator;
		int mExpectedDenominator;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		bool mIsCurrentActivity;
};
}

#endif /* FRACTIONVIEWMEASURE_HPP_ */
