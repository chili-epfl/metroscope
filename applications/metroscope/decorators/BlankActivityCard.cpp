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

#include "BlankActivityCard.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>
#include <wykobi/wykobi_utilities.hpp>

const std::string decorators::BlankActivityCard::scDecoratorName("BlankActivityCard");
const  DecoratorManager::Registerer decorators::BlankActivityCard::mRegisterer(decorators::BlankActivityCard::scDecoratorName, &decorators::BlankActivityCard::create);

namespace {

}

decorators::FiducialDecorator *decorators::BlankActivityCard::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::BlankActivityCard(pDecoratorManager,
						pDecoratorManager.loadMarker(pSetting["marker"]),
						(BlankNumberModel *)pDecoratorManager.loadDecorator(pSetting["number_model"]),
						(RegroupDigits *)pDecoratorManager.loadDecorator(pSetting["regroup_digits"]),
						pSetting["first_summand"],
						pSetting["second_summand"]);

	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::BlankActivityCard::BlankActivityCard(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, BlankNumberModel *pModel, RegroupDigits *pRegroup, const int pFirstSummand, const int pSecondSummand):
			FiducialDecorator(pDecoratorManager, pMarker),
			mNumberModel(pModel),
			mRegroupDigits(pRegroup),
			mFirstSummand(pFirstSummand),
			mSecondSummand(pSecondSummand),
			mNumbersAreSet(false),
			mIsSolutionUnitCorrect(false),
			mIsSolutionTenCorrect (false),
			mIsSolutionCentCorrect (false),
			mIsTraslationStartedU2T(false),
			mInitialTimeTraslationU2T(0),
			mIsTraslationDoneU2T(false),
			mIsTraslationStartedT2C(false),
			mInitialTimeTraslationT2C(0),
			mIsTraslationDoneT2C(false),
			mAreUnitsStacked(false),
			mAreTensStacked(false),
			mAreCentsStacked(false)
			{
				tCent1 = mFirstSummand/100;
				tCent2 = mSecondSummand/100;
				tTen1 = (mFirstSummand/10)%10;
				tTen2 = (mSecondSummand/10)%10;
				tUnit1 = mFirstSummand%10;
				tUnit2 = mSecondSummand%10;

				mCentsSum = (tCent1 + tCent2)*100;
				mTensSum = (tTen1 + tTen2)*10;
				mUnitsSum = tUnit1 + tUnit2;

				mSolutionUnit = (mFirstSummand + mSecondSummand)%10;
				mSolutionTen = ((mFirstSummand + mSecondSummand)/10)%10;
				mSolutionCent = (mFirstSummand + mSecondSummand)/100;

				mIsNecessaryU2T = (tUnit1 + tUnit2) != mSolutionUnit;
				mIsNecessaryT2C = (tTen1 + tTen2) != mSolutionTen;
			}

decorators::BlankActivityCard::~BlankActivityCard(){

}

void decorators::BlankActivityCard::update() {

	if(mMarker->isPresent()){
		ShowInstruction();
		DrawRectangles();
		ShowActiveCards();

		if(mNumberModel->AreCardsInsideRectangles() && !mNumbersAreSet)	SetNumbers();

		if(mNumbersAreSet){
			CheckSolution();
			CheckSemiStack();

			if(mRegroupDigits->IsGrouperPresent())	UpdateDigits();

			if(mNumberModel->IsAStackNearRectangle())	CheckSolution(); FillSolutionRectangles();

			if(mNumberModel ->AreCardsInsideSolution())	DrawSolutionDigits();

			DrawDigits();
			DrawNumbersAndLines();
		}
	}
}

void decorators::BlankActivityCard::CheckSemiStack(){
	mAreUnitsStacked = mNumberModel->AreCardsSemiStacked(0);
	mAreTensStacked = mNumberModel->AreCardsSemiStacked(1);
	mAreCentsStacked = mNumberModel->AreCardsSemiStacked(2);
}

void decorators::BlankActivityCard::ShowInstruction(){
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderText(scINSTRUCTION_TEXT, scINSTRUCTION_X, scINSTRUCTION_Y, 1.0f,255.0f, 0.0f, 0.0f, 1.0f);
	mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::BlankActivityCard::DrawRectangles(){
	mDecoratorManager.GetDisplay().PushTransformation();

	mDecoratorManager.GetDisplay().RenderQuad(scCENT1_X1,scY1,scCENT1_X2,scY1,scCENT1_X2,scY2,scCENT1_X1,scY2,scYELLOW_R,scYELLOW_G,scYELLOW_B, 1.0f);
	mDecoratorManager.GetDisplay().RenderQuad(scTEN1_X1,scY1,scTEN1_X2,scY1,scTEN1_X2,scY2,scTEN1_X1,scY2,scGREEN_R,scGREEN_G,scGREEN_B, 1.0f);
	mDecoratorManager.GetDisplay().RenderQuad(scUNIT1_X1,scY1,scUNIT1_X2,scY1,scUNIT1_X2,scY2,scUNIT1_X1,scY2,scBLUE_R,scBLUE_G,scBLUE_B, 1.0f);

	mDecoratorManager.GetDisplay().RenderCenteredText("+",scPLUS_X,scY_AVG,true,3.0f,0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().RenderQuad(scCENT2_X1,scY1,scCENT2_X2,scY1,scCENT2_X2,scY2,scCENT2_X1,scY2,scYELLOW_R,scYELLOW_G,scYELLOW_B, 1.0f);
	mDecoratorManager.GetDisplay().RenderQuad(scTEN2_X1,scY1,scTEN2_X2,scY1,scTEN2_X2,scY2,scTEN2_X1,scY2,scGREEN_R,scGREEN_G,scGREEN_B, 1.0f);
	mDecoratorManager.GetDisplay().RenderQuad(scUNIT2_X1,scY1,scUNIT2_X2,scY1,scUNIT2_X2,scY2,scUNIT2_X1,scY2,scBLUE_R,scBLUE_G,scBLUE_B, 1.0f);

	mDecoratorManager.GetDisplay().RenderCenteredText("=",scEQUAL_X,scY_AVG,true,3.0f,0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().RenderQuad(scCENT_SOLUTION_X1,scY1,scCENT_SOLUTION_X2,scY1,scCENT_SOLUTION_X2,scY2,scCENT_SOLUTION_X1,scY2,scYELLOW_R,scYELLOW_G,scYELLOW_B, 1.0f);
	mDecoratorManager.GetDisplay().RenderQuad(scTEN_SOLUTION_X1,scY1,scTEN_SOLUTION_X2,scY1,scTEN_SOLUTION_X2,scY2,scTEN_SOLUTION_X1,scY2,scGREEN_R,scGREEN_G,scGREEN_B, 1.0f);
	mDecoratorManager.GetDisplay().RenderQuad(scUNIT_SOLUTION_X1,scY1,scUNIT_SOLUTION_X2,scY1,scUNIT_SOLUTION_X2,scY2,scUNIT_SOLUTION_X1,scY2,scBLUE_R,scBLUE_G,scBLUE_B, 1.0f);

	mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::BlankActivityCard::TranslateNumber(int pType){

	switch(pType){
		case 0:{
			if(!mIsTraslationStartedU2T){
				mInitialTimeTraslationU2T = Time::MillisTimestamp();
				mIsTraslationStartedU2T = true;
			}

			wykobi::point2d<float> tOriginU2T;
			tOriginU2T.x = mNumberModel->GetStackXAvg(0);
			tOriginU2T.y = mNumberModel->GetStackYAvg(0);

			wykobi::point2d<float> tEndU2T;
			tEndU2T.x = mNumberModel->GetStackXAvg(1);
			tEndU2T.y = mNumberModel->GetStackYAvg(1);

			mDecoratorManager.GetCam2World().InterpolatedMap(tOriginU2T);
			mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOriginU2T);
			mDecoratorManager.GetCam2World().InterpolatedMap(tEndU2T);
			mDecoratorManager.GetWorld2Proj().InterpolatedMap(tEndU2T);

			long tElapsedTime = Time::MillisTimestamp() - mInitialTimeTraslationU2T;
			static const long cTraslationTime = 10l*650l;

			if(tElapsedTime <= cTraslationTime && mIsTraslationStartedU2T){
				float tPartialX = tOriginU2T.x + (tEndU2T.x - tOriginU2T.x)*(tElapsedTime/(float)(cTraslationTime));
				float tPartialY = tOriginU2T.y + (tEndU2T.y - tOriginU2T.y)*(tElapsedTime/(float)(cTraslationTime));
				mIsTraslationDoneU2T = false;

				char * tDigit = new char[3];
				sprintf(tDigit, "%d", mUnitsSum/10);
				float tPartialFactor = 3.0 - (1.5)*(tElapsedTime/(float)(cTraslationTime));

				mDecoratorManager.GetDisplay().RenderText(tDigit, tPartialX,tPartialY,tPartialFactor,0.0f,0.0f,0.0f,1.0f);

			}else if (tElapsedTime > cTraslationTime && mIsTraslationStartedU2T){
				mIsTraslationDoneU2T = true;
			}
		break;
		}
		case 1:{
			if(!mIsTraslationStartedT2C){
				mInitialTimeTraslationT2C = Time::MillisTimestamp();
				mIsTraslationStartedT2C = true;
			}

			wykobi::point2d<float> tOriginT2C;
			tOriginT2C.x = mNumberModel->GetStackXAvg(1);
			tOriginT2C.y = mNumberModel->GetStackYAvg(1);

			wykobi::point2d<float> tEndT2C;
			tEndT2C.x = mNumberModel->GetStackXAvg(2);
			tEndT2C.y = mNumberModel->GetStackYAvg(2);

			mDecoratorManager.GetCam2World().InterpolatedMap(tOriginT2C);
			mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOriginT2C);
			mDecoratorManager.GetCam2World().InterpolatedMap(tEndT2C);
			mDecoratorManager.GetWorld2Proj().InterpolatedMap(tEndT2C);

			long tElapsedTime = Time::MillisTimestamp() - mInitialTimeTraslationT2C;
			static const long cTraslationTime = 10l*650l;

			if(tElapsedTime <= cTraslationTime && mIsTraslationStartedT2C){
				float tPartialX = tOriginT2C.x + (tEndT2C.x - tOriginT2C.x)*(tElapsedTime/(float)(cTraslationTime));
				float tPartialY = tOriginT2C.y + (tEndT2C.y - tOriginT2C.y)*(tElapsedTime/(float)(cTraslationTime));
				mIsTraslationDoneT2C = false;

				char * tDigit = new char[3];
				sprintf(tDigit, "%d", mTensSum/100);
				float tPartialFactor = 3.0 - (1.5)*(tElapsedTime/(float)(cTraslationTime));

				mDecoratorManager.GetDisplay().RenderText(tDigit, tPartialX,tPartialY,tPartialFactor,0.0f,0.0f,0.0f,1.0f);

			}else if (tElapsedTime > cTraslationTime && mIsTraslationStartedT2C){
				mIsTraslationDoneT2C = true;
			}
			break;
		}
	}
}

void decorators::BlankActivityCard::UpdateDigits(){

	if(!mIsSolutionUnitCorrect){
		if (mRegroupDigits->IsGrouperPresent(0)) {

			bool isAnimationU2TDone = mRegroupDigits->RegroupAnimation(0, mIsNecessaryU2T, mAreUnitsStacked, mAreTensStacked);

			if(isAnimationU2TDone && mAreUnitsStacked && mAreTensStacked && mIsNecessaryU2T){
				TranslateNumber(0);

				if(mIsTraslationDoneU2T){
					mUnitsSum%=10;
					mTensSum+=10;
					mIsNecessaryU2T = false;
				}
			}
		}
	}

	if(!mIsSolutionTenCorrect){
		if (mRegroupDigits->IsGrouperPresent(1)) {

			bool a = mNumberModel->AreCardsSemiStacked(0);
			bool b = mNumberModel->AreCardsSemiStacked(1);
			bool c = mIsNecessaryU2T && a && b ;

			bool isAnimationT2CDone = mRegroupDigits->RegroupAnimation(1, mIsNecessaryT2C, mAreTensStacked, mAreCentsStacked);

			if(isAnimationT2CDone && mIsNecessaryT2C && mAreTensStacked && mAreCentsStacked){
				TranslateNumber(1);

				if(mIsTraslationDoneU2T){
					mTensSum%=100;
					mCentsSum+=100;
					mIsNecessaryT2C = false;
				}
			}
		}
	}
}

void decorators::BlankActivityCard::ShowActiveCards(){
	std::vector<BlankNumberCard *> tActiveCards = mNumberModel->GetActiveCards();

	for(unsigned int i = 0; i < tActiveCards.size(); i++){
		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*(tActiveCards[i]->GetMarker()), 19.0f, 19.0f, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
		mDecoratorManager.GetDisplay().RenderText(".", 1.5f,1.5f,1.0f,tActiveCards[i]->mR,tActiveCards[i]->mG,tActiveCards[i]->mB,1.0f);
		mDecoratorManager.GetDisplay().PopTransformation();
	}
}

void decorators::BlankActivityCard::SetNumbers(){

	std::vector<BlankNumberCard *> tActiveCards = mNumberModel->GetActiveCards();

	if(tActiveCards.size()==6){

		for(unsigned int i = 0; i < tActiveCards.size(); i++){
			int tCardNumber = 0;
			int tCardOperator = 0;

			switch(tActiveCards[i]->mType){
			case 2://cents
				tCardNumber = (tActiveCards[i]->GetLocation().x < scTEN1_X1) ? tCent1 : tCent2;
				tCardOperator = (tActiveCards[i]->GetLocation().x < scTEN1_X1) ? 0 : 1;
			break;
			case 1: //tens
				tCardNumber = (tActiveCards[i]->GetLocation().x < scUNIT1_X1) ? tTen1 : tTen2;
				tCardOperator = (tActiveCards[i]->GetLocation().x < scUNIT1_X1) ? 0 : 1;
			break;
			case 0: //units
				tCardNumber = (tActiveCards[i]->GetLocation().x < scUNIT1_X2) ? tUnit1 : tUnit2;
				tCardOperator = (tActiveCards[i]->GetLocation().x < scUNIT1_X2) ? 0 : 1;
			break;
			}

			tActiveCards[i]->SetNumber(tCardNumber);
			tActiveCards[i]->SetOperator(tCardOperator);
		}
	}
	mNumbersAreSet = true;
}

void decorators::BlankActivityCard::DrawNumbersAndLines(){

	mNumberModel->DrawNumbers(mCentsSum, mTensSum, mUnitsSum, mSolutionCent, mSolutionTen, mSolutionUnit,(tCent1+tCent2),(tTen1+tTen2),mIsTraslationDoneU2T, mIsTraslationDoneT2C);

}

void decorators::BlankActivityCard::CheckSolution(){

	mIsSolutionUnitCorrect = (mUnitsSum == mSolutionUnit);
	mIsSolutionTenCorrect = ((mTensSum/10) == mSolutionTen);
	mIsSolutionCentCorrect = ((mCentsSum/100) == mSolutionCent);

	mIsNecessaryU2T = !mIsSolutionUnitCorrect;
	mIsNecessaryT2C = !mIsSolutionTenCorrect;

}

void decorators::BlankActivityCard::FillSolutionRectangles(){

	mDecoratorManager.GetDisplay().PushTransformation();

	if (mNumberModel->IsStackNearRectangle(0) && mIsSolutionUnitCorrect) mDecoratorManager.GetDisplay().RenderQuadFilled(scUNIT_SOLUTION_X1,scY1,scUNIT_SOLUTION_X2,scY1,scUNIT_SOLUTION_X2,scY2,scUNIT_SOLUTION_X1,scY2,scBLUE_R,scBLUE_G,scBLUE_B, 0.25f);
	else if((mNumberModel->IsStackNearRectangle(0) || mNumberModel->IsStackInsideSolution(0)) && !mIsSolutionUnitCorrect) mDecoratorManager.GetDisplay().RenderQuadFilled(scUNIT_SOLUTION_X1,scY1,scUNIT_SOLUTION_X2,scY1,scUNIT_SOLUTION_X2,scY2,scUNIT_SOLUTION_X1,scY2,scRED_R,scRED_G,scRED_B, 0.25f);

	if (mNumberModel->IsStackNearRectangle(1) && mIsSolutionTenCorrect) mDecoratorManager.GetDisplay().RenderQuadFilled(scTEN_SOLUTION_X1,scY1,scTEN_SOLUTION_X2,scY1,scTEN_SOLUTION_X2,scY2,scTEN_SOLUTION_X1,scY2,scGREEN_R,scGREEN_G,scGREEN_B, 0.25f);
	else if ((mNumberModel->IsStackNearRectangle(1) || mNumberModel->IsStackInsideSolution(1)) && !mIsSolutionTenCorrect) mDecoratorManager.GetDisplay().RenderQuadFilled(scTEN_SOLUTION_X1,scY1,scTEN_SOLUTION_X2,scY1,scTEN_SOLUTION_X2,scY2,scTEN_SOLUTION_X1,scY2,scRED_R,scRED_G,scRED_B, 0.25f);

	if (mNumberModel->IsStackNearRectangle(2) && mIsSolutionCentCorrect) mDecoratorManager.GetDisplay().RenderQuadFilled(scCENT_SOLUTION_X1,scY1,scCENT_SOLUTION_X2,scY1,scCENT_SOLUTION_X2,scY2,scCENT_SOLUTION_X1,scY2,scYELLOW_R,scYELLOW_G,scYELLOW_B, 0.25f);
	else if ((mNumberModel->IsStackNearRectangle(2) || mNumberModel->IsStackInsideSolution(2)) && !mIsSolutionCentCorrect) mDecoratorManager.GetDisplay().RenderQuadFilled(scCENT_SOLUTION_X1,scY1,scCENT_SOLUTION_X2,scY1,scCENT_SOLUTION_X2,scY2,scCENT_SOLUTION_X1,scY2,scRED_R,scRED_G,scRED_B, 0.25f);

	mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::BlankActivityCard::DrawDigits(){

	mDecoratorManager.GetDisplay().PushTransformation();

	char* tDigitText = new char [1];

	sprintf(tDigitText, "%d",tCent1);//Cent 1
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,scCENT1_X_AVG,scY_AVG,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	sprintf(tDigitText, "%d",tCent2);//Cent 2
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,scCENT2_X_AVG,scY_AVG,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	sprintf(tDigitText, "%d",tTen1);//Ten 1
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,scTEN1_X_AVG,scY_AVG,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	sprintf(tDigitText, "%d",tTen2);//Ten 2
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,scTEN2_X_AVG,scY_AVG,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	sprintf(tDigitText, "%d",tUnit1);//Unit 1
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,scUNIT1_X_AVG,scY_AVG,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	sprintf(tDigitText, "%d",tUnit2); //Unit 2
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,scUNIT2_X_AVG,scY_AVG,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::BlankActivityCard::DrawSolutionDigits(){
	mDecoratorManager.GetDisplay().PushTransformation();
	char* tDigitSolutionText = new char [1];

	if(mNumberModel->IsStackInsideSolution(2)){
		if(mIsSolutionCentCorrect){
			sprintf(tDigitSolutionText, "%d",mSolutionCent);
			mDecoratorManager.GetDisplay().RenderCenteredText(tDigitSolutionText,scCENT_SOLUTION_X_AVG,scY_AVG,true,4.0f,0.0f,0.0f,0.0f,1.0f);
			mDecoratorManager.GetDisplay().RenderQuadFilled(scCENT_SOLUTION_X1,scY1,scCENT_SOLUTION_X2,scY1,scCENT_SOLUTION_X2,scY2,scCENT_SOLUTION_X1,scY2,255.0f,255.0f,255.0f, 0.0f);
		}else{
			mDecoratorManager.GetDisplay().RenderQuadFilled(scCENT_SOLUTION_X1,scY1,scCENT_SOLUTION_X2,scY1,scCENT_SOLUTION_X2,scY2,scCENT_SOLUTION_X1,scY2,scRED_R,scRED_G,scRED_B, 0.25f);
		}
	}

	if(mNumberModel->IsStackInsideSolution(1)){
		if(mIsSolutionTenCorrect){
			sprintf(tDigitSolutionText, "%d",mSolutionTen);
			mDecoratorManager.GetDisplay().RenderCenteredText(tDigitSolutionText,scTEN_SOLUTION_X_AVG,scY_AVG,true,4.0f,0.0f,0.0f,0.0f,1.0f);
			mDecoratorManager.GetDisplay().RenderQuadFilled(scTEN_SOLUTION_X1,scY1,scTEN_SOLUTION_X2,scY1,scTEN_SOLUTION_X2,scY2,scTEN_SOLUTION_X1,scY2,255.0f,255.0f,255.0f, 0.0f);
		}else{
			mDecoratorManager.GetDisplay().RenderQuadFilled(scTEN_SOLUTION_X1,scY1,scTEN_SOLUTION_X2,scY1,scTEN_SOLUTION_X2,scY2,scTEN_SOLUTION_X1,scY2,scRED_R,scRED_G,scRED_B, 0.25f);
		}
	}


	if(mNumberModel->IsStackInsideSolution(0)){
		if(mIsSolutionUnitCorrect){
			sprintf(tDigitSolutionText, "%d",mSolutionUnit);
			mDecoratorManager.GetDisplay().RenderCenteredText(tDigitSolutionText,scUNIT_SOLUTION_X_AVG,scY_AVG,true,4.0f,0.0f,0.0f,0.0f,1.0f);
			mDecoratorManager.GetDisplay().RenderQuadFilled(scUNIT_SOLUTION_X1,scY1,scUNIT_SOLUTION_X2,scY1,scUNIT_SOLUTION_X2,scY2,scUNIT_SOLUTION_X1,scY2,255.0f,255.0f,255.0f, 0.0f);
		}else{
			mDecoratorManager.GetDisplay().RenderQuadFilled(scUNIT_SOLUTION_X1,scY1,scUNIT_SOLUTION_X2,scY1,scUNIT_SOLUTION_X2,scY2,scUNIT_SOLUTION_X1,scY2,scRED_R,scRED_G,scRED_B, 0.25f);
		}
	}

	mDecoratorManager.GetDisplay().PopTransformation();
}
