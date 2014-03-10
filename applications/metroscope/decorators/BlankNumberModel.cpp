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
			mActiveCards()
			{

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

	std::vector<decorators::BlankNumberCard *> & decorators::BlankNumberModel::GetGroupOf(BlankNumberCard *pCardMember){
		ClearGroupedCards();
		wykobi::point2d<float> tFirstMemberPoint = pCardMember->GetLocation();
		BlankNumberCard * tAuxNumberCard;
		float tAuxMinDistance = 10000.0f;

		for(unsigned int i = 0; i < mActiveCards.size(); i++ ){
			if(!(mActiveCards[i]->GetMarker() == pCardMember->GetMarker())){
				wykobi::point2d<float> tSecondMemberPoint = mActiveCards[i]->GetLocation();
				float tDistance = wykobi::distance(tFirstMemberPoint.x,tFirstMemberPoint.y,tSecondMemberPoint.x,tSecondMemberPoint.y);

				if(tDistance<tAuxMinDistance){
					tAuxMinDistance = tDistance;
					tAuxNumberCard = mActiveCards[i];
						}
					}
				}

				if(tAuxMinDistance < 90.0f) {
					mGroupedCards.push_back(pCardMember);
					mGroupedCards.push_back(tAuxNumberCard);
				}

		return mGroupedCards;
	}

	bool decorators::BlankNumberModel::AreCardsInsideRectangles(float pXRectangle1, float pYRectangle1, float pXRectangle2, float pYRectangle2){
		int numberOfCorrectCards = 0;

		if((unsigned) mActiveCards.size()!= (unsigned) mNumSummand*3) return false;

		else{
			wykobi::point2d<float> cents1;
			wykobi::point2d<float> cents2;
			wykobi::point2d<float> tens1;
			wykobi::point2d<float> tens2;
			wykobi::point2d<float> units1;
			wykobi::point2d<float> units2;

			cents1.x = pXRectangle1;
			cents1.y = pYRectangle1;

			cents2.x = pXRectangle2;
			cents2.y = pYRectangle2;

			tens1.x = cents1.x + 100.0f;
			tens1.y = cents1.y;

			tens2.x = cents2.x + 100.0f;
			tens2.y = cents2.y;

			units1.x = tens1.x + 100.0f;
			units1.y = tens1.y;

			units2.x = tens2.x + 100.0f;
			units2.y = tens2.y;

			for (unsigned int i = 0; i < mActiveCards.size(); i++){
				wykobi::point2d<float> position = mActiveCards[i]->GetLocation();

				switch(mActiveCards[i]->mType)
						{
							case 0: //units
								 if((units1.x < position.x  && position.x < (units1.x+99.0f)) && (units1.y < position.y  && position.y < (units1.y+133.0f))) numberOfCorrectCards++;
								 else if ((units2.x < position.x  && position.x < (units2.x+99.0f)) && (units2.y < position.y  && position.y < (units2.y+133.0f))) numberOfCorrectCards++;
								 break;
							case 1: //tens
								if((tens1.x < position.x  && position.x < (tens1.x+99.0f)) && (tens1.y < position.y  && position.y < (tens1.y+133.0f))) numberOfCorrectCards++;
								else if ((tens2.x < position.x  && position.x < (tens2.x+99.0f)) && (tens2.y < position.y  && position.y < (tens2.y+133.0f))) numberOfCorrectCards++;
								break;
							case 2: //Cents
								if((cents1.x < position.x  && position.x < (cents1.x+99.0f)) && (cents1.y < position.y  && position.y < (cents1.y+133.0f))) numberOfCorrectCards++;
								else if ((cents2.x < position.x  && position.x < (cents2.x+99.0f)) && (cents2.y < position.y  && position.y < (cents2.y+133.0f))) numberOfCorrectCards++;
								break;
						}
			}
		}

		return (numberOfCorrectCards == mNumSummand*3);
	}


