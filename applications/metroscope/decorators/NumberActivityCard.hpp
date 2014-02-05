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

#ifndef NumberActivityCard_HPP
#define NumberActivityCard_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "NumberModel.hpp"

namespace decorators {

class NumberActivityCard : public FiducialDecorator
{
	public:
		NumberActivityCard(DecoratorManager &pDecoratorManager,
				FiducialMarker *pMarker,
				NumberModel *pModel,
				const char * const pActivityName);
		virtual ~NumberActivityCard();


	protected:
		void update();

		bool mPreviouslyCurrentActivity; //Tells whether this card was the current activity on the last cycle

		NumberModel *mNumberModel;
		const char *const mActivityName;
		bool mOperatorsPresent;

		void PlaceCenteredTextAtLocationRelativeToMarker(const char *pText, const FiducialMarker *pMarker, float pTextOffsetX, float pTextOffsetY, float pTextScale, float pTextWidth, float pR, float pG, float pB, float pA);
		void showInstruction();


	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

};

}
#endif
