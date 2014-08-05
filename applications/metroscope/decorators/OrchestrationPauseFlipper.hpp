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

#ifndef ORCHESTRATIONPAUSEFLIPPER_HPP_
#define ORCHESTRATIONPAUSEFLIPPER_HPP_


#include <qa/components/misc/NetworkedStateManager.hpp>
#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "../FractionsConstants.hpp"
#include "Flipper.hpp"

	static const long scTogglePauseDelay = 5l*1000l;



namespace decorators {

class OrchestrationPauseFlipper : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		OrchestrationPauseFlipper(DecoratorManager &pDecoratorManager, Flipper * pFlipper);

	protected:
		void update();
		void blackoutScreen();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		Flipper *mFlipper;
		long mLastToggle;

};

}
#endif /* ORCHESTRATIONPAUSE_HPP_ */
