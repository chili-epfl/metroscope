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
#include "AngleModel.hpp"
#include "RectangleFractionModel.hpp"
#include "TokenModel.hpp"
#include "FractionCard.hpp"
#include "Flipper.hpp"
#include "FractionBugHint.hpp"
#include "Carte.hpp"
#include "../KillBugConstant.hpp"
#include <fstream>

namespace decorators{

class KillBugModel : public FiducialDecorator
{
	public:
		static FiducialDecorator *create (libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		KillBugModel (DecoratorManager &pDecoratorManager,
				AngleModel *pAngleModel1, AngleModel *pAngleModel2,
				RectangleFractionModel *pRectangleModel1, RectangleFractionModel *pRectangleModel2,
				TokenModel *pTokenModel1, TokenModel *pTokenModel2,
				Flipper *pGoFlipper, FractionBugHint ** pFractionHints,
				FractionCard ** pFractionCards,
				Carte ** pCartes);
		~KillBugModel();

	protected:
		void update();
		void MakeMove();
		void Start();

		void DisplayMap();
		void DisplayBug();
		void DisplayProportions();
		void RenderProportion(float pProportion, int pProportionNumber);
		bool IsCartePresent();
		void FetchProportions();
		int GetProportionNumber(wykobi::point2d<float> pPosition);
		void SetProportionNumber(wykobi::point2d<float> pPosition, float pProportion);

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		AngleModel *mCircularModel1;
		AngleModel *mCircularModel2;
		RectangleFractionModel *mRectangleModel1;
		RectangleFractionModel *mRectangleModel2;
		TokenModel *mTokenModel1;
		TokenModel *mTokenModel2;
		Flipper *mFlipper;
		FractionBugHint **mHints;
		FractionCard ** mFractionCards;
		Carte ** mCartes;

		Carte *mActualCarte;
		float mProportion1,mProportion2,mProportion3,mProportion4;

		int mDisplayWidth, mDisplayHeight;
		float mWorkingTriangle;
		int mMapSize;
		float mCellDimensionX, mCellDimensionY;
		float mMapWidth, mMapHeight;
		wykobi::point2d<int> mBugPosition;
		int mSteps;
		bool mGameStarted;
		int mActiveManipulatives;

};
}

#endif /* KILLBUGMODEL_HPP_ */
