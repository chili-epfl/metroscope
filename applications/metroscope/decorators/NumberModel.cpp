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
			int tNumCards = pSetting["num_cards"];
			NumberCard **tNumberCards = new NumberCard *[tNumCards];
			for (int i = 0; i < tNumCards; ++i) {
				tNumberCards[i] = (NumberCard *) pDecoratorManager.loadDecorator(tCardStrings[i]);
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

	void decorators::NumberModel::getCardsInsideRectangle(std::vector<NumberCard *> & pCardArray, wykobi::point2d<float> pOrigin, wykobi::vector2d<float> pTransformXVector, wykobi::vector2d<float> pTransformYVector, float pXMin, float pXMax, float pYMin, float pYMax)
	{
		wykobi::line<float,2> tTransformXAxis = wykobi::make_line(make_ray(pOrigin, pTransformXVector));
		wykobi::line<float,2> tTransformYAxis = wykobi::make_line(make_ray(pOrigin, pTransformYVector));

		for(int i=0; i < mNumCards; i++){
			wykobi::point2d<float> tPoint = mNumberCards[i]->GetLocation();
			if(!(tPoint.x ==0.0f && tPoint.y == 0.0f)){
				mDecoratorManager.GetCam2World().InterpolatedMap(tPoint);
				wykobi::point2d<float> tTransformedPoint;

				float tXOrientation = (wykobi::dot_product(pTransformXVector, wykobi::make_vector(pOrigin) - wykobi::make_vector(tPoint)) > 0) ? -1.0f : 1.0f;
				float tYOrientation = (wykobi::dot_product(pTransformYVector, wykobi::make_vector(pOrigin) - wykobi::make_vector(tPoint)) > 0) ? -1.0f : 1.0f;

				tTransformedPoint.x = tXOrientation*wykobi::distance(wykobi::closest_point_on_line_from_point(tTransformXAxis, tPoint), pOrigin);
				tTransformedPoint.y = tYOrientation*wykobi::distance(wykobi::closest_point_on_line_from_point(tTransformYAxis, tPoint), pOrigin);

				if(valueIsInRange(tTransformedPoint.x, pXMin, pXMax)&& valueIsInRange(tTransformedPoint.y, pYMin, pYMax)) {
					pCardArray.push_back(mNumberCards[i]);
				}
			}
		}
	}

	bool decorators::NumberModel::valueIsInRange(float &pValue, float pRangeStart, float pRangeEnd){
		return pValue > pRangeStart && pValue < pRangeEnd;
	}
