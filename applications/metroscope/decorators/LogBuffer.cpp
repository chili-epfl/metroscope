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

#include "LogBuffer.hpp"
#include <iostream>

const std::string decorators::LogBuffer::scDecoratorName("LogBuffer");
const  DecoratorManager::Registerer decorators::LogBuffer::mRegisterer(decorators::LogBuffer::scDecoratorName, &decorators::LogBuffer::create);

decorators::FiducialDecorator *decorators::LogBuffer::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		std::map<const std::string, FiducialMarker * > tTags;
		tTags["sheet"] = pDecoratorManager.loadMarker(pSetting["sheet"]);
		tTags["vcard"] = pDecoratorManager.loadMarker(pSetting["vcard"]);
		tTags["tcard1"] = pDecoratorManager.loadMarker(pSetting["tcard1"]);
		tTags["tcard2"] = pDecoratorManager.loadMarker(pSetting["tcard2"]);
		tTags["tcard3"] = pDecoratorManager.loadMarker(pSetting["tcard3"]);
		tTags["shape1"] = pDecoratorManager.loadMarker(pSetting["shape1"]);
		tTags["shape2"] = pDecoratorManager.loadMarker(pSetting["shape2"]);
		tTags["shape3"] = pDecoratorManager.loadMarker(pSetting["shape3"]);
		tTags["shape4"] = pDecoratorManager.loadMarker(pSetting["shape4"]);
		tTags["shape5"] = pDecoratorManager.loadMarker(pSetting["shape5"]);
		int tLogSize = pSetting["logsize"];
		return new decorators::LogBuffer(pDecoratorManager, tLogSize, tTags);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::LogBuffer::LogBuffer(DecoratorManager &pDecoratorManager, std::size_t pLogSize, const std::map<const std::string, FiducialMarker * > & pTags):
FiducialDecorator(pDecoratorManager, 0),
mTags(pTags)
{
	for (auto it = mTags.begin(); it != mTags.end(); ++it)
		for (std::size_t i=0; i<pLogSize; ++i)
			mLog[(*it).first].push_back(LogEntry(false));
}

void decorators::LogBuffer::update()
{
	for (auto it = mTags.begin(); it != mTags.end(); ++it)
	{
		auto tName = (*it).first;
		auto tTag = (*it).second;
		std::deque<LogEntry> & tLog = mLog[tName];
		if (tTag->isPresent())
			tLog.push_back(LogEntry(true, tTag->getCenter().x, tTag->getCenter().y));
		else tLog.push_back(LogEntry(false));
		tLog.pop_front();
	}
}

