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

#ifndef EQUIVALENTFRACTIONVIEW_HPP_
#define EQUIVALENTFRACTIONVIEW_HPP_


#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "CircularFractionModel.hpp"
#include "RectangleFractionModel.hpp"
#include "TokenModel.hpp"
#include <fstream>
#include "../KillBugConstant.hpp"
#include "FractionCard.hpp"

namespace decorators {

class EquivalentFractionView : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		EquivalentFractionView(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker,
				CircularFractionModel *pAngleModel1, CircularFractionModel *pAngleModel2,
				RectangleFractionModel *pRectangleModel1, RectangleFractionModel *pRectangleModel2, TokenModel *pTokenModel1,
				FractionCard ** pActivityCards, FractionCard ** pFractionCards);

	protected:
		void update();
		CircularFractionModel *mAngleModel1;
		CircularFractionModel *mAngleModel2;
		RectangleFractionModel *mRectangleModel1;
		RectangleFractionModel *mRectangleModel2;
		TokenModel *mTokenModel;
		FractionCard **mActivityCard;
		FractionCard **mFractionCards;
		bool IsActivityPresent();
		float mCurrentProportion;
		void CheckManipulativesPresent();
		int mActiveManipulatives;
		int mEquivalentManipulatives;
		FractionCard *mCurrentActivity;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
};

}


#endif /* EQUIVALENTFRACTIONVIEW_HPP_ */
