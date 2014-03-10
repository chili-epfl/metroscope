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

#ifndef BLANKNUMBERMODEL_HPP_
#define BLANKNUMBERMODEL_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <wykobi/wykobi.hpp>
#include "BlankNumberCard.hpp"

#include <list>
#include <queue>
#include <deque>

namespace decorators {

	class BlankNumberModel : public FiducialDecorator
	{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		BlankNumberModel (DecoratorManager &pDecoratorManager,BlankNumberCard **pCards, const int pSummandNumber);
		~BlankNumberModel();

		std::vector<BlankNumberCard *> & GetActiveCards();
		void ClearGroupedCards();

		void ClearActiveCards();


		std::vector<BlankNumberCard *> & GetGroupOf(BlankNumberCard *pCardMember);
		BlankNumberCard * & GetClosestCard(BlankNumberCard *pCard);

		bool AreCardsInsideRectangles(float pXRectangle1, float pYRectangle1, float pXRectangle2, float pYRectangle2);

	protected:
		void update();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		const int mNumSummand;
		BlankNumberCard ** mBlankCards;

		std::vector<BlankNumberCard *> mActiveCards;
		std::vector<BlankNumberCard *> mGroupedCards;

	};
}

#endif /* BLANKNUMBERMODEL_HPP_ */