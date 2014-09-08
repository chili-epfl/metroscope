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

		void SetCurrentActivity(bool pIsCurrentActivity) {mIsCurrentActivity = pIsCurrentActivity;}
		bool IsPresent(){return mMarker->isPresent() && IsActivityPresent();}

	protected:
		void update();
		bool IsActivityPresent();
		void CheckManipulativesPresent();
		void CheckCircularModel();
		void CheckRectangularModel();
		void CheckTokenModel();
		void CheckFractionModel();
		void ShowFeedback(wykobi::quadix<float,2> pMarkerCorners, int pManipulativeType, bool pIsCorrect); //0 Circular, 1 Rectangular, 2 Token, 3 Card
		void ShowCardFeedback();


		/*CircularFractionModel *mAngleModel1;
		CircularFractionModel *mAngleModel2;
		RectangleFractionModel *mRectangleModel1;
		RectangleFractionModel *mRectangleModel2;
		*/

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		std::vector<CircularFractionModel *> mCircularModel;
		std::vector<RectangleFractionModel *> mRectangularModel;
		TokenModel *mTokenModel;
		FractionCard **mActivityCard;
		FractionCard **mFractionCards;

		float mCurrentProportion;

		int mActiveManipulatives;
		int mEquivalentManipulatives;
		FractionCard *mCurrentActivity;
		bool mIsCurrentActivity;
};

}


#endif /* EQUIVALENTFRACTIONVIEW_HPP_ */
