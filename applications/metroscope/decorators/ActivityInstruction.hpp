/*******************************************************************************
*   Copyright 2014 EPFL                                                        *
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

#ifndef ACTIVITYINSTRUCTION_HPP_
#define ACTIVITYINSTRUCTION_HPP_



#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "../FractionsConstants.hpp"

static const color * const scMESSAGE_COLOR = &scBLACK;
static const float scINSTRUCTION_WIDTH = 1000.0f;
static const float scINSTRUCTION_SCALE = 1.0f;


namespace decorators {

class ActivityInstruction : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		ActivityInstruction(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, std::string pMessage, FiducialMarker *pPosition);

	protected:
		void update();
		void displayMessage();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		const std::string mMessage;
		const FiducialMarker *mMessagePositionMarker;
};

}
#endif /* ActivityInstruction_HPP_ */
