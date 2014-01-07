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

#include "FractionHint.hpp"

#include <iostream>

const std::string decorators::FractionHint::scDecoratorName("FractionHint");
const  DecoratorManager::Registerer decorators::FractionHint::mRegisterer(decorators::FractionHint::scDecoratorName, &decorators::FractionHint::create);

decorators::FiducialDecorator *decorators::FractionHint::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		HintType tHintType;
		switch((int)pSetting["hint_type"]){
		case 0: tHintType = PieChart; break;
		case 1: tHintType = PieChartWithPieces; break;
		case 2: tHintType = FilledBar; break;
		case 3: tHintType = ShadedPieces; break;
		case 4: tHintType = CommonDenominators; break;
		case 5: tHintType = HelpfulMessage; break;
		case 6: tHintType = Decimal; break;
		case 7: tHintType = UnitBlocks; break;
		default:
			std::cerr << "Failed to load " << scDecoratorName << ". Invalid HintType." << std::endl;
			return 0;
		}
		return new decorators::FractionHint(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),
											(FractionModel *) pDecoratorManager.loadDecorator(pSetting["fraction_model"]),
											(FractionAnswerChecker *) pDecoratorManager.loadDecorator(pSetting["answer_checker"]),
											tHintType);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::FractionHint::FractionHint(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FractionModel *pFractionModel, FractionAnswerChecker *pChecker, HintType pHintType):
FiducialDecorator(pDecoratorManager, pMarker),
mFractionModel(pFractionModel),
mChecker(pChecker),
mHintType(pHintType)
{

}
decorators::FractionHint::~FractionHint(){ /*Empty*/}

void decorators::FractionHint::update() {
	if (mMarker->isPresent())
	{
		if(mHintType == HelpfulMessage){
			if(!mChecker->IsChecking()){
				displayHelpfulMessage();
				displayHelpfulMessageGraphics(mFractionModel->GetHintErrorType());
			}
		} else {
			std::vector<FractionCard *> tActiveCards = mFractionModel->GetActiveCards();
			int tHintNumber = mFractionModel->AddHintType(mHintType);
			if(tActiveCards.size() != 0){
				if(mHintType == UnitBlocks){
					drawUnitBlocks(tActiveCards, tHintNumber);
				} else{
					displayHintRepresentationsUnderCards(tActiveCards, mHintType, tHintNumber*scHINT_GRAPHICS_VERTICAL_OFFSET);
				}
			}
		}
	} else {
		mFractionModel->RemoveHintType(mHintType);
	}

}

void decorators::FractionHint::displayHintRepresentationsUnderCards(std::vector<FractionCard *> &pCards, HintType pHintType, float pVerticalOffset){
	int tLCD;
	if(mHintType == CommonDenominators) tLCD = mFractionModel->FractionLCD(pCards);
	for(std::vector<FractionCard *>::iterator it = pCards.begin(); it != pCards.end(); ++it){
		FractionCard *tFraction = *it;
		if(tFraction->IsPresent()){
			wykobi::point2d<float> tCenter = tFraction->GetMarker()->getCenter();
			mDecoratorManager.GetCam2World().InterpolatedMap(tCenter);
			mDecoratorManager.GetWorld2Proj().InterpolatedMap(tCenter);
			int tNumerator = mHintType == CommonDenominators? tFraction->GetNumerator()*tLCD/tFraction->GetDenominator():tFraction->GetNumerator();
			int tDenominator = mHintType == CommonDenominators? tLCD:tFraction->GetDenominator();
			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*(tFraction->GetMarker()), scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
			graphicallyRepresentFraction(pHintType, tNumerator, tDenominator,
										0.0f, scHINT_GRAPHICS_OFFSET_FROM_CARD_CENTER + pVerticalOffset,
										scHINT_GRAPHICS_ICONS_WIDTH, scHINT_GRAPHICS_ICONS_HEIGHT);
			mDecoratorManager.GetDisplay().PopTransformation();
		}
	}
}

void decorators::FractionHint::graphicallyRepresentFraction(HintType pHintType, int pNumerator, int pDenominator, float pX, float pY, float pWidth, float pHeight){
	switch(pHintType){
	case PieChart:
		drawPieChart(pNumerator, pDenominator, pX, pY, pWidth,
						scPIE_CHART_FILL_COLOR, scPIE_CHART_OUTLINE_COLOR);
		break;
	case PieChartWithPieces:
		drawPieChartWithPieces(pNumerator, pDenominator, pX, pY, pWidth,
				scDIVIDED_PIE_CHART_COLOR, scPIE_CHART_OUTLINE_COLOR);
		break;
	case FilledBar:
		drawFilledBarFraction(pNumerator, pDenominator, pX, pY, pWidth, pHeight, scFILLED_BAR_COLOR, scFILLED_BAR_OUTLINE_COLOR);
		break;
	case ShadedPieces:
		drawShadedPieces(pNumerator, pDenominator, pX, pY, scSHADED_PIECES_FILL_COLOR, scSHADED_PIECES_OUTLINE_COLOR);
		break;
	case CommonDenominators:
		displaySingleFraction(pNumerator, pDenominator, pX, pY, pWidth, pHeight, scHINT_GRAPHICS_FRACTIONS_SCALE,
				scCOMMON_DENOMINATORS_NUM_COLOR, scCOMMON_DENOMINATORS_DENOM_COLOR, scFRACTION_BAR_COLOR);
		break;
	case Decimal:
		displayDecimalRepresentation(pNumerator, pDenominator, pX, pY, scDECIMAL_HINT_COLOR);
		break;
	default:
		std::cerr << "Error: Invalid HintType" << std::endl;
		break;
	}
}


void decorators::FractionHint::displayHelpfulMessage(){
	if(mMarker->isPresent()){
			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarker, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
				mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(mFractionModel->GetHintMessage(), scTEXT_DELIMITERS,
						scHINT_MESSAGE_OFFSET_X, scHINT_MESSAGE_OFFSET_Y, scHINT_MESSAGE_WIDTH,
						false, scHINT_MESSAGE_SCALE,
						scHINT_MESSAGE_COLOR->r, scHINT_MESSAGE_COLOR->g, scHINT_MESSAGE_COLOR->b, scHINT_MESSAGE_COLOR->a);
				mDecoratorManager.GetDisplay().PopTransformation();
	}
}

void decorators::FractionHint::displayHelpfulMessageGraphics(ErrorType pType){
	int tNum1, tDenom1, tNum2, tDenom2;
	mFractionModel->GetHintMessageGraphicsFractions(tNum1, tDenom1, tNum2, tDenom2);
	switch (pType){
	case FractionWithLabels: displaySingleFractionWithLabels(tNum1,tDenom1, scNUMERATOR_LABEL, scDENOMINATOR_LABEL, scHINT_MESSAGE_OFFSET_X, 110); break;
	case TwoFractionsWithGraphics:
		displayTwoFractionsWithGraphics(tNum1, tDenom1, tNum2, tDenom2, mFractionModel->GetHintMessageGraphicsType(), scHINT_MESSAGE_OFFSET_X, 0.0);
		break;
	default: break;
	}

}

void decorators::FractionHint::drawUnitBlocks(std::vector<FractionCard *> &tActiveCards, int pHintNumber){
	const char *tActivity = mFractionModel->GetCurrentActivity();
	int tNumActiveCards = tActiveCards.size();
	if(tNumActiveCards==3 && (strcmp(tActivity, scADDITION_NAME) == 0 || strcmp(tActivity, scSUBTRACTION_NAME) == 0)){
		drawAdditionAndSubtractionUnitBlocks(tActiveCards[0], tActiveCards[1], tActiveCards[2], pHintNumber*scHINT_GRAPHICS_VERTICAL_OFFSET);
	} else if(tNumActiveCards==3 && strcmp(tActivity, scMULTIPLICATION_NAME) == 0){
		drawMultiplicationUnitBlocks(tActiveCards[0], tActiveCards[1], tActiveCards[2], pHintNumber*scHINT_GRAPHICS_VERTICAL_OFFSET);
	} else if(tNumActiveCards == 3 && strcmp(tActivity, scDIVISION_NAME) == 0){
		drawDivisionUnitBlocks(tActiveCards[0], tActiveCards[1], tActiveCards[2], pHintNumber*scHINT_GRAPHICS_VERTICAL_OFFSET);
	} else {
		int count = 0;
		float tTopCornerX = -scUNIT_BLOCK_WIDTH/2;
		float tTopCornerY = scHINT_GRAPHICS_OFFSET_FROM_CARD_CENTER + pHintNumber*scHINT_GRAPHICS_VERTICAL_OFFSET - scUNIT_BLOCK_WIDTH/2;
		for(std::vector<FractionCard *>::iterator it = tActiveCards.begin(); it != tActiveCards.end(); ++it){
			FractionCard *tFraction = *it;
			if(tFraction->IsPresent()){
				int pHorizontalPartitions = count%2 == 0? tFraction->GetDenominator():1;
				int pVerticalPartitions = count%2 == 0? 1:tFraction->GetDenominator();
				drawSimpleUnitBlocks(tFraction->GetMarker(),
						tFraction->GetNumerator(), tFraction->GetDenominator(),
						pHorizontalPartitions, pVerticalPartitions,
						tTopCornerX, tTopCornerY,
						scUNIT_BLOCK_OPERAND_2_COLOR, false, false);
			}
			count++;
		}
	}
}

void decorators::FractionHint::drawAdditionAndSubtractionUnitBlocks(FractionCard *pOperand1, FractionCard *pOperand2, FractionCard *pResult, float pVerticalOffset){
	bool tAddition = (strcmp(mFractionModel->GetCurrentActivity(), scADDITION_NAME) == 0);
	int tOp1Numerator = pOperand1->GetNumerator();
	int tOp1Denominator = pOperand1->GetDenominator();
	int tOp2Numerator = pOperand2->GetNumerator();
	int tOp2Denominator = pOperand2->GetDenominator();
	int tResultNumerator = pResult->GetNumerator();
	int tResultDenominator = pResult->GetDenominator();

	int tResultHorizontalPartitions;
	int tResultVerticalPartitions;
	bool tHighlightHorizontal = false;
	bool tHighlightVertical = false;
	if((tHighlightHorizontal = tResultDenominator%tOp1Denominator == 0)){
		tResultHorizontalPartitions = tOp1Denominator;
		tResultVerticalPartitions = tResultDenominator/tOp1Denominator;
		if(tResultVerticalPartitions %tOp2Denominator == 0) tHighlightVertical = true;
	} else if ((tHighlightVertical = tResultDenominator%tOp2Denominator == 0)) {
		tResultVerticalPartitions = tOp2Denominator;
		tResultHorizontalPartitions = tResultDenominator/tOp2Denominator;
	} else {
		mFractionModel->getClosestFactors(tResultDenominator, tResultVerticalPartitions, tResultHorizontalPartitions);
	}

	float tTopCornerX = -scUNIT_BLOCK_WIDTH/2;
	float tTopCornerY = scHINT_GRAPHICS_OFFSET_FROM_CARD_CENTER + pVerticalOffset - scUNIT_BLOCK_WIDTH/2;

	if(pOperand1->IsPresent()){
		drawSimpleUnitBlocks(pOperand1->GetMarker(),
									 pOperand1->GetNumerator(), pOperand1->GetDenominator(),
									 pOperand1->GetDenominator(), 1,
									 tTopCornerX, tTopCornerY,
									 scUNIT_BLOCK_OPERAND_1_COLOR, tHighlightHorizontal, false);

	}
	if (pOperand2->IsPresent()){
		drawSimpleUnitBlocks(pOperand2->GetMarker(),
							 pOperand2->GetNumerator(), pOperand2->GetDenominator(),
							 1, pOperand2->GetDenominator(),
							 tTopCornerX, tTopCornerY,
							 scUNIT_BLOCK_OPERAND_2_COLOR, false, tHighlightVertical);
	}

	if(pResult->IsPresent()){
		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*(pResult->GetMarker()), scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
		if(tAddition) {
			drawAdditionResultUnitBlock(tTopCornerX, tTopCornerY, tOp1Numerator, tOp1Denominator, tOp2Numerator, tOp2Denominator, tResultNumerator, tResultDenominator, tResultHorizontalPartitions, tResultVerticalPartitions, tHighlightHorizontal, tHighlightVertical);
		} else{
			drawSubtractionResultUnitBlock(tTopCornerX, tTopCornerY, tOp1Numerator, tOp1Denominator, tOp2Numerator, tOp2Denominator, tResultNumerator, tResultDenominator, tResultHorizontalPartitions, tResultVerticalPartitions, tHighlightHorizontal, tHighlightVertical);
		}
		mDecoratorManager.GetDisplay().PopTransformation();
	}

}

void decorators::FractionHint::drawAdditionResultUnitBlock(float pTopCornerX, float pTopCornerY, int pOp1Numerator, int pOp1Denominator, int pOp2Numerator, int pOp2Denominator, int pResultNumerator, int pResultDenominator, int pResultHorizontalPartitions, int pResultVerticalPartitions, bool pHighlightHorizontal, bool pHighlightVertical){
		int tLCD = mFractionModel->LCM(pOp1Denominator, pOp2Denominator);
		int tFirstPortion, tSecondPortion, tThirdPortion;
		int tOp1ConvertedNumerator = pOp1Numerator*(pResultDenominator/pOp1Denominator);
		int tOp2ConvertedNumerator = pOp2Numerator*(pResultDenominator/pOp2Denominator);

		int tCorrectNumerator = tOp1ConvertedNumerator + tOp2ConvertedNumerator;

			if(pResultNumerator > 0 && pResultDenominator%tLCD == 0) {
				if (pResultNumerator > 0 && pResultNumerator <= tOp1ConvertedNumerator){
						tFirstPortion = pResultNumerator;
						tSecondPortion = 0;
						tThirdPortion = 0;
				} else {
					if(pResultNumerator <= tCorrectNumerator){
						tFirstPortion = tOp1ConvertedNumerator;
						tSecondPortion = pResultNumerator - tOp1ConvertedNumerator;
						tThirdPortion = 0;
					} else{
						tFirstPortion = tOp1ConvertedNumerator;
						tSecondPortion = tOp2ConvertedNumerator;
						tThirdPortion = pResultNumerator - tCorrectNumerator;
					}
				}
			}

			int tNumUnitBlocks = pResultNumerator/pResultDenominator + (pResultNumerator%pResultDenominator==0?0:1);
			if(tNumUnitBlocks == 0 && pResultNumerator == 0) tNumUnitBlocks = 1;
			float tOffset = .5-((float)tNumUnitBlocks)/2-((float)tNumUnitBlocks-1)/2*(scUNIT_BLOCK_SEPARATION/scUNIT_BLOCK_WIDTH);
			pTopCornerX +=tOffset*scUNIT_BLOCK_WIDTH;
			for (int i= 0; i < tNumUnitBlocks; i++){
				if (pResultNumerator > 0 && pResultDenominator%tLCD == 0){
					if(tFirstPortion >= pResultDenominator){
						drawUnitBlockFill(pTopCornerX, pTopCornerY, pResultHorizontalPartitions, pResultVerticalPartitions,
								pResultDenominator, 0, 0,
							scUNIT_BLOCK_OPERAND_1_COLOR, scUNIT_BLOCK_OPERAND_2_COLOR, scUNIT_BLOCK_OVERFLOW_COLOR);
						tFirstPortion -=pResultDenominator;
					} else if (tFirstPortion + tSecondPortion >= pResultDenominator){
						drawUnitBlockFill(pTopCornerX, pTopCornerY, pResultHorizontalPartitions, pResultVerticalPartitions,
							tFirstPortion, pResultDenominator-tFirstPortion, 0,
							scUNIT_BLOCK_OPERAND_1_COLOR, scUNIT_BLOCK_OPERAND_2_COLOR, scUNIT_BLOCK_OVERFLOW_COLOR);
						tSecondPortion -= (pResultDenominator - tFirstPortion);
						tFirstPortion = 0;
					} else if (tFirstPortion + tSecondPortion + tThirdPortion >= pResultDenominator ){
						drawUnitBlockFill(pTopCornerX, pTopCornerY, pResultHorizontalPartitions, pResultVerticalPartitions,
							tFirstPortion, tSecondPortion, pResultDenominator-tFirstPortion-tSecondPortion,
							scUNIT_BLOCK_OPERAND_1_COLOR, scUNIT_BLOCK_OPERAND_2_COLOR, scUNIT_BLOCK_OVERFLOW_COLOR);
						tThirdPortion -= (pResultDenominator - tFirstPortion - tSecondPortion);
						tFirstPortion = 0;
						tSecondPortion = 0;
					} else{
						drawUnitBlockFill(pTopCornerX, pTopCornerY, pResultHorizontalPartitions, pResultVerticalPartitions,
							tFirstPortion, tSecondPortion, tThirdPortion,
							scUNIT_BLOCK_OPERAND_1_COLOR, scUNIT_BLOCK_OPERAND_2_COLOR, scUNIT_BLOCK_OVERFLOW_COLOR);
					}

				} else if (pResultNumerator > 0) {
					if(i == tNumUnitBlocks-1){
						drawUnitBlockFill(pTopCornerX, pTopCornerY,
							pResultHorizontalPartitions, pResultVerticalPartitions,
							pResultNumerator%pResultDenominator==0? pResultDenominator: pResultNumerator%pResultDenominator, 0, 0,
							scUNIT_BLOCK_RESULT_COLOR, NULL, NULL);
					} else {
						drawUnitBlockFill(pTopCornerX, pTopCornerY,
							pResultHorizontalPartitions, pResultVerticalPartitions,
							pResultDenominator, 0, 0,
							scUNIT_BLOCK_RESULT_COLOR, NULL, NULL);
					}

				}
				drawUnitBlockLines(pTopCornerX, pTopCornerY, pResultHorizontalPartitions, pResultVerticalPartitions, pHighlightHorizontal, pHighlightVertical);
				drawUnitBlockBox(pTopCornerX, pTopCornerY);

				pTopCornerX += scUNIT_BLOCK_WIDTH+scUNIT_BLOCK_SEPARATION;
			}
}

void decorators::FractionHint::drawSubtractionResultUnitBlock(float pTopCornerX, float pTopCornerY, int pOp1Numerator, int pOp1Denominator, int pOp2Numerator, int pOp2Denominator, int pResultNumerator, int pResultDenominator, int pResultHorizontalPartitions, int pResultVerticalPartitions, bool pHighlightHorizontal, bool pHighlightVertical){
	int tLCD = mFractionModel->LCM(pOp1Denominator, pOp2Denominator);
	int tOp1ConvertedNumerator = pOp1Numerator*(pResultDenominator/pOp1Denominator);
	int tOp2ConvertedNumerator = pOp2Numerator*(pResultDenominator/pOp2Denominator);

	int tFirstPortion, tSecondPortion, tThirdPortion;
	const color *tFirstPortionColor = NULL;
	const color *tSecondPortionColor = NULL;
	const color *tThirdPortionColor = NULL;
	int tCorrectNumerator = tOp1ConvertedNumerator - tOp2ConvertedNumerator;
	if(pResultDenominator%tLCD == 0){
		if(pResultNumerator == tCorrectNumerator){
			tFirstPortion = pResultNumerator;
			tFirstPortionColor = scUNIT_BLOCK_OPERAND_1_COLOR;
			tSecondPortion = tOp2ConvertedNumerator;
			tSecondPortionColor = scUNIT_BLOCK_OPERAND_BLEND_COLOR_TRANSPARENT;
			tThirdPortion = 0;
			tThirdPortionColor = NULL;
		} else if(pResultNumerator < tCorrectNumerator){
			tFirstPortion = pResultNumerator;
			tFirstPortionColor = scUNIT_BLOCK_OPERAND_1_COLOR;
			tSecondPortion = tCorrectNumerator - pResultNumerator;
			tSecondPortionColor = scUNIT_BLOCK_OPERAND_1_COLOR_TRANSPARENT;
			tThirdPortion = tOp2ConvertedNumerator;
			tThirdPortionColor = scUNIT_BLOCK_OPERAND_BLEND_COLOR_TRANSPARENT;
		} else if (pResultNumerator > tCorrectNumerator && pResultNumerator < tOp1ConvertedNumerator){
			tFirstPortion = pResultNumerator;
			tFirstPortionColor = scUNIT_BLOCK_OPERAND_1_COLOR;
			tSecondPortion = tOp1ConvertedNumerator - pResultNumerator;
			tSecondPortionColor = scUNIT_BLOCK_OPERAND_BLEND_COLOR_TRANSPARENT;
			tThirdPortion = pResultNumerator- tCorrectNumerator;
			tThirdPortionColor =scUNIT_BLOCK_OPERAND_2_COLOR_TRANSPARENT;
		} else if (pResultNumerator < tOp1ConvertedNumerator + tOp2ConvertedNumerator) {
			tFirstPortion = tOp1ConvertedNumerator;
			tFirstPortionColor = scUNIT_BLOCK_OPERAND_1_COLOR;
			tSecondPortion = pResultNumerator - tFirstPortion;
			tSecondPortionColor = scUNIT_BLOCK_OPERAND_2_COLOR;
			tThirdPortion = tOp2ConvertedNumerator - tSecondPortion;
			tThirdPortionColor = scUNIT_BLOCK_OPERAND_2_COLOR_TRANSPARENT;
		} else {
			tFirstPortion = tOp1ConvertedNumerator;
			tFirstPortionColor = scUNIT_BLOCK_OPERAND_1_COLOR;
			tSecondPortion = tOp2ConvertedNumerator;
			tSecondPortionColor = scUNIT_BLOCK_OPERAND_2_COLOR;
			tThirdPortion = pResultNumerator - tFirstPortion - tSecondPortion;
			tThirdPortionColor = scUNIT_BLOCK_OVERFLOW_COLOR;
		}
	}

		int tNumUnitBlocks = pResultNumerator/pResultDenominator + (pResultNumerator%pResultDenominator==0?0:1);
		if(tNumUnitBlocks == 0 && pResultNumerator == 0) tNumUnitBlocks = 1;
		float tOffset = .5-((float)tNumUnitBlocks)/2-((float)tNumUnitBlocks-1)/2*(scUNIT_BLOCK_SEPARATION/scUNIT_BLOCK_WIDTH);
		pTopCornerX +=tOffset*scUNIT_BLOCK_WIDTH;
		for (int i= 0; i < tNumUnitBlocks; i++){
			if (pResultNumerator >= 0 && pResultDenominator%tLCD == 0){
				if(tFirstPortion >= pResultDenominator){
					drawUnitBlockFill(pTopCornerX, pTopCornerY, pResultHorizontalPartitions, pResultVerticalPartitions,
							pResultDenominator, 0, 0,
						tFirstPortionColor, tSecondPortionColor, tThirdPortionColor);
					tFirstPortion -=pResultDenominator;
				} else if (tFirstPortion + tSecondPortion >= pResultDenominator){
					drawUnitBlockFill(pTopCornerX, pTopCornerY, pResultHorizontalPartitions, pResultVerticalPartitions,
						tFirstPortion, pResultDenominator-tFirstPortion, 0,
						tFirstPortionColor, tSecondPortionColor, tThirdPortionColor);
					tSecondPortion -= (pResultDenominator - tFirstPortion);
					tFirstPortion = 0;
				} else if (tFirstPortion + tSecondPortion + tThirdPortion >= pResultDenominator ){
					drawUnitBlockFill(pTopCornerX, pTopCornerY, pResultHorizontalPartitions, pResultVerticalPartitions,
						tFirstPortion, tSecondPortion, pResultDenominator-tFirstPortion-tSecondPortion,
						tFirstPortionColor, tSecondPortionColor, tThirdPortionColor);
					tThirdPortion -= (pResultDenominator - tFirstPortion - tSecondPortion);
					tFirstPortion = 0;
					tSecondPortion = 0;
				} else{
					drawUnitBlockFill(pTopCornerX, pTopCornerY, pResultHorizontalPartitions, pResultVerticalPartitions,
						tFirstPortion, tSecondPortion, tThirdPortion,
						tFirstPortionColor, tSecondPortionColor, tThirdPortionColor);
				}

			} else if (pResultNumerator >= 0) {
				if(i == tNumUnitBlocks-1){
					drawUnitBlockFill(pTopCornerX, pTopCornerY,
						pResultHorizontalPartitions, pResultVerticalPartitions,
						pResultNumerator%pResultDenominator==0? pResultDenominator: pResultNumerator%pResultDenominator, 0, 0,
						scUNIT_BLOCK_RESULT_COLOR, NULL, NULL);
				} else {
					drawUnitBlockFill(pTopCornerX, pTopCornerY,
						pResultHorizontalPartitions, pResultVerticalPartitions,
						pResultDenominator, 0, 0,
						scUNIT_BLOCK_RESULT_COLOR, NULL, NULL);
				}

			}
			drawUnitBlockLines(pTopCornerX, pTopCornerY, pResultHorizontalPartitions, pResultVerticalPartitions, pHighlightHorizontal, pHighlightVertical);
			drawUnitBlockBox(pTopCornerX, pTopCornerY);
			pTopCornerX += scUNIT_BLOCK_WIDTH+scUNIT_BLOCK_SEPARATION;
		}
}


void decorators::FractionHint::drawMultiplicationUnitBlocks(FractionCard *pOperand1, FractionCard *pOperand2, FractionCard *pResult, float pVerticalOffset){

		int tOp1Numerator = pOperand1->GetNumerator();
		int tOp1Denominator = pOperand1->GetDenominator();
		int tOp2Numerator = pOperand2->GetNumerator();
		int tOp2Denominator = pOperand2->GetDenominator();
		int tResultNumerator = pResult->GetNumerator();
		int tResultDenominator = pResult->GetDenominator();

		int tResultHorizontalPartitions;
		int tResultVerticalPartitions;
		bool tBoxHorizontal = false;
		bool tBoxVertical = false;
		if(tResultDenominator%(tOp1Denominator/mFractionModel->GCD(tOp1Numerator, tOp1Denominator))  == 0){
			tBoxHorizontal = true;
			tResultHorizontalPartitions = tOp1Denominator;
			tResultVerticalPartitions = tResultDenominator/tOp1Denominator;
			if(tResultVerticalPartitions%(tOp2Denominator/mFractionModel->GCD(tOp2Numerator, tOp2Denominator))== 0) tBoxVertical = true;
		} else if (tResultDenominator%(tOp2Denominator/mFractionModel->GCD(tOp2Numerator, tOp2Denominator)) == 0) {
			tBoxVertical = true;
			tResultVerticalPartitions = tOp2Denominator;
			tResultHorizontalPartitions = tResultDenominator/tOp2Denominator;
		} else {
			mFractionModel->getClosestFactors(tResultDenominator, tResultVerticalPartitions, tResultHorizontalPartitions);
		}
		tBoxHorizontal = (tBoxHorizontal && pOperand1->GetValue() <= 1 && pOperand2->GetValue() <= 1 && pResult->GetValue() <=1);
		tBoxVertical = (tBoxVertical && pOperand1->GetValue() <= 1 && pOperand2->GetValue() <= 1 && pResult->GetValue() <=1);

		int tOp1ConvertedNumerator = tOp1Numerator*(tResultHorizontalPartitions/tOp1Denominator);
		int tOp2ConvertedNumerator = tOp2Numerator*(tResultVerticalPartitions/tOp2Denominator);


		float tTopCornerX = -scUNIT_BLOCK_WIDTH/2;
		float tTopCornerY = scHINT_GRAPHICS_OFFSET_FROM_CARD_CENTER + pVerticalOffset - scUNIT_BLOCK_WIDTH/2;

		if(pOperand1->IsPresent()){
			drawSimpleUnitBlocks(pOperand1->GetMarker(),
										 tOp1Numerator, tOp1Denominator,
										 tOp1Denominator, 1,
										 tTopCornerX, tTopCornerY,
										 scUNIT_BLOCK_OPERAND_1_COLOR, false, false);

			if(tBoxHorizontal) {
				mDecoratorManager.GetDisplay().PushTransformation();
				mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*(pOperand1->GetMarker()), scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
				outlineUnitBoxArea(tTopCornerX, tTopCornerY, tResultHorizontalPartitions, tOp1ConvertedNumerator, true, scUNIT_BLOCK_HIGHTLIGHTED_LINE_COLOR);
				mDecoratorManager.GetDisplay().PopTransformation();
			}
		}

		if (pOperand2->IsPresent()){
			drawSimpleUnitBlocks(pOperand2->GetMarker(),
													 tOp2Numerator, tOp2Denominator,
													 1, tOp2Denominator,
													 tTopCornerX, tTopCornerY,
													 scUNIT_BLOCK_OPERAND_2_COLOR, false, false);
			if(tBoxVertical){
				mDecoratorManager.GetDisplay().PushTransformation();
				mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*(pOperand2->GetMarker()), scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
				outlineUnitBoxArea(tTopCornerX, tTopCornerY, tResultVerticalPartitions, tOp2ConvertedNumerator, false, scUNIT_BLOCK_HIGHTLIGHTED_LINE_COLOR);
				mDecoratorManager.GetDisplay().PopTransformation();
			}
		}

		if(pResult->IsPresent()){
				if(tBoxHorizontal && tBoxVertical){
					mDecoratorManager.GetDisplay().PushTransformation();
					mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*(pResult->GetMarker()), scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
					drawMultiplicationUnitBlockFill(tTopCornerX, tTopCornerY, tResultHorizontalPartitions, tResultVerticalPartitions, tOp1ConvertedNumerator, tOp2ConvertedNumerator, tResultNumerator);
					drawUnitBlockLines(tTopCornerX, tTopCornerY, tResultHorizontalPartitions, tResultVerticalPartitions, false, false);
					drawUnitBlockBox(tTopCornerX, tTopCornerY);
					outlineUnitBoxArea(tTopCornerX, tTopCornerY, tResultHorizontalPartitions, tOp1ConvertedNumerator, true, scUNIT_BLOCK_HIGHTLIGHTED_LINE_COLOR);
					outlineUnitBoxArea(tTopCornerX, tTopCornerY, tResultVerticalPartitions, tOp2ConvertedNumerator, false, scUNIT_BLOCK_HIGHTLIGHTED_LINE_COLOR);
					mDecoratorManager.GetDisplay().PopTransformation();
				} else {
					drawSimpleUnitBlocks(pResult->GetMarker(),
										tResultNumerator, tResultDenominator,
										tResultHorizontalPartitions, tResultVerticalPartitions,
										tTopCornerX, tTopCornerY,
										scUNIT_BLOCK_RESULT_COLOR, false, false);

					mDecoratorManager.GetDisplay().PushTransformation();
					mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*(pResult->GetMarker()), scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
					if(tBoxHorizontal)
						outlineUnitBoxArea(tTopCornerX, tTopCornerY, tResultHorizontalPartitions, tOp1ConvertedNumerator, true, scUNIT_BLOCK_HIGHTLIGHTED_LINE_COLOR);
					if(tBoxVertical)
						outlineUnitBoxArea(tTopCornerX, tTopCornerY, tResultVerticalPartitions, tOp2ConvertedNumerator, false, scUNIT_BLOCK_HIGHTLIGHTED_LINE_COLOR);
					mDecoratorManager.GetDisplay().PopTransformation();
				}
			}
}

void decorators::FractionHint::drawMultiplicationUnitBlockFill(float pTopCornerX, float pTopCornerY, int pNumHorizontalPartitions, int pNumVerticalPartitions, int pOp1Numerator, int pOp2Numerator, int pResultNumerator){
	if(pResultNumerator != 0){
	float tHorizontalBarSpacing = scUNIT_BLOCK_WIDTH/pNumHorizontalPartitions;
		float tVerticalBarSpacing = scUNIT_BLOCK_WIDTH/pNumVerticalPartitions;
		const color *tColor = scUNIT_BLOCK_RESULT_COLOR;
		for(int i=0; i < pOp1Numerator; i++){
			for(int j = 0; j < pOp2Numerator; j++){
				mDecoratorManager.GetDisplay().RenderQuadFilled(pTopCornerX + j*tVerticalBarSpacing, pTopCornerY + i*tHorizontalBarSpacing,
																	pTopCornerX + (j+1)*tVerticalBarSpacing, pTopCornerY + i*tHorizontalBarSpacing,
																	pTopCornerX + (j+1)*tVerticalBarSpacing, pTopCornerY + (i+1)*tHorizontalBarSpacing,
																	pTopCornerX + j*tVerticalBarSpacing, pTopCornerY + (i+1)*tHorizontalBarSpacing,
																	tColor->r, tColor->g, tColor->b, tColor->a);
				pResultNumerator--;
				if(pResultNumerator == 0) return;
			}
		}
		tColor = scUNIT_BLOCK_OVERFLOW_COLOR;
		for(int i=0; i < pOp1Numerator; i++){
			for(int j = pOp2Numerator; j < pNumVerticalPartitions; j++){
				mDecoratorManager.GetDisplay().RenderQuadFilled(pTopCornerX + j*tVerticalBarSpacing, pTopCornerY + i*tHorizontalBarSpacing,
																	pTopCornerX + (j+1)*tVerticalBarSpacing, pTopCornerY + i*tHorizontalBarSpacing,
																	pTopCornerX + (j+1)*tVerticalBarSpacing, pTopCornerY + (i+1)*tHorizontalBarSpacing,
																	pTopCornerX + j*tVerticalBarSpacing, pTopCornerY + (i+1)*tHorizontalBarSpacing,
																	tColor->r, tColor->g, tColor->b, tColor->a);
				pResultNumerator--;
				if(pResultNumerator == 0) return;
			}
		}

		for(int i = pOp1Numerator; i < pNumHorizontalPartitions; i++){
			for(int j = 0; j < pNumVerticalPartitions; j++){
				mDecoratorManager.GetDisplay().RenderQuadFilled(pTopCornerX + j*tVerticalBarSpacing, pTopCornerY + i*tHorizontalBarSpacing,
																			pTopCornerX + (j+1)*tVerticalBarSpacing, pTopCornerY + i*tHorizontalBarSpacing,
																			pTopCornerX + (j+1)*tVerticalBarSpacing, pTopCornerY + (i+1)*tHorizontalBarSpacing,
																			pTopCornerX + j*tVerticalBarSpacing, pTopCornerY + (i+1)*tHorizontalBarSpacing,
																			tColor->r, tColor->g, tColor->b, tColor->a);
				pResultNumerator--;
				if(pResultNumerator == 0) return;
			}
		}
	}
}

void decorators::FractionHint::drawDivisionUnitBlocks(FractionCard *pOperand1, FractionCard *pOperand2, FractionCard *pResult, float pVerticalOffset){
	float tTopCornerX = -scUNIT_BLOCK_WIDTH/2;
	float tTopCornerY = scHINT_GRAPHICS_OFFSET_FROM_CARD_CENTER + pVerticalOffset - scUNIT_BLOCK_WIDTH/2;

	if(pOperand1->IsPresent()){
		drawSimpleUnitBlocks(pOperand1->GetMarker(),
							 pOperand1->GetNumerator(), pOperand1->GetDenominator(),
							 pOperand1->GetDenominator(), 1,
							 tTopCornerX, tTopCornerY,
							 scUNIT_BLOCK_OPERAND_1_COLOR, false, false);
	}

	if (pOperand2->IsPresent()){
		drawSimpleUnitBlocks(pOperand2->GetMarker(),
									 pOperand2->GetNumerator(), pOperand2->GetDenominator(),
									 1, pOperand2->GetDenominator(),
									 tTopCornerX, tTopCornerY,
									 scUNIT_BLOCK_OPERAND_2_COLOR, false, false);
	}

	if(pResult->IsPresent()){
		int tResultNumerator = pResult->GetNumerator();
		int tResultDenominator = pResult->GetDenominator();
		int tResultHorizontalPartitions;
		int tResultVerticalPartitions;
		mFractionModel->getClosestFactors(tResultDenominator, tResultVerticalPartitions, tResultHorizontalPartitions);
		drawSimpleUnitBlocks(pResult->GetMarker(),
							 tResultNumerator, tResultDenominator,
							 tResultHorizontalPartitions, tResultVerticalPartitions,
							 tTopCornerX, tTopCornerY,
							 scUNIT_BLOCK_RESULT_COLOR, false, false);
	}
}

void decorators::FractionHint::drawSimpleUnitBlocks(const FiducialMarker *pMarker, int pNumerator, int pDenominator, int pHorizontalPartitions, int pVerticalPartitions, float pTopCornerX, float pTopCornerY, const color *pFillColor, bool pHighlightHorizontal, bool pHighlightVertical){
	mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*pMarker, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
		int tNumUnitBlocks = pNumerator/pDenominator + (pNumerator%pDenominator==0?0:1);
		if(tNumUnitBlocks == 0 && pNumerator == 0) tNumUnitBlocks = 1;
		float tOffset = .5-((float)tNumUnitBlocks)/2-((float)tNumUnitBlocks-1)/2*(scUNIT_BLOCK_SEPARATION/scUNIT_BLOCK_WIDTH);
		pTopCornerX +=tOffset*scUNIT_BLOCK_WIDTH;
		for (int i= 0; i < tNumUnitBlocks; i++){
			if(i == tNumUnitBlocks-1 && pNumerator > 0) drawUnitBlockFill(pTopCornerX, pTopCornerY, pHorizontalPartitions, pVerticalPartitions, pNumerator%pDenominator==0? pDenominator: pNumerator%pDenominator, 0, 0, pFillColor, NULL, NULL);
			else if(pNumerator > 0)drawUnitBlockFill(pTopCornerX, pTopCornerY, 1, 1, pDenominator, 0, 0, pFillColor, NULL, NULL); //Draw a completely filled block
			drawUnitBlockLines(pTopCornerX, pTopCornerY, pHorizontalPartitions, pVerticalPartitions, pHighlightHorizontal, pHighlightVertical);
			drawUnitBlockBox(pTopCornerX, pTopCornerY);
			pTopCornerX += scUNIT_BLOCK_WIDTH+scUNIT_BLOCK_SEPARATION;
		}
	mDecoratorManager.GetDisplay().PopTransformation();

}


void decorators::FractionHint::outlineUnitBoxArea(float pTopCornerX, float pTopCornerY, int pTotalPartitions, int pNumPartitionsWide, bool pHorizontal, const color *pColor){
	float tPartitionSize = scUNIT_BLOCK_WIDTH/pTotalPartitions;
	if(pHorizontal){
		mDecoratorManager.GetDisplay().RenderLine(pTopCornerX, pTopCornerY,
							pTopCornerX + scUNIT_BLOCK_WIDTH, pTopCornerY,
							pColor->r, pColor->g, pColor->b, pColor->a);
		mDecoratorManager.GetDisplay().RenderLine(pTopCornerX, pTopCornerY + tPartitionSize*pNumPartitionsWide,
							pTopCornerX + scUNIT_BLOCK_WIDTH, pTopCornerY + tPartitionSize*pNumPartitionsWide,
							pColor->r, pColor->g, pColor->b, pColor->a);
	}else {
		mDecoratorManager.GetDisplay().RenderLine(pTopCornerX, pTopCornerY,
							pTopCornerX, pTopCornerY + scUNIT_BLOCK_WIDTH,
							pColor->r, pColor->g, pColor->b, pColor->a);
		mDecoratorManager.GetDisplay().RenderLine(pTopCornerX + tPartitionSize*pNumPartitionsWide, pTopCornerY,
							pTopCornerX + tPartitionSize*pNumPartitionsWide, pTopCornerY + scUNIT_BLOCK_WIDTH,
							pColor->r, pColor->g, pColor->b, pColor->a);
	}
}

void decorators::FractionHint::drawUnitBlockFill(float pTopCornerX, float pTopCornerY, int pNumHorizontalPartitions, int pNumVerticalPartitions, int pNumerator1, int pNumerator2, int pNumerator3, const color *pColor1, const color *pColor2, const color *pColor3){
	float tHorizontalBarSpacing = scUNIT_BLOCK_WIDTH/pNumHorizontalPartitions;
	float tVerticalBarSpacing = scUNIT_BLOCK_WIDTH/pNumVerticalPartitions;

	const color *tColor = pColor1;
	for(int i=0; i < pNumHorizontalPartitions; i++){
			for(int j = 0; j < pNumVerticalPartitions; j++){
				if(pNumerator1 > 0) pNumerator1--;
				else if(pNumerator2 > 0) {
					pNumerator2--;
					tColor = pColor2;
				} else if (pNumerator3 >0){
					pNumerator3 --;
					tColor = pColor3;
				} else {
					return;
				}

				mDecoratorManager.GetDisplay().RenderQuadFilled(pTopCornerX + j*tVerticalBarSpacing, pTopCornerY + i*tHorizontalBarSpacing,
																pTopCornerX + (j+1)*tVerticalBarSpacing, pTopCornerY + i*tHorizontalBarSpacing,
																pTopCornerX + (j+1)*tVerticalBarSpacing, pTopCornerY + (i+1)*tHorizontalBarSpacing,
																pTopCornerX + j*tVerticalBarSpacing, pTopCornerY + (i+1)*tHorizontalBarSpacing,
																tColor->r, tColor->g, tColor->b, tColor->a);

				if(pNumerator1 <= 0 && pNumerator2<=0 && pNumerator3 <=0) break;
			}
			if(pNumerator1 <= 0 && pNumerator2<=0 && pNumerator3 <=0) break;
	}
}
void decorators::FractionHint::drawUnitBlockLines(float pTopCornerX, float pTopCornerY, int pNumHorizontalPartitions, int pNumVerticalPartitions, bool pHighlightHorizontal, bool pHighlightVertical){

	float tHorizontalBarSpacing = scUNIT_BLOCK_WIDTH/pNumHorizontalPartitions;
	float tVerticalBarSpacing = scUNIT_BLOCK_WIDTH/pNumVerticalPartitions;

	const color *tColor = pHighlightHorizontal? scUNIT_BLOCK_HIGHTLIGHTED_LINE_COLOR: scUNIT_BLOCK_OUTLINE_COLOR;
	for(int i = 1; i < pNumHorizontalPartitions; i++){ //Horizontal Lines
				mDecoratorManager.GetDisplay().RenderLine(pTopCornerX, pTopCornerY + i*tHorizontalBarSpacing,
									pTopCornerX + scUNIT_BLOCK_WIDTH, pTopCornerY + i*tHorizontalBarSpacing,
									tColor->r, tColor->g, tColor->b, tColor->a);

	}
	tColor = pHighlightVertical?scUNIT_BLOCK_HIGHTLIGHTED_LINE_COLOR: scUNIT_BLOCK_OUTLINE_COLOR;
	for(int i = 1; i < pNumVerticalPartitions; i++){ //Vertical Lines
		mDecoratorManager.GetDisplay().RenderLine(pTopCornerX + i*tVerticalBarSpacing, pTopCornerY,
									pTopCornerX + i*tVerticalBarSpacing, pTopCornerY + scUNIT_BLOCK_WIDTH,
									tColor->r, tColor->g, tColor->b, tColor->a);
	}

}


void decorators::FractionHint::drawUnitBlockBox(float pTopCornerX, float pTopCornerY){
	mDecoratorManager.GetDisplay().RenderQuad(pTopCornerX, pTopCornerY,
							pTopCornerX+scUNIT_BLOCK_WIDTH, pTopCornerY,
							pTopCornerX+scUNIT_BLOCK_WIDTH, pTopCornerY + scUNIT_BLOCK_WIDTH,
							pTopCornerX, pTopCornerY + scUNIT_BLOCK_WIDTH,
							scUNIT_BLOCK_OUTLINE_COLOR->r, scUNIT_BLOCK_OUTLINE_COLOR->g, scUNIT_BLOCK_OUTLINE_COLOR->b, scUNIT_BLOCK_OUTLINE_COLOR->a);
}



void decorators::FractionHint::drawPieChart(int pNumerator,int pDenominator,
												float pX, float pY, float pRadius,
												const color *pFillColor, const color *pOutlineColor){



	int tNumUnitBlocks = pNumerator/pDenominator + (pNumerator%pDenominator==0?0:1);
	if(tNumUnitBlocks == 0 && pNumerator == 0) tNumUnitBlocks = 1;
	float tOffset = .5-((float)tNumUnitBlocks)/2;
	for (int i= 0; i < tNumUnitBlocks; i++){
		mDecoratorManager.GetDisplay().RenderEllipse(pX + (tOffset+i)*scHINT_GRAPHICS_ICONS_WIDTH*2, pY, pRadius, pRadius, pOutlineColor->r, pOutlineColor->g, pOutlineColor->b, pOutlineColor->a);
		if(i == tNumUnitBlocks-1){
			int tNumerator = pNumerator%pDenominator == 0? pDenominator: pNumerator%pDenominator;
			mDecoratorManager.GetDisplay().RenderFilledSector(pX + (tOffset+i)*scHINT_GRAPHICS_ICONS_WIDTH*2, pY, pRadius, pRadius, scDEGREES_IN_CIRCLE*((float)tNumerator/pDenominator), 0.0f, pFillColor->r, pFillColor->g, pFillColor->b, pFillColor->a);
		}
		else{
			mDecoratorManager.GetDisplay().RenderFilledSector(pX + (tOffset+i)*scHINT_GRAPHICS_ICONS_WIDTH*2, pY, pRadius, pRadius, scDEGREES_IN_CIRCLE, 0.0f, pFillColor->r, pFillColor->g, pFillColor->b, pFillColor->a);
		}
	}
}

void decorators::FractionHint::drawPieChartWithPieces(int pNumerator, int pDenominator,
														float pX, float pY, float pRadius,
														const color *pFillColor, const color *pOutlineColor){
	drawPieChart(pNumerator, pDenominator, pX, pY, pRadius, pFillColor, pOutlineColor);

	if(pDenominator > 1){
		int tNumUnitBlocks = pNumerator/pDenominator + (pNumerator%pDenominator==0?0:1);
		if(tNumUnitBlocks == 0 && pNumerator == 0) tNumUnitBlocks = 1;
		float tOffset = .5-((float)tNumUnitBlocks)/2;
		for (int j= 0; j < tNumUnitBlocks; j++){
			float tSegmentRadians = scRADIANS_IN_CIRCLE/pDenominator;
			for(int i = 0; i < pDenominator; i++){
				mDecoratorManager.GetDisplay().RenderLine(pX + (tOffset+j)*scHINT_GRAPHICS_ICONS_WIDTH*2, pY, pX+(tOffset+j)*scHINT_GRAPHICS_ICONS_WIDTH*2+ pRadius*cos(i*tSegmentRadians), pY+pRadius*sin(i*tSegmentRadians), pOutlineColor->r, pOutlineColor->g, pOutlineColor->b, pOutlineColor->a);
			}
		}
	}
}

void decorators::FractionHint::drawFilledBarFraction(int pNumerator, int pDenominator,
														float pX, float pY, float pWidth, float pHeight,
														const color *pFillColor, const color *pOutlineColor){


	int tNumFilledBars = pNumerator/pDenominator + (pNumerator%pDenominator==0?0:1);
	if(tNumFilledBars == 0 && pNumerator == 0) tNumFilledBars = 1;
	float tOffset = .5-((float)tNumFilledBars)/2;
	for (int i= 0; i < tNumFilledBars; i++){
		float tX = pX + (tOffset+i)*scHINT_GRAPHICS_ICONS_WIDTH*2;
		if(i == tNumFilledBars-1){
			int tNumerator = pNumerator%pDenominator == 0? pDenominator: pNumerator%pDenominator;
			mDecoratorManager.GetDisplay().RenderQuadFilled(tX - pWidth/2, pY - pHeight/2,
						tX + pWidth/2, pY - pHeight/2,
						tX + pWidth/2, pY - pHeight/2 + pHeight*tNumerator/pDenominator,
						tX - pWidth/2, pY - pHeight/2 + pHeight*tNumerator/pDenominator,
						pFillColor->r, pFillColor->g, pFillColor->b, pFillColor->a);
			mDecoratorManager.GetDisplay().RenderLine(tX - pWidth/2, pY - pHeight/2 + pHeight*tNumerator/pDenominator,
						tX + pWidth/2, pY - pHeight/2 + pHeight*tNumerator/pDenominator,
						pOutlineColor->r, pOutlineColor->g, pOutlineColor->b, pOutlineColor->a);
		}else{
			mDecoratorManager.GetDisplay().RenderQuadFilled(tX - pWidth/2, pY - pHeight/2,
						tX + pWidth/2, pY - pHeight/2,
						tX + pWidth/2, pY + pHeight/2,
						tX - pWidth/2, pY + pHeight/2,
						pFillColor->r, pFillColor->g, pFillColor->b, pFillColor->a);
		}
		mDecoratorManager.GetDisplay().RenderQuad(tX - pWidth/2, pY - pHeight/2,
						tX + pWidth/2, pY - pHeight/2,
						tX + pWidth/2, pY + pHeight/2,
						tX - pWidth/2, pY + pHeight/2,
						pOutlineColor->r, pOutlineColor->g, pOutlineColor->b, pOutlineColor->a);
	}


}

/*This does NOT work for fractions greater than 1*/
void decorators::FractionHint::drawShadedPieces(int pNumerator, int pDenominator,
													float pX, float pY,
													const color *pFillColor, const color *pOutlineColor)
{
	float tRadius = scFRACTION_PIECE_SIZE;
	int tColumns, tRows;
	mFractionModel->getClosestFactors(pDenominator, tColumns, tRows);
	for (int i = 0; i < tRows; i++){
		for(int j = 0; j < tColumns; j++){
			if(i*tColumns + j < pNumerator) mDecoratorManager.GetDisplay().RenderFilledEllipse(pX - (tColumns-1)*tRadius + j*2*tRadius, pY + i*2*tRadius, tRadius, tRadius, pFillColor->r, pFillColor->g, pFillColor->b, pFillColor->a);
			mDecoratorManager.GetDisplay().RenderEllipse(pX - (tColumns-1)*tRadius + j*2*tRadius, pY + i*2*tRadius, tRadius, tRadius, pOutlineColor->r, pOutlineColor->g, pOutlineColor->b, pOutlineColor->a);
		}
	}
}

void decorators::FractionHint::displayDecimalRepresentation(int pNumerator, int pDenominator, float pX, float pY, const color *pColor){
	char buffer[scDECIMAL_HINT_PRECISION+1];
	sprintf((char *)&buffer, "%.3f", ((float)pNumerator)/pDenominator);
	mDecoratorManager.GetDisplay().RenderCenteredText((char *)&buffer, pX, pY, true, scDECIMAL_HINT_SCALE, pColor->r, pColor->g, pColor->b, pColor->a);
}


void decorators::FractionHint::displaySingleFraction(int pNumerator, int pDenominator,
														float pX, float pY,
														float pWidth, float pHeight, float pScale,
														const color *pNumeratorColor, const color *pDenominatorColor, const color *pFractionBarColor){

	mDecoratorManager.GetDisplay().RenderLine(pX - .5*pWidth, pY, pX + .5*pWidth, pY, pFractionBarColor->r, pFractionBarColor->g, pFractionBarColor->b, pFractionBarColor->a);
	char tNumeratorText[scMAX_NUMERATOR_DIGITS + 1];
	char tDenominatorText[scMAX_DENOMINATOR_DIGITS + 1];
	sprintf(tNumeratorText, "%d", pNumerator);
	sprintf(tDenominatorText, "%d", pDenominator);
	mDecoratorManager.GetDisplay().RenderCenteredText(tNumeratorText, pX, pY - pHeight/3, true, pScale, pNumeratorColor->r, pNumeratorColor->g, pNumeratorColor->b, pNumeratorColor->a);
	mDecoratorManager.GetDisplay().RenderCenteredText(tDenominatorText, pX, pY + pHeight/3, true, pScale, pDenominatorColor->r, pDenominatorColor->g, pDenominatorColor->b, pDenominatorColor->a);

}


void decorators::FractionHint::displaySingleFractionWithLabels(int pNumerator, int pDenominator, const char *pNumeratorLabel, const char *pDenominatorLabel, float pXOffset, float pYOffset){
	const color *tNumeratorColor = scNUMERATOR_DIAL_COLOR;
	const color *tDenominatorColor = scDENOMINATOR_DIAL_COLOR;
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarker, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
	mDecoratorManager.GetDisplay().Translate(pXOffset, pYOffset);
	mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(pNumeratorLabel, scTEXT_DELIMITERS,
			scHINT_GRAPHICS_WIDTH/4, -scHINT_GRAPHICS_ICONS_HEIGHT/3,
			 scHINT_GRAPHICS_WIDTH/2, false, scHINT_GRAPHICS_FRACTIONS_LABELS_SCALE,
			tNumeratorColor->r, tNumeratorColor->g, tNumeratorColor->b, tNumeratorColor->a);
	mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(pDenominatorLabel, scTEXT_DELIMITERS,
				scHINT_GRAPHICS_WIDTH/4, scHINT_GRAPHICS_ICONS_HEIGHT/3,
				scHINT_GRAPHICS_WIDTH/2, false, scHINT_GRAPHICS_FRACTIONS_LABELS_SCALE,
				tDenominatorColor->r, tDenominatorColor->g, tDenominatorColor->b, tDenominatorColor->a);
	displaySingleFraction(pNumerator, pDenominator, -scHINT_GRAPHICS_WIDTH/4, 0, scHINT_GRAPHICS_ICONS_WIDTH, scHINT_GRAPHICS_ICONS_HEIGHT, scHINT_GRAPHICS_FRACTIONS_SCALE, tNumeratorColor, tDenominatorColor, scFRACTION_BAR_COLOR);
	mDecoratorManager.GetDisplay().PopTransformation();


}


void decorators::FractionHint::displayTwoFractionsWithGraphics(int pNumerator1, int pDenominator1, int pNumerator2, int pDenominator2, HintType pType, float pXOffset, float pYOffset){

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarker, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
	mDecoratorManager.GetDisplay().Translate(pXOffset, pYOffset);
	displaySingleFraction(pNumerator1, pDenominator1, - scHINT_GRAPHICS_WIDTH/4, 0, scHINT_GRAPHICS_ICONS_WIDTH, scHINT_GRAPHICS_ICONS_HEIGHT, scHINT_GRAPHICS_FRACTIONS_SCALE, scNUMERATOR_DIAL_COLOR, scDENOMINATOR_DIAL_COLOR, scFRACTION_BAR_COLOR);

	displaySingleFraction(pNumerator2, pDenominator2, scHINT_GRAPHICS_WIDTH/4, 0, scHINT_GRAPHICS_ICONS_WIDTH, scHINT_GRAPHICS_ICONS_HEIGHT, scHINT_GRAPHICS_FRACTIONS_SCALE, scNUMERATOR_DIAL_COLOR, scDENOMINATOR_DIAL_COLOR, scFRACTION_BAR_COLOR);

	graphicallyRepresentFraction(pType, pNumerator1, pDenominator1,
											-scHINT_GRAPHICS_WIDTH/4, scHINT_GRAPHICS_VERTICAL_OFFSET,
											scHINT_GRAPHICS_ICONS_WIDTH, scHINT_GRAPHICS_ICONS_HEIGHT);

	graphicallyRepresentFraction(pType, pNumerator2, pDenominator2,
											scHINT_GRAPHICS_WIDTH/4, scHINT_GRAPHICS_VERTICAL_OFFSET,
											scHINT_GRAPHICS_ICONS_WIDTH, scHINT_GRAPHICS_ICONS_HEIGHT);
	mDecoratorManager.GetDisplay().PopTransformation();
}
