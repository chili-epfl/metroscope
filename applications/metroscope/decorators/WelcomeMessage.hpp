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

#ifndef WELCOMEMESSAGE_HPP_
#define WELCOMEMESSAGE_HPP_


#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "Executor.hpp"
#include "../FractionsConstants.hpp"


namespace decorators {

class WelcomeMessage : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		WelcomeMessage(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, std::string message, std::string imgFile, int numActiveMarkers, Executor ** activeMarkers);

	protected:
		void update();
		void displayMessage();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		const std::string sMessage;
		const std::string sImgFile;
		const int mNumActiveMarkers;
		Executor ** mActiveMarkers;
};

}
#endif /* SIMPLETEXT_HPP_ */
