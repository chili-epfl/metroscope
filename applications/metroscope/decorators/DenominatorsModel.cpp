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

#include "DenominatorsModel.hpp"
#include <iostream>

const std::string decorators::DenominatorsModel::scDecoratorName("DenominatorsModel");
const DecoratorManager::Registerer decorators::DenominatorsModel::mRegisterer(decorators::DenominatorsModel::scDecoratorName, &decorators::DenominatorsModel::create);

decorators::FiducialDecorator *decorators::DenominatorsModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager){
	try{
			libconfig::Setting & tCardStrings = pSetting["cards"];
			int tCards = pSetting["num_cards"];
			DenominatorCard **tDenominatorCards = new DenominatorCard *[tCards];

			for(int i = 0; i < tCards ; i++){
				tDenominatorCards[i] = (DenominatorCard *)pDecoratorManager.loadDecorator(tCardStrings[i]);
			}

			return new decorators::DenominatorsModel(pDecoratorManager, tDenominatorCards, tCards);

		}catch(libconfig::SettingNotFoundException &e) {
			std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
		} catch(libconfig::SettingTypeException &e) {
			std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
		}
		return 0;
}

decorators::DenominatorsModel::DenominatorsModel(DecoratorManager &pDecoratorManager, DenominatorCard **pCards, int pCardsNumber):
		FiducialDecorator(pDecoratorManager,0),
		mDenomCards(pCards),
		mCardsNumber(pCardsNumber){

}

decorators::DenominatorsModel::~DenominatorsModel(){
	delete []mDenomCards;
}

void decorators::DenominatorsModel::update(){
	for (int i = 0; i < mCardsNumber; i++){
			if (mDenomCards[i]->IsPresent()) mActiveDenomCards.push_back(mDenomCards[i]);
		}
}

decorators::DenominatorCard* decorators::DenominatorsModel::GetActiveCard(){
	if(mActiveDenomCards.size()!=0){
		mActiveDenomCards.clear();
		return mActiveDenomCards[0];
	}
	return 0;
}

bool decorators::DenominatorsModel::isAnyCardActive(){
	return (mActiveDenomCards.size() > 0);
}
