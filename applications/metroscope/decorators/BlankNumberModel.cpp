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
			BlankNumberCard **tBlankCards = new BlankNumberCard *[tSummandNumber*3];

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
			mBlankCards(pCards),
			mNumSummand(pSummandNumber),
			mAreCardsStacked(false),
			mActiveBlankCards()
			{
				for(int i = 0; i < mNumSummand*3; i++){
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
		if(mActiveBlankCards.size()!= 0) ClearActiveCards();

		for (int i = 0; i < mNumSummand*3; i++){
			if (mBlankCards[i]->IsPresent()) mActiveBlankCards.push_back(mBlankCards[i]);
		}

		return mActiveBlankCards;
	}



	void decorators::BlankNumberModel::ClearActiveCards(){
		mActiveBlankCards.clear();
	}

	void decorators::BlankNumberModel::ClearGroupedCards(){
		mGroupedCards.clear();
	}

	bool decorators::BlankNumberModel::AreCardsSemiStacked(int pType){
		std::vector<decorators::BlankNumberCard *> tCards = GetCardsByType(pType);

		if(tCards[0]->IsPresent() && tCards[1]->IsPresent()){
			float tDistance = wykobi::distance(tCards[0]->GetLocation().x,tCards[0]->GetLocation().y,tCards[1]->GetLocation().x,tCards[1]->GetLocation().y);
			return (tDistance < 50.0f);
		}

		return 0;

	}

	std::vector<decorators::BlankNumberCard *> & decorators::BlankNumberModel::GetCardsByType(int pType){
		switch (pType)
		{
		case 0: return mUnitCards;
		case 1: return mTenCards;
		}
		return mCentCards;
	}


	bool decorators::BlankNumberModel::AreCardsInsideRectangles(){
		int numberOfCorrectCards = 0;
		wykobi::point2d<float> position;
		for (unsigned int i = 0; i < mActiveBlankCards.size(); i++){

			position = mActiveBlankCards[i]->GetLocation();

			if(scY1 < position.y && position.y < scY2) {
				switch(mActiveBlankCards[i]->mType){
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

	bool decorators::BlankNumberModel::IsAStackNearRectangle(){
		return (IsStackNearRectangle(0) || IsStackNearRectangle(1) || IsStackNearRectangle(2));
	}

	float decorators::BlankNumberModel::GetStackXAvg(int pType){
		if(AreCardsSemiStacked(pType)){
			std::vector<decorators::BlankNumberCard *> tCards = GetCardsByType(pType);
			return (tCards[0]->GetLocation().x +tCards[1]->GetLocation().x)/2.0;
		}
		return -1.0;
	}

	float decorators::BlankNumberModel::GetStackYAvg(int pType){
			if(AreCardsSemiStacked(pType)){
				std::vector<decorators::BlankNumberCard *> tCards = GetCardsByType(pType);
				return (tCards[0]->GetLocation().y +tCards[1]->GetLocation().y)/2.0;
			}
			return 0.0;
		}

	bool decorators::BlankNumberModel::IsStackNearRectangle(int pType){
		float tRectangleX = 0.0f;
		float tRectangleY = scY2;

		switch(pType){
		case 0: //units
			tRectangleX = scUNIT_SOLUTION_X_AVG;
			break;
		case 1:
			tRectangleX = scTEN_SOLUTION_X_AVG;
			break;
		case 2:
			tRectangleX = scCENT_SOLUTION_X_AVG;
			break;
		}

		if(AreCardsSemiStacked(pType)){
			float tDistance = wykobi::distance(GetStackXAvg(pType),GetStackYAvg(pType),tRectangleX,tRectangleY);
			return (tDistance < 50.0f);
		}
		return 0;
	}

	bool decorators::BlankNumberModel::AreCardsInsideSolution(){
		return (IsStackInsideSolution(0) || IsStackInsideSolution(1) || IsStackInsideSolution(2));
	}

	bool decorators::BlankNumberModel::IsStackInsideSolution(int pType){
				wykobi::point2d<float> position;
				for (unsigned int i = 0; i < mActiveBlankCards.size(); i++){

					position = mActiveBlankCards[i]->GetLocation();

					if(scY1 < position.y && position.y < scY2) {
						switch(pType){
						case 0: //units
							if(scUNIT_SOLUTION_X1 < position.x && position.x < scUNIT_SOLUTION_X2) return true; break;
						case 1: //tens
							if(scTEN_SOLUTION_X1 < position.x && position.x < scTEN_SOLUTION_X2) return true; break;
						case 2: //Cents
							if(scCENT_SOLUTION_X1 < position.x && position.x < scCENT_SOLUTION_X2) return true; break;
						}
					}
				}
				return false;
	}



