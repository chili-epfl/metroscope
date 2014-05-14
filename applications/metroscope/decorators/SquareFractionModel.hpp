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

#ifndef SquareFractionModel_HPP_
#define SquareFractionModel_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "SquareFractionCard.hpp"
#include <list>
#include <queue>
#include <deque>

namespace decorators{
	class SquareFractionModel : public FiducialDecorator{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		SquareFractionModel (DecoratorManager &pDecoratorManager,FiducialMarker * pMarker, SquareFractionCard ** pCards, int pCardNumber);

		int getTotalCards(){return mNumberActiveCards;}
		int getActiveTokens(int pType){
			return(pType == 0 ? mActiveFirstCard : mActiveSecondCard);
		}

		bool isPresent();

		float getProportion();

	protected:
		void update();
		int mNumberActiveCards;
		int mActiveFirstCard;
		int mActiveSecondCard;
		int mActualDenominator;

		int getDenominator();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		const int mCardNumber;
		SquareFractionCard ** mCards;
		std::vector<SquareFractionCard *> mFirstCards;
		std::vector<SquareFractionCard *> mSecondCards;
		std::vector<SquareFractionCard *> mActiveCards;
	};
}

#endif /* SquareFractionModel_HPP_ */


