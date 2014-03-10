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
		if(mNumberModel->AreCardsInsideRectangles(120.0f,50.0f,478.0f,50.0f)){
			SetNumbers();
		}
		if(mNumbersAreSet){
			DrawDigits();
		}
		DrawNumbersAndLines();

	}
}

void decorators::BlankActivityCard::ShowInstruction(){
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderText("Put the cards in the boxes", 300.0f, 700.0f, 1.0f,255.0f, 0.0f, 0.0f, 1.0f);
	mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::BlankActivityCard::DrawRectangles(){
	mDecoratorManager.GetDisplay().PushTransformation();
	//mDecoratorManager.GetDisplay().RenderQuadFilled(120.0f,50.0f,200.0f,50.0f,200.0f,150.0f,120.0f,150.0f,255.0f,255.0f,0.0f,0.40f);//1st yellow
	mDecoratorManager.GetDisplay().RenderQuad(120.0f,50.0f,200.0f,50.0f,200.0f,150.0f,120.0f,150.0f,255.0f,255.0f,0.0f,1.0f); //1st yellow

	//mDecoratorManager.GetDisplay().RenderQuadFilled(201.0f,50.0f,281.0f,50.0f,281.0f,150.0f,201.0f,150.0f,0.0f,153.0f,0.0f,0.40f);//1st green
	mDecoratorManager.GetDisplay().RenderQuad(201.0f,50.0f,281.0f,50.0f,281.0f,150.0f,201.0f,150.0f,0.0f,153.0f,0.0f,1.0f); //1st greeen

	//mDecoratorManager.GetDisplay().RenderQuadFilled(282.0f,50.0f,362.0f,50.0f,362.0f,150.0f,282.0f,150.0f,0.0f,0.0f,204.0f,0.40f);//1st blue
	mDecoratorManager.GetDisplay().RenderQuad(282.0f,50.0f,362.0f,50.0f,362.0f,150.0f,282.0f,150.0f,0.0f,0.0f,204.0f,1.0f); //1st blue

	mDecoratorManager.GetDisplay().RenderCenteredText("+",420.0f,100.0f,true,3.0f,0.0f,0.0f,0.0f,1.0f);

	//mDecoratorManager.GetDisplay().RenderQuadFilled(478.0f,50.0f,558.0f,50.0f,558.0f,150.0f,478.0f,150.0f,255.0f,255.0f,0.0f,0.40f);
	mDecoratorManager.GetDisplay().RenderQuad(478.0f,50.0f,558.0f,50.0f,558.0f,150.0f,478.0f,150.0f,255.0f,255.0f,0.0f,1.0f); //2nd yellow

	//mDecoratorManager.GetDisplay().RenderQuadFilled(559.0f,50.0f,639.0f,50.0f,639.0f,150.0f,559.0f,150.0f,0.0f,153.0f,0.0f,0.40f);
	mDecoratorManager.GetDisplay().RenderQuad(559.0f,50.0f,639.0f,50.0f,639.0f,150.0f,559.0f,150.0f,0.0f,153.0f,0.0f,1.0f); //2nd greeen

	//mDecoratorManager.GetDisplay().RenderQuadFilled(640.0f,50.0f,720.0f,50.0f,720.0f,150.0f,640.0f,150.0f,0.0f,0.0f,204.0f,.4f);
	mDecoratorManager.GetDisplay().RenderQuad(640.0f,50.0f,720.0f,50.0f,720.0f,150.0f,640.0f,150.0f,0.0f,0.0f,204.0f,1.0f); //2nd blue
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
	/*int tCent1 = mFirstSummand/100;
		int tCent2 = mSecondSummand/100;
		int tTen1 = (mFirstSummand/10)%10;
		int tTen2 = (mSecondSummand/10)%10;
		int tUnit1 = mFirstSummand%10;
		int tUnit2 = mSecondSummand%10;*/
	//bool tCent1IsUsed = false;
	//bool tCent2IsUsed = false;
	//bool tTen1IsUsed = false;
	//bool tTen2IsUsed = false;
	//bool tUnit1IsUsed = false;
	//bool tUnit2IsUsed = false;

	/*
	char tCent1Text[1];
	char tCent2Text[1];
	char tTen1Text[1];
	char tTen2Text[1];
	char tUnit1Text[1];
	char tUnit2Text[1];

	sprintf(tCent1Text, "%d", tCent1);
	sprintf(tCent2Text, "%d", tCent2);

	sprintf(tTen1Text, "%d", tTen1);
	sprintf(tTen2Text, "%d", tTen2);

	sprintf(tUnit1Text, "%d", tUnit1);
	sprintf(tUnit2Text, "%d", tUnit2);*/

	std::vector<BlankNumberCard *> tActiveCards = mNumberModel->GetActiveCards();
	for(unsigned int i = 0; i < tActiveCards.size(); i++){
			mDecoratorManager.GetDisplay().PushTransformation();

			switch(tActiveCards[i]->mType){
			case 2://cents
				if(tActiveCards[i]->GetLocation().x < 200) tActiveCards[i]->SetNumber(tCent1);
				else tActiveCards[i]->SetNumber(tCent2);
				break;
			case 1: //tens
				if(tActiveCards[i]->GetLocation().x < 281) tActiveCards[i]->SetNumber(tTen1);
				else tActiveCards[i]->SetNumber(tTen2);
				break;
			case 0: //units
				if(tActiveCards[i]->GetLocation().x < 362) tActiveCards[i]->SetNumber(tUnit1);
				else tActiveCards[i]->SetNumber(tUnit2);
				break;
			}
			/*
			switch(tActiveCards[i]->mType){
			case 2: //cents
				if(!tCent1IsUsed){
					tActiveCards[i]->SetNumber(tCent1);
					tCent1IsUsed = true;
				} else {
					tActiveCards[i]->SetNumber(tCent2);
				} break;
			case 1: //tens
				if(!tTen1IsUsed){
					tActiveCards[i]->SetNumber(tTen1);
					tTen1IsUsed = true;
				} else {
					tActiveCards[i]->SetNumber(tTen2);
				} break;
			case 0: //units
				if(!tUnit1IsUsed){
					tActiveCards[i]->SetNumber(tUnit1);
					tUnit1IsUsed = true;
				} else {
					tActiveCards[i]->SetNumber(tUnit2);
				} break;
			}*/
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
		if(!mNumberModel->AreCardsInsideRectangles(120.0f,50.0f,478.0f,50.0f)){
			tLocation = tActiveCards[i]->GetLocation();
			switch(tActiveCards[i]->mType){
			case 2:
				sprintf(tNumberText, "%d", (tActiveCards[i]->GetNumber()*100));

				if(tActiveCards[i]->GetNumber() == tCent1) mDecoratorManager.GetDisplay().RenderLine(160.0f,150.0f,tLocation.x+5.0f, tLocation.y+5.0f, 0.0f,0.0f,0.0f,1.0f);
				else mDecoratorManager.GetDisplay().RenderLine(518.0f,150.0f,tLocation.x+5.0f, tLocation.y+5.0f, 0.0f,0.0f,0.0f,1.0f);
			break;
			case 1:
				sprintf(tNumberText, "%d", (tActiveCards[i]->GetNumber()*10));

				if(tActiveCards[i]->GetNumber() == tTen1) mDecoratorManager.GetDisplay().RenderLine(241.0f,150.0f,tLocation.x+5.0f, tLocation.y+5.0f, 0.0f,0.0f,0.0f,1.0f);
				else mDecoratorManager.GetDisplay().RenderLine(599.0f,150.0f,tLocation.x+5.0f, tLocation.y+5.0f, 0.0f,0.0f,0.0f,1.0f);
			break;
			case 0:
				sprintf(tNumberText, "%d", tActiveCards[i]->GetNumber());

				if(tActiveCards[i]->GetNumber() == tUnit1) mDecoratorManager.GetDisplay().RenderLine(322.0f,150.0f,tLocation.x+5.0f, tLocation.y+5.0f, 0.0f,0.0f,0.0f,1.0f);
				else mDecoratorManager.GetDisplay().RenderLine(680.0f,150.0f,tLocation.x+5.0f, tLocation.y+5.0f, 0.0f,0.0f,0.0f,1.0f);
			}
		}else {
				sprintf(tNumberText, "%d", tActiveCards[i]->GetNumber());
		}
			mDecoratorManager.GetDisplay().RenderText(tNumberText, tActiveCards[i]->GetLocation().x+20.0f,tActiveCards[i]->GetLocation().y+25.0f,1.0f,tActiveCards[i]->r,tActiveCards[i]->g,tActiveCards[i]->b,1.0f);

		}
	}

	mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::BlankActivityCard::DrawDigits(){
	mDecoratorManager.GetDisplay().PushTransformation();
	//Cent 1
	char* tDigitText = new char [1];
	sprintf(tDigitText, "%d",tCent1);
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,160.0f,100.0f,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	//Cent 2
	sprintf(tDigitText, "%d",tCent2);
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,518.0f,100.0f,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	//Ten 1
	sprintf(tDigitText, "%d",tTen1);
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,241.0f,100.0f,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	//Ten 2
	sprintf(tDigitText, "%d",tTen2);
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,599.0f,100.0f,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	//Unit 1
	sprintf(tDigitText, "%d",tUnit1);
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,322.0f,100.0f,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	//Unit 2
	sprintf(tDigitText, "%d",tUnit2);
	mDecoratorManager.GetDisplay().RenderCenteredText(tDigitText,680.0f,100.0f,true,4.0f,0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().PopTransformation();
}

/*
 *

 */

/*
 *




void decorators::NumberActivityCard::drawGroups(){

	std::vector<NumberCard *> tActiveCardsRectangle = mNumberModel->GetActiveCards();

	for (unsigned int i = 0; i < tActiveCardsRectangle.size(); i++){

		std::vector<NumberCard *> tCardsInside = mNumberModel->GetGroupOf(tActiveCardsRectangle[i]);

		if(tCardsInside.size()== 2){
			wykobi::point2d<float> tFirstCardLocation = tCardsInside[0]->GetLocation();
			wykobi::point2d<float> tSecondCardLocation = tCardsInside[1]->GetLocation();

			DrawEllipseOutsideCards(tFirstCardLocation, tSecondCardLocation, tCardsInside[0]->GetNumber(),tCardsInside[1]->GetNumber());
		}
	}
}

void decorators::NumberActivityCard::DrawEllipseOutsideCards(wykobi::point2d<float> pFirstCard, wykobi::point2d<float> pSecondCard, int pFirstNumber, int pSecondNumber){

	float tXPoint = (pFirstCard.x + pSecondCard.x)/2.0f;
	float tYPoint = (pFirstCard.y + pSecondCard.y)/2.0f;

	int sum = pFirstNumber + pSecondNumber;
	char* mSum= new char[2];
	sprintf (mSum, "%d", sum);

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderEllipse(tXPoint,tYPoint, 80.0f,80.0f,0.0f,0.0f,0.0f,1.0f,1);
	mDecoratorManager.GetDisplay().RenderText(mSum, tXPoint,tYPoint+50.0f,1.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().PopTransformation();
}



void ShowInstruction();
void ShowActiveCards();
void DrawLines();
void DrawNumbers();
void DrawRectangles();
 *
 */
