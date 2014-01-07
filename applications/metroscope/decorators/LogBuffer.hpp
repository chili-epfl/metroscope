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

#ifndef LogBuffer_HPP
#define LogBuffer_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>

namespace decorators {

class LogBuffer : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		LogBuffer(DecoratorManager &pDecoratorManager, std::size_t pLogSize, const std::map<const std::string, FiducialMarker * > & pTags);

	class LogEntry {
		public:
		LogEntry(bool pPresent, float pX = 0.0f, float pY = 0.0f):
			present(pPresent), x(pX), y(pY){}
		bool present;
		float x,y;
	};

	const std::map<const std::string, std::deque<LogEntry> > & GetLog() const {return mLog;}
	
	protected:
		void update();
		std::map<const std::string, FiducialMarker * > mTags;
		std::map<const std::string, std::deque<LogEntry> > mLog;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
};

}
#endif
