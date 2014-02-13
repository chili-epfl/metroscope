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

#include "NumberModel.hpp"
#include <iostream>

	const std::string decorators::NumberModel::scDecoratorName("NumberModel");
	const  DecoratorManager::Registerer decorators::NumberModel::mRegisterer(decorators::NumberModel::scDecoratorName, &decorators::NumberModel::create);

	decorators::FiducialDecorator *decorators::NumberModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
	{
		try {
			libconfig::Setting & tCardStrings = pSetting["cards"];
			std::string i = tCardStrings[0];
			int tNumCards = pSetting["num_cards"];
			NumberCard **tNumberCards = new NumberCard *[tNumCards];
			for (int i = 0; i < tNumCards; ++i) {
				tNumberCards[i] = (NumberCard *)pDecoratorManager.loadDecorator(tCardStrings[i]);
			}

			return new decorators::NumberModel(pDecoratorManager, tNumberCards, tNumCards);

		} catch(libconfig::SettingNotFoundException &e) {
			std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
		} catch(libconfig::SettingTypeException &e) {
			std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
		}
		return 0;

	}

	decorators::NumberModel::NumberModel(DecoratorManager &pDecoratorManager, NumberCard **pCards, const int pNumCards):
	FiducialDecorator(pDecoratorManager, 0),
	mNumCards(pNumCards),
	mNumberCards(pCards),
	mActiveCards()
	{
	}

	decorators::NumberModel::~NumberModel(){
	delete [] mNumberCards;
	}

	void decorators::NumberModel::update(){

	}

	void decorators::NumberModel::SetCurrentActivity (const char *pActivityName){
		std::vector<const char *>::iterator it = std::find(mPresentActivities.begin(), mPresentActivities.end(), pActivityName);
		if(it == mPresentActivities.end()) mPresentActivities.push_back(pActivityName);
	}

	std::vector<decorators::NumberCard *> & decorators::NumberModel::GetActiveCards() {
		ClearActiveCards();
		for(int i=0; i < mNumCards; i++){
			if(mNumberCards[i]->IsPresent()) mActiveCards.push_back(mNumberCards[i]);
		}
		return mActiveCards;
	}

	const char *decorators::NumberModel::GetCurrentActivity () const {
		if(!mPresentActivities.empty())return mPresentActivities.front();
		return "";
	}

	void decorators::NumberModel::RemoveActivity(const char *pActivityName){
		std::vector<const char *>::iterator it = std::find(mPresentActivities.begin(), mPresentActivities.end(), pActivityName);
		if(it != mPresentActivities.end()) mPresentActivities.erase(it);
	}

	std::vector<const char*> & decorators::NumberModel::GetPresentActivities() {
		return mPresentActivities;
	}

	void decorators::NumberModel::ClearActiveCards(){
		mActiveCards.clear();
	}

	void decorators::NumberModel::ClearGroupedCards(){
		mGroupedCards.clear();
	}

	std::vector<decorators::NumberCard *> & decorators::NumberModel::GetGroupOf(NumberCard *pNumberMember){
		ClearGroupedCards();
		wykobi::point2d<float> tFirstMemberPoint = pNumberMember->GetLocation();
		NumberCard * tAuxNumberCard;
		float tAuxMinDistance = 10000.0f;

		for(unsigned int i = 0; i < mActiveCards.size(); i++ ){

			if(!(mActiveCards[i]->GetMarker()== pNumberMember->GetMarker())){
				wykobi::point2d<float> tSecondMemberPoint = mActiveCards[i]->GetLocation();
				float tDistance = wykobi::distance(tFirstMemberPoint.x,tFirstMemberPoint.y,tSecondMemberPoint.x,tSecondMemberPoint.y);

				if(tDistance<tAuxMinDistance){
					tAuxMinDistance = tDistance;
					tAuxNumberCard = mActiveCards[i];
				}
			}
		}

		if(tAuxMinDistance < 90.0f) {
							mGroupedCards.push_back(pNumberMember);
							mGroupedCards.push_back(tAuxNumberCard);
						}

		return mGroupedCards;
	}


	bool decorators::NumberModel::valueIsInRange(float &pValue, float pRangeStart, float pRangeEnd){
		return (pRangeEnd-pRangeStart)>abs(pValue);
	}
