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

#ifndef FractionHint_HPP
#define FractionHint_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "FractionModel.hpp"
#include "FractionAnswerChecker.hpp"

namespace decorators {

class FractionHint : public FiducialDecorator
{
	protected:
		void update();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		FractionModel *mFractionModel;
		FractionAnswerChecker *mChecker;

		const HintType mHintType;

		void displayHintRepresentationsUnderCards(std::vector<FractionCard *> &pCards, HintType pType, float pVerticalOffset);
		void graphicallyRepresentFraction(HintType pType, int pNumerator, int pDenominator, float pX, float pY, float pWidth, float pHeight);

		void displayHelpfulMessage();
		void displayHelpfulMessageGraphics(ErrorType pType);

		void drawUnitBlocks(std::vector<FractionCard *> &tActiveCards, int pHintNumber);
		void drawAdditionAndSubtractionUnitBlocks(FractionCard *pOperand1, FractionCard *pOperand2, FractionCard *pResult, float pVerticalOffset);
		void drawMultiplicationUnitBlocks(FractionCard *pOperand1, FractionCard *pOperand2, FractionCard *pResult, float pVerticalOffset);
		void drawDivisionUnitBlocks(FractionCard *pOperand1, FractionCard *pOperand2, FractionCard *pResult, float pVerticalOffset);

		void drawSimpleUnitBlocks(const FiducialMarker *pMarker, int pNumerator, int pDenominator,  int pHorizontalPartitions, int pVerticalPartitions, float pTopCornerX, float pTopCornerY, const color *pFillColor, bool pHighlightHorizontal, bool pHighlightVertical);

		void drawMultiplicationUnitBlockFill(float pTopCornerX, float pTopCornerY, int pNumHorizontalPartitions, int pNumVerticalPartitions, int pOp1Numerator, int pOp2Numerator, int pResultNumerator);
		void outlineUnitBoxArea(float pTopCornerX, float pTopCornerY, int pTotalPartitions, int pNumPartitionsWide, bool pHorizontal, const color *pColor);
		void drawUnitBlockFill(float pTopCornerX, float pTopCornerY, int pNumHorizontalPartitions, int pNumVerticalPartitions, int pNumerator1, int pNumerator2, int pNumerator3, const color *pColor1, const color *pColor2, const color *pColor3);
		void drawUnitBlockLines(float pTopCornerX, float pTopCornerY, int pNumHorizontalPartitions, int pNumVerticalPartitions, bool pHighlightHorizontal, bool pHighlightVertical);
		void drawUnitBlockBox(float pTopCornerX, float pTopCornerY);
		void drawAdditionResultUnitBlock(float pTopCornerX, float pTopCornerY, int pOp1Numerator, int pOp1Denominator, int pOp2Numerator, int pOp2Denominator, int pResultNumerator, int pResultDenominator, int pResultHorizontalPartitions, int pResultVerticalPartitions, bool pHighlightHorizontal, bool pHighlightVertical);
		void drawSubtractionResultUnitBlock(float pTopCornerX, float pTopCornerY, int pOp1Numerator, int pOp1Denominator, int pOp2Numerator, int pOp2Denominator, int pResultNumerator, int pResultDenominator, int pResultHorizontalPartitions, int pResultVerticalPartitions, bool pHighlightHorizontal, bool pHighlightVertical);

		void drawPieChart(int pNumerator, int pDenominator, float pX, float pY, float pRadius, const color *pFillColor, const color *pOutlineColor);
		void drawPieChartWithPieces(int pNumerator, int pDenominator, float pX, float pY, float pRadius, const color *pFillColor, const color *pOutlineColor);
		void drawFilledBarFraction(int pNumerator, int pDenominator, float pX, float pY, float pWidth, float pHeight, const color *pFillColor, const color *pOutlineColor);
		void drawShadedPieces(int pNumerator, int pDenominator, float pX, float pY, const color *pFillColor, const color *pOutlineColor);
		void displayDecimalRepresentation(int pNumerator, int pDenominator, float pX, float pY, const color *pColor);

		void displaySingleFraction(int pNumerator, int pDenominator, float pX, float pY, float pWidth, float pHeight, float pScale, const color *pNumeratorColor, const color *pDenominatorColor, const color *pFractionBarColor);
    	void displaySingleFractionWithLabels(int pNumerator, int pDenominator, const char *pNumeratorLabel, const char *pDenominatorLabel, float pXOffset, float pYOffset);
		void displayTwoFractionsWithGraphics(int pNumerator1, int pDenominator1, int pNumerator2, int pDenominator2, HintType pType, float pXOffset, float pYOffset);



	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		FractionHint(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FractionModel *pFractionModel, FractionAnswerChecker *pChecker, HintType pHintType);
		~FractionHint();
};

}
#endif
