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

#include "NumberActivityCard.hpp"
#include <iostream>


const std::string decorators::NumberActivityCard::scDecoratorName("NumberActivityCard");
namespace{

}

decorators::NumberActivityCard::NumberActivityCard(DecoratorManager &pDecoratorManager,
			FiducialMarker *pMarker,
			NumberModel *pModel,
			const char * const pActivityName):
				FiducialDecorator(pDecoratorManager, pMarker),
				mNumberModel(pModel),
				mActivityName(pActivityName)
				{
				}

decorators::NumberActivityCard::~NumberActivityCard(){

}

void decorators::NumberActivityCard::update() {
	if(mMarker->isPresent()){
		showInstruction();
		showActiveCards();
		drawGroups();
	}
}

void decorators::NumberActivityCard::showInstruction(){
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderText("Group the numbers that add up XX", 300.0f, 700.0f, 1.0f,0.0f, 0.0f, 0.0f, 1.0f);
	mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::NumberActivityCard::showActiveCards(){
	std::vector<NumberCard *> tActiveCards = mNumberModel->GetActiveCards();

	for(unsigned int i = 0; i < tActiveCards.size(); i++){
		mDecoratorManager.GetDisplay().PushTransformation();
		char* mSNumber= new char[2];
		sprintf (mSNumber, "%d", tActiveCards[i]->GetNumber());
		mDecoratorManager.GetDisplay().RenderText(mSNumber, tActiveCards[i]->GetLocation().x,tActiveCards[i]->GetLocation().y,1.0f,15.0f,0.0f,0.0f,1.0f);
		mDecoratorManager.GetDisplay().PopTransformation();

	}
}

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


