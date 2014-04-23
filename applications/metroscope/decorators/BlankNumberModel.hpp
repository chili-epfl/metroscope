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

#ifndef BlanKNumberModel_HPP_
#define BlanKNumberModel_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <wykobi/wykobi.hpp>
#include "../ArithmeticsConstants.hpp"
#include "BlankNumberCard.hpp"
#include <list>
#include <queue>
#include <deque>

namespace decorators {

	class BlankNumberModel : public FiducialDecorator
	{
	public:

		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		BlankNumberModel (DecoratorManager &pDecoratorManager, BlankNumberCard **pCards, const int pSummandNumber);
		~BlankNumberModel();

		std::vector<BlankNumberCard *> & GetActiveCards();
		float GetStackXAvg(int pType);
		float GetStackYAvg(int pType);

		void ClearGroupedCards();
		void ClearActiveCards();

		BlankNumberCard * & GetClosestCard(BlankNumberCard *pCard);

		bool AreCardsInsideRectangles();
		bool AreCardsSemiStacked(int pType);
		bool IsAStackNearRectangle();
		bool IsStackNearRectangle(int pType);
		bool AreCardsInsideSolution();
		bool IsStackInsideSolution(int pType);
		std::vector<BlankNumberCard *> & GetCardsByType(int pType);

		void DrawNumbers(float tCentsSum, float tTensSum, float tUnitsSum,
				int tCentsSol, int tTensSol, int tUnitsSol,
				int tOriginalCentsSum, int tOriginalTensSum,
				bool tTensChange, bool tCentsChange);
		bool IsStackInsideBigRectangles(int tType);
		bool CardHasSum(BlankNumberCard * pCard);

	protected:
		void update();

	private:

		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;


		BlankNumberCard **mBlankCards;
		const int mNumSummand;
		bool mAreCardsStacked;

		std::vector<BlankNumberCard *> mActiveBlankCards;


		std::vector<BlankNumberCard *> mGroupedCards;
		std::vector<BlankNumberCard *> mCentCards;
		std::vector<BlankNumberCard *> mTenCards;
		std::vector<BlankNumberCard *> mUnitCards;


	};
}

#endif /* BLANKNUMBERMODEL_HPP_ */
