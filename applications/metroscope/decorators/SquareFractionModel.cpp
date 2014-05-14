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

#include "SquareFractionModel.hpp"
#include <iostream>

const std::string decorators::SquareFractionModel::scDecoratorName("SquareFractionModel");
const DecoratorManager::Registerer decorators::SquareFractionModel::mRegisterer(decorators::SquareFractionModel::scDecoratorName, &decorators::SquareFractionModel::create);

decorators::FiducialDecorator *decorators::SquareFractionModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager){
	try{ //DecoratorManager &pDecoratorManager, SquareFractionCard ** pCards, int pCardNumber
		libconfig::Setting & tCardsString = pSetting["cards"];
		int tCardsNumber = pSetting["num_cards"];
		SquareFractionCard **tCards = new SquareFractionCard *[tCardsNumber];

		for(int i = 0 ; i < tCardsNumber ; i++){
			tCards[i] = (SquareFractionCard *)pDecoratorManager.loadDecorator(tCardsString[i]);
		}

		return new decorators::SquareFractionModel(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), tCards, tCardsNumber);

	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::SquareFractionModel::SquareFractionModel(DecoratorManager &pDecoratorManager,FiducialMarker * pMarker, SquareFractionCard **pCards, int pCardNumber):
		FiducialDecorator(pDecoratorManager, pMarker),
		mNumberActiveCards(0),
		mActiveFirstCard(0),
		mActiveSecondCard(0),
		mActualDenominator(0),
		mCardNumber(pCardNumber),
		mCards (pCards){

			for(int i = 0 ; i < mCardNumber ; i++){
				switch(mCards[i]->getType()){
				case 0: mFirstCards.push_back(mCards[i]); break;
				case 1: mSecondCards.push_back(mCards[i]); break;
				}
			}
}
 /*
decorators::SquareFractionModel::~SquareFractionModel(){
	delete []mCards;
}*/

void decorators::SquareFractionModel::update(){
	mActiveCards.clear();
	mNumberActiveCards = 0;
	mActiveFirstCard = 0;
	mActiveSecondCard = 0;

	for(int i = 0; i < mCardNumber; i++){
		if(mCards[i]->IsPresent()){
			mNumberActiveCards++;
			mActiveCards.push_back(mCards[i]);
			if(mCards[i]->getType() == 0) mActiveFirstCard++;
			else mActiveSecondCard++;
		}
	}
}


bool decorators::SquareFractionModel::isPresent(){
	return (mMarker->isPresent());
}

float decorators::SquareFractionModel::getProportion(){
	int tFirstColor = 0;
	int tSecondColor = 0;
	int tDenominator = 0;
	float tProportion = 0;
	wykobi::point2d<float> tMarkerLocation = mMarker->getCenter();

	if(mNumberActiveCards > 0){
		tDenominator = mActiveCards[0]->getDenom();

		for(int i = 0 ; i < mNumberActiveCards ; i++){
			////
			wykobi::point2d<float> tCardLocation = mActiveCards[i]->GetLocation();


			mDecoratorManager.GetCam2World().InterpolatedMap(tCardLocation);
			mDecoratorManager.GetCam2World().InterpolatedMap(tMarkerLocation);

			float distance = wykobi::distance(tCardLocation.x,tCardLocation.y, tMarkerLocation.x, tMarkerLocation.y);

			if(distance > 40.0f && distance < 70.0f){
				int tType = mActiveCards[i]->getType();
				if(tType == 0) {
					tFirstColor++;
				}else{
					tSecondColor++;
				}
			}
		}
		}

		tProportion = (float)(tFirstColor/(float)tDenominator);

		//if(mNumberActiveCards == tDenominator) return tProportion;
		if((tFirstColor + tSecondColor) == tDenominator) return tProportion;


	return 0;

}
