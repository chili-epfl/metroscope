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

#ifndef BLANKACTIVITYCARD_HPP_
#define BLANKACTIVITYCARD_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "BlankNumberModel.hpp"
#include "RegroupDigits.hpp"
#include "../ArithmeticsConstants.hpp"

namespace decorators {
	class BlankActivityCard : public FiducialDecorator
	{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		BlankActivityCard (DecoratorManager &pDecoratorManager,
				FiducialMarker *pMarker,
				BlankNumberModel *pModel,
				RegroupDigits *pRegroup,
				const int firstSummand,
				const int secondSummand);
		const FiducialMarker *GetMarker() const {return mMarker;}

		virtual ~BlankActivityCard();


	protected:
		void update();

		BlankNumberModel *mNumberModel;
		RegroupDigits *mRegroupDigits;
		const int mFirstSummand;
		const int mSecondSummand;
		bool mNumbersAreSet;


		int tCent1, tCent2;
		int tTen1, tTen2;
		int tUnit1, tUnit2;
		int mCentsSum, mTensSum, mUnitsSum;
		int mSolutionUnit, mSolutionTen, mSolutionCent;
		bool mIsSolutionUnitCorrect, mIsSolutionTenCorrect, mIsSolutionCentCorrect;
		bool mIsNecessaryU2T, mIsNecessaryT2C;
		bool mAreUnitsStacked, mAreTensStacked, mAreCentsStacked;

		void ShowInstruction();
		void ShowActiveCards();
		void DrawNumbersAndLines();
		void SetNumbers();
		void DrawRectangles();
		void TranslateNumber(int pType);
		void DrawDigits();
		void CheckSemiStack();
		void CheckSolution();
		void FillSolutionRectangles();
		void DrawSolutionDigits();
		void UpdateDigits();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		bool mIsTraslationStartedU2T;
		long mInitialTimeTraslationU2T;
		bool mIsTraslationDoneU2T;
		bool mIsTraslationStartedT2C;
		long mInitialTimeTraslationT2C;
		bool mIsTraslationDoneT2C;
		bool mIsTraslationStarted;
		long mInitialTimeTraslation;
		bool mIsTraslationDone;


	};
}

#endif /* BLANKACTIVITYCARD_HPP_ */

