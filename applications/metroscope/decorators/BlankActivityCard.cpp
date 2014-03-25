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
		return new decorators::BlankActivityCard(pDecoratorManager,
						pDecoratorManager.loadMarker(pSetting["marker"]),
						(BlankNumberModel *)pDecoratorManager.loadDecorator(pSetting["number_model"]),
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
		const int pFirstSummand,
		const int pSecondSummand):
			FiducialDecorator(pDecoratorManager, pMarker),
			mNumberModel(pModel),
			mFirstSummand(pFirstSummand),
			mSecondSummand(pSecondSummand),
			mNumbersAreSet(false)
			{
				tCent1 = mFirstSummand/100;
				tCent2 = mSecondSummand/100;
				tTen1 = (mFirstSummand/10)%10;
				tTen2 = (mSecondSummand/10)%10;
				tUnit1 = mFirstSummand%10;
				tUnit2 = mSecondSummand%10;
			}

decorators::BlankActivityCard::~BlankActivityCard(){

}

void decorators::BlankActivityCard::update() {
	if(mMarker->isPresent()){
		ShowInstruction();
		DrawRectangles();
		ShowActiveCards();
		if(mNumberModel->AreCardsInsideRectangles(120.0f,250.0f,535.0f,250.0f)&& !mNumbersAreSet){
			SetNumbers();
		}
		if(mNumbersAreSet){
			DrawDigits();
			DrawNumbersAndLines();
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
		mDecoratorManager.GetDisplay().RenderText(".", tActiveCards[i]->GetLocation().x-15.0f,tActiveCards[i]->GetLocation().y-15.0f,1.0f,tActiveCards[i]->r,tActiveCards[i]->g,tActiveCards[i]->b,1.0f);
		mDecoratorManager.GetDisplay().PopTransformation();
	}
}

void decorators::BlankActivityCard::SetNumbers(){

	std::vector<BlankNumberCard *> tActiveCards = mNumberModel->GetActiveCards();
	for(unsigned int i = 0; i < tActiveCards.size(); i++){
			mDecoratorManager.GetDisplay().PushTransformation();

			switch(tActiveCards[i]->mType){
			case 2://cents
				if(tActiveCards[i]->GetLocation().x < 220) tActiveCards[i]->SetNumber(tCent1);
				else tActiveCards[i]->SetNumber(tCent2);
				break;
			case 1: //tens
				if(tActiveCards[i]->GetLocation().x < 320) tActiveCards[i]->SetNumber(tTen1);
				else tActiveCards[i]->SetNumber(tTen2);
				break;
			case 0: //units
				if(tActiveCards[i]->GetLocation().x < 420) tActiveCards[i]->SetNumber(tUnit1);
				else tActiveCards[i]->SetNumber(tUnit2);
				break;
			}
		}
	mNumbersAreSet = true;

}

void decorators::BlankActivityCard::DrawNumbersAndLines(){
	if(mNumbersAreSet){
	char * tNumberText = new char[3];
	wykobi::point2d<float> tLocation;
	std::vector<BlankNumberCard *> tActiveCards = mNumberModel->GetActiveCards();

	mDecoratorManager.GetDisplay().PushTransformation();
	for(unsigned int i = 0; i < tActiveCards.size(); i++){
		if(!mNumberModel->AreCardsInsideRectangles(scCENT1_X1,scY1,scCENT2_X1,scY1)){
			tLocation = tActiveCards[i]->GetLocation();
			switch(tActiveCards[i]->mType){
			case 2:
				sprintf(tNumberText, "%d", (tActiveCards[i]->GetNumber()*100));
				if(tActiveCards[i]->GetNumber() == tCent1) mDecoratorManager.GetDisplay().RenderLine(scCENT1_X_AVG,scY2,tLocation.x+5.0f, tLocation.y+5.0f, 0.0f,0.0f,0.0f,1.0f);
				else mDecoratorManager.GetDisplay().RenderLine(scCENT2_X_AVG,scY2,tLocation.x+5.0f, tLocation.y+5.0f, 0.0f,0.0f,0.0f,1.0f);
			break;

			case 1:
				sprintf(tNumberText, "%d", (tActiveCards[i]->GetNumber()*10));
				if(tActiveCards[i]->GetNumber() == tTen1) mDecoratorManager.GetDisplay().RenderLine(scTEN1_X_AVG,scY2,tLocation.x+5.0f, tLocation.y+5.0f, 0.0f,0.0f,0.0f,1.0f);
				else mDecoratorManager.GetDisplay().RenderLine(scTEN2_X_AVG,scY2,tLocation.x+5.0f, tLocation.y+5.0f, 0.0f,0.0f,0.0f,1.0f);
			break;

			case 0:
				sprintf(tNumberText, "%d", tActiveCards[i]->GetNumber());
				if(tActiveCards[i]->GetNumber() == tUnit1) mDecoratorManager.GetDisplay().RenderLine(scUNIT1_X_AVG,scY2,tLocation.x+5.0f, tLocation.y+5.0f, 0.0f,0.0f,0.0f,1.0f);
				else mDecoratorManager.GetDisplay().RenderLine(scUNIT2_X_AVG,scY2,tLocation.x+5.0f, tLocation.y+5.0f, 0.0f,0.0f,0.0f,1.0f);
			}
		}else {
				sprintf(tNumberText, "%d", tActiveCards[i]->GetNumber());
		}
		mDecoratorManager.GetDisplay().RenderText(tNumberText, tLocation.x-30.0f,tLocation.y+80.0f,3.0f,tActiveCards[i]->r,tActiveCards[i]->g,tActiveCards[i]->b,1.0f);
		}
	}
	mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::BlankActivityCard::DrawDigits(){
	mDecoratorManager.GetDisplay().PushTransformation();
	char* tDigitText = new char [1];

	//Cent 1
	sprintf(tDigitText, "%d",tCent1);
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,scCENT1_X_AVG,scY_AVG,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	//Cent 2
	sprintf(tDigitText, "%d",tCent2);
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,scCENT2_X_AVG,scY_AVG,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	//Ten 1
	sprintf(tDigitText, "%d",tTen1);
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,scTEN1_X_AVG,scY_AVG,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	//Ten 2
	sprintf(tDigitText, "%d",tTen2);
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,scTEN2_X_AVG,scY_AVG,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	//Unit 1
	sprintf(tDigitText, "%d",tUnit1);
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,scUNIT1_X_AVG,scY_AVG,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	//Unit 2
	sprintf(tDigitText, "%d",tUnit2);
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,scUNIT2_X_AVG,scY_AVG,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().PopTransformation();
}

