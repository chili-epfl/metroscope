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

#include "BlankNumberModel.hpp"
#include <iostream>

	const std::string decorators::BlankNumberModel::scDecoratorName("BlankNumberModel");
	const DecoratorManager::Registerer decorators::BlankNumberModel::mRegisterer(decorators::BlankNumberModel::scDecoratorName, &decorators::BlankNumberModel::create);

	decorators::FiducialDecorator *decorators::BlankNumberModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
	{
		try{
			libconfig::Setting & tCardStrings = pSetting["cards"];
			int tSummandNumber = pSetting["summands"];
			BlankNumberCard **tBlankCards = new BlankNumberCard *[tSummandNumber*2];

			for(int i = 0; i < tSummandNumber*3 ; i++){
				tBlankCards[i] = (BlankNumberCard *)pDecoratorManager.loadDecorator(tCardStrings[i]);
			}

			return new decorators::BlankNumberModel(pDecoratorManager, tBlankCards, tSummandNumber);

		}catch(libconfig::SettingNotFoundException &e) {
					std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
				} catch(libconfig::SettingTypeException &e) {
					std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
				}
				return 0;
	}

	decorators::BlankNumberModel::BlankNumberModel(DecoratorManager  &pDecoratorManager, BlankNumberCard **pCards, const int pSummandNumber):
			FiducialDecorator(pDecoratorManager,0),
			mNumSummand(pSummandNumber),
			mBlankCards(pCards),
			mAreCardsStacked(false),
			mActiveCards()
			{
				for(unsigned int i = 0; i < mNumSummand*3; i++){
					switch(mBlankCards[i]->mType){
						case 0: mUnitCards.push_back(mBlankCards[i]); break;
						case 1: mTenCards.push_back(mBlankCards[i]); break;
						case 2: mCentCards.push_back(mBlankCards[i]); break;
					}
				}
			}

	decorators::BlankNumberModel::~BlankNumberModel(){
		delete [] mBlankCards;
	}

	void decorators::BlankNumberModel::update(){

	}

	std::vector<decorators::BlankNumberCard *> & decorators::BlankNumberModel::GetActiveCards() {
		ClearActiveCards();
		for (int i = 0; i < mNumSummand*3; i++){
			if (mBlankCards[i]->IsPresent()) mActiveCards.push_back(mBlankCards[i]);
		}
		return mActiveCards;
	}

	void decorators::BlankNumberModel::ClearActiveCards(){
		mActiveCards.clear();
	}

	void decorators::BlankNumberModel::ClearGroupedCards(){
		mGroupedCards.clear();
	}

	bool decorators::BlankNumberModel::AreCardsSemiStacked(int pType){
		std::vector<decorators::BlankNumberCard *> tCards = GetCardsByType(pType);

		return (abs(tCards[0]->GetLocation().y - tCards[1]->GetLocation().y) < 66.0f && abs(tCards[0]->GetLocation().x - tCards[1]->GetLocation().x) < 10.0f);
		/*
		if(abs(tCards[0]->GetLocation().y-tCards[1]->GetLocation().y)<66.0f && abs(tCards[0]->GetLocation().x-tCards[1]->GetLocation().x)<10.0f){
			return true;
		}else{
			return false;
		}*/

	}

	std::vector<decorators::BlankNumberCard *> & decorators::BlankNumberModel::GetCardsByType(int pType){
		switch (pType)
		{
		case 0: return mUnitCards;
		case 1: return mTenCards;
		case 2: return mCentCards;
		}
	}

	bool decorators::BlankNumberModel::AreCardsInsideRectangles(){
		int numberOfCorrectCards = 0;
		for (unsigned int i = 0; i < mActiveCards.size(); i++){
			wykobi::point2d<float> position = mActiveCards[i]->GetLocation();


			if(scY1 < position.y && position.y < scY2) {
				switch(mActiveCards[i]->mType){
				case 0: //units
					if((scUNIT1_X1 < position.x && position.x < scUNIT1_X2) || (scUNIT2_X1 < position.x && position.x < scUNIT2_X2)){
						numberOfCorrectCards++;
					}
					break;
				case 1: //tens
					if((scTEN1_X1 < position.x && position.x < scTEN1_X2) || (scTEN2_X1 < position.x && position.x < scTEN2_X2)){
						numberOfCorrectCards++;
					}
					break;
				case 2: //Cents
					if((scCENT1_X1 < position.x && position.x < scCENT1_X2) || (scCENT2_X1 < position.x && position.x < scCENT2_X2)){
						numberOfCorrectCards++;
					}
				break;
				}
			}
		}
		return (numberOfCorrectCards == mNumSummand*3);
	}


