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

static const float scINSTRUCTION_X = 300.0f;
static const float scINSTRUCTION_Y = 150.0f;
static const char* const scINSTRUCTION_TEXT = "Put the cards in the boxes";

static const float scCENT1_X1 = 120.0f;
static const float scCENT1_X2 = 219.0f;
static const float scCENT2_X1 = 535.0f;
static const float scCENT2_X2 = 634.0f;
static const float scTEN1_X1 = 220.0f;
static const float scTEN1_X2 = 319.0f;
static const float scTEN2_X1 = 635.0f;
static const float scTEN2_X2 = 734.0f;
static const float scUNIT1_X1 = 320.0f;
static const float scUNIT1_X2 = 419.0f;
static const float scUNIT2_X1 = 735.0f;
static const float scUNIT2_X2 = 834.0f;

static const float scCENT_SOLUTION_X1 = 950.0f;
static const float scCENT_SOLUTION_X2 = 1049.0f;
static const float scTEN_SOLUTION_X1 = 1050.0f;
static const float scTEN_SOLUTION_X2 = 1149.0f;
static const float scUNIT_SOLUTION_X1 = 1150.0f;
static const float scUNIT_SOLUTION_X2 = 1249.0f;

static const float scY1 = 250.0f;
static const float scY2 = 384.0;

static const float scCENT1_X_AVG = 169.0f;
static const float scCENT2_X_AVG = 584.0f;
static const float scTEN1_X_AVG = 269.0f;
static const float scTEN2_X_AVG = 684.0f;
static const float scUNIT1_X_AVG = 369.0f;
static const float scUNIT2_X_AVG = 784.0f;
static const float scY_AVG = 317.0f;

static const float scGREEN_R = 0.0f;
static const float scGREEN_G = 153.0f;
static const float scGREEN_B = 0.0f;
static const float scBLUE_R = 0.0f;
static const float scBLUE_G = 0.0f;
static const float scBLUE_B = 204.0f;
static const float scYELLOW_R = 255.0f;
static const float scYELLOW_G = 255.0f;
static const float scYELLOW_B = 0.0f;

static const float scPLUS_X = 485.0f;
static const float scEQUAL_X = 900.0f;




namespace decorators {
	class BlankActivityCard : public FiducialDecorator
	{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		BlankActivityCard (DecoratorManager &pDecoratorManager,
				FiducialMarker *pMarker,
				BlankNumberModel *pModel,
				const int firstSummand,
				const int secondSummand);
		virtual ~BlankActivityCard();

	protected:
		void update();

		BlankNumberModel *mNumberModel;

		bool mNumbersAreSet;

		void ShowInstruction();
		void ShowActiveCards();
		void DrawNumbersAndLines();
		void SetNumbers();
		void DrawRectangles();
		void DrawDigits();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		const int mFirstSummand;
		const int mSecondSummand;
		/*
		int tCent1 = mFirstSummand/100;
		int tCent2 = mSecondSummand/100;
		int tTen1 = (mFirstSummand/10)%10;
		int tTen2 = (mSecondSummand/10)%10;
		int tUnit1 = mFirstSummand%10;
		int tUnit2 = mSecondSummand%10;
		*/
		int tCent1;
		int tCent2;
		int tTen1;
		int tTen2;
		int tUnit1;
		int tUnit2;
	};
}

#endif /* BLANKACTIVITYCARD_HPP_ */

