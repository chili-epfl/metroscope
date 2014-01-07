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

#ifndef VariableFractionController_HPP
#define VariableFractionController_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "NumberDial.hpp"

namespace decorators {

class VariableFractionController : public NumberDial
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		VariableFractionController(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, VariableFractionCard *pVariableFractionCard, FiducialMarker *pStabilizedGraphicsMarker, bool pIsNumerator);

	protected:
		void update();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		enum Type {Numerator, Denominator};
		Type mType;

		VariableFractionCard *mVariableFractionCard;
};

}
#endif
