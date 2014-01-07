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

#ifndef VariableFractionCard_HPP
#define VariableFractionCard_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "FractionCard.hpp"

namespace decorators {

class VariableFractionCard : public FractionCard
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		VariableFractionCard(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const int pDefaultNumerator, const int pDefaultDenominator, const std::string pFractionStr);
		~VariableFractionCard(){}

		void SetNumerator(int pNumerator){ mNumerator = pNumerator; }
		void SetDenominator(int pDenominator){ mDenominator = pDenominator; }
		void DisplayFraction(const char *pNumStr, const char *pDenomStr, float pXOffset, float pYOffset, bool pDrawCenterLine, const color *pFractionColor);

	protected:
		void update();


	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;




};

}
#endif
