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

const std::string decorators::BlankActivityCard::scDecoratorName("BlankActivityCard");
const  DecoratorManager::Registerer decorators::BlankActivityCard::mRegisterer(decorators::BlankActivityCard::scDecoratorName, &decorators::BlankActivityCard::create);

namespace {

}

decorators::FiducialDecorator *decorators::BlankActivityCard::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tFlippersStrings = pSetting["flippers"];
		Flipper **tFlippers = new Flipper *[2];
		tFlippers [0] = (Flipper *)pDecoratorManager.loadDecorator(tFlippersStrings[0]);
		tFlippers [1] = (Flipper *)pDecoratorManager.loadDecorator(tFlippersStrings[1]);

		return new decorators::BlankActivityCard(pDecoratorManager,
						pDecoratorManager.loadMarker(pSetting["marker"]),
						(BlankNumberModel *)pDecoratorManager.loadDecorator(pSetting["number_model"]),
						tFlippers,
						pSetting["first_summand"],
						pSetting["second_summand"]);
			} catch(libconfig::SettingNotFoundException &e) {
				std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
			} catch(libconfig::SettingTypeException &e) {
				std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
			}
			return 0;
}

decorators::BlankActivityCard::BlankActivityCard(DecoratorManager &pDecoratorManager,
		FiducialMarker *pMarker,
		BlankNumberModel *pModel,
		Flipper **pFlipper,
		const int pFirstSummand,
		const int pSecondSummand):
			FiducialDecorator(pDecoratorManager, pMarker),
			mNumberModel(pModel),
			mFlippers (pFlipper),
			mFirstSummand(pFirstSummand),
			mSecondSummand(pSecondSummand),
			mNumbersAreSet(false),
			mLastShot(Time::MillisTimestamp())
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
			}

decorators::BlankActivityCard::~BlankActivityCard(){

}

void decorators::BlankActivityCard::update() {
	if(mMarker->isPresent()){
		ShowInstruction();
		DrawRectangles();
		ShowActiveCards();

		if(mNumberModel->AreCardsInsideRectangles() && !mNumbersAreSet){
			SetNumbers();
		}

		if(mNumbersAreSet){
			if(!mNumberModel->AreCardsInsideRectangles())	DrawNumbersAndLines();
			DrawDigits();
		}
	}
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

void decorators::BlankActivityCard::ShowActiveCards(){
	std::vector<BlankNumberCard *> tActiveCards = mNumberModel->GetActiveCards();

	for(unsigned int i = 0; i < tActiveCards.size(); i++){
		mDecoratorManager.GetDisplay().PushTransformation();
		//mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*(tActiveCards[i]->GetMarker()), 20.0f, 20.0f, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
		mDecoratorManager.GetDisplay().RenderText(".", tActiveCards[i]->GetLocation().x,tActiveCards[i]->GetLocation().y,1.0f,tActiveCards[i]->r,tActiveCards[i]->g,tActiveCards[i]->b,1.0f);
		mDecoratorManager.GetDisplay().PopTransformation();
	}
}

void decorators::BlankActivityCard::SetNumbers(){

	std::vector<BlankNumberCard *> tActiveCards = mNumberModel->GetActiveCards();
	for(unsigned int i = 0; i < tActiveCards.size(); i++){
		switch(tActiveCards[i]->mType){
		case 2://cents
			if(tActiveCards[i]->GetLocation().x < scTEN1_X1) tActiveCards[i]->SetNumber(tCent1);
			else tActiveCards[i]->SetNumber(tCent2);
		break;
		case 1: //tens
			if(tActiveCards[i]->GetLocation().x < scUNIT1_X1) tActiveCards[i]->SetNumber(tTen1);
			else tActiveCards[i]->SetNumber(tTen2);
		break;
		case 0: //units
			if(tActiveCards[i]->GetLocation().x < scUNIT1_X2) tActiveCards[i]->SetNumber(tUnit1);
			else tActiveCards[i]->SetNumber(tUnit2);
		break;
		}
	}
	mNumbersAreSet = true;
}

void decorators::BlankActivityCard::DrawNumbersAndLines(){

	if(mNumbersAreSet){
		char * tNumberText = new char[3];
		char * tNumberSumText = new char [3];
		float tFactor = 0.0f;
		float tFactorSum = 0.0f;
		float tLocationX = 0.0f;
		float tLocationY = 0.0f;

	wykobi::point2d<float> tLocation;
	std::vector<BlankNumberCard *> tActiveCards = mNumberModel->GetActiveCards();

	mDecoratorManager.GetDisplay().PushTransformation();

	for(unsigned int i = 0; i < tActiveCards.size(); i++){
		bool tNumberIsStacked =false ;
		bool tNumberHasSum = false;
		float tLineOrigin = 0.0;
		if(!mNumberModel->AreCardsInsideRectangles()){
			tLocation = tActiveCards[i]->GetLocation();

			switch(tActiveCards[i]->mType){

			case 2:
				sprintf(tNumberText, "%d", (tActiveCards[i]->GetNumber()*100));
				sprintf(tNumberSumText,"%d",mCentsSum);

				tNumberHasSum = (tActiveCards[i]->GetNumber() == tCent1);
				tNumberIsStacked = (mNumberModel -> AreCardsSemiStacked(2));
				tLineOrigin = (tActiveCards[i]->GetNumber() == tCent1) ? scCENT1_X_AVG : scCENT2_X_AVG;

				break;

			case 1:
				sprintf(tNumberText, "%d", (tActiveCards[i]->GetNumber()*10));
				sprintf(tNumberSumText,"%d",mTensSum);

				tNumberHasSum = (tActiveCards[i]->GetNumber() == tTen1);
				tNumberIsStacked = (mNumberModel -> AreCardsSemiStacked(1));
				tLineOrigin = (tActiveCards[i]->GetNumber() == tTen1) ? scTEN1_X_AVG : scTEN2_X_AVG;

				break;

			case 0:
				sprintf(tNumberText, "%d", tActiveCards[i]->GetNumber());
				sprintf(tNumberSumText,"%d",mUnitsSum);

				tNumberHasSum = (tActiveCards[i]->GetNumber() == tUnit1);
				tNumberIsStacked = (mNumberModel -> AreCardsSemiStacked(0));
				tLineOrigin = (tActiveCards[i]->GetNumber() == tUnit1) ? scUNIT1_X_AVG : scUNIT2_X_AVG;

				break;
			}

			mDecoratorManager.GetDisplay().RenderLine(tLineOrigin,scY2,tLocation.x, tLocation.y, 0.0f,0.0f,0.0f,1.0f);

			//mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*(tActiveCards[i]->GetMarker()), 20.0f, 20.0f, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());



			tLocationX = (!tNumberIsStacked) ? tLocation.x-30.0f : tLocation.x+30.0f;
			tLocationY = (!tNumberIsStacked) ? tLocation.y + 80.0f: tLocation.y;
			tFactor = (!tNumberIsStacked) ? 3.0f : 1.0f;
			tFactorSum = (tNumberHasSum && tNumberIsStacked) ? 3.0 : 1.0f;

			if(tNumberHasSum && tNumberIsStacked){

				mDecoratorManager.GetDisplay().RenderText(tNumberSumText, tLocation.x,tLocation.y+50.0f,tFactorSum,tActiveCards[i]->r,tActiveCards[i]->g,tActiveCards[i]->b,1.0f);
			}

			mDecoratorManager.GetDisplay().RenderText(tNumberText, tLocationX,tLocationY, tFactor,tActiveCards[i]->r,tActiveCards[i]->g,tActiveCards[i]->b,1.0f);

			}
		}
	}
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

