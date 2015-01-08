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

#ifndef EXECUTOR_HPP_
#define EXECUTOR_HPP_



#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "../FractionsConstants.hpp"
#include "SimpleText.hpp"
//static const color * const scMESSAGE_COLOR = &scGREEN;


namespace decorators {

class Executor : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		Executor(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pMarkerMessages, std::string pCommand, std::string pMessage, int pCountdown, int pExitcode);

	protected:
		void update();
		void displayMessage();
		void displayCountdown();
		void executeCommand();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		const std::string mCommand;
		const std::string mMessage;
		const int mCountdown = 0;
		FiducialMarker *mMarkerMessages;
		long mCountdownStart;
		const int mExitcode = 0;

};

}
#endif /* EXECUTOR_HPP_ */
