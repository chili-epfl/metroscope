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

#ifndef NumberModel_HPP
#define NumberModel_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <wykobi/wykobi.hpp>
#include "NumberCard.hpp"
#include <list>
#include <queue>
#include <deque>

namespace decorators {

class NumberModel : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		NumberModel(DecoratorManager &pDecoratorManager,
				NumberCard **pCards,
				const int pNumCards);
		~NumberModel();

		NumberCard **GetCardArray() const {return mNumberCards;}
		int GetNumCards() const {return mNumCards;}
		const char *GetCurrentActivity () const;
		std::vector<NumberCard *> & GetActiveCards();
		std::vector<const char*> & GetPresentActivities();

		void ClearActiveCards();
		void ClearGroupedCards();

		std::vector<NumberCard *> & GetGroupOf(NumberCard *pNumberMember);
		NumberCard* & GetClosestCard(NumberCard *pNumberCard);

		void SetCurrentActivity (const char *pActivityName);
		void RemoveActivity(const char *pActivityName);

	protected:
		void update();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		const int mNumCards;
		NumberCard **mNumberCards; //array of pointers to all of the NumberCard objects

		std::vector<const char *> mPresentActivities; //front of this vector is the current activity
		std::vector<NumberCard *> mActiveCards;
		std::vector<NumberCard *> mGroupedCards;

		bool valueIsInRange(float &pValue, float pRangeStart, float pRangeEnd);

};

}
#endif
