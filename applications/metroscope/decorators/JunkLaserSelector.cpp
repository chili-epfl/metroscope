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

#include "JunkLaserSelector.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>
#include <qa/utils/Geometry.hpp>

const std::string decorators::JunkLaserSelector::scDecoratorName("JunkLaserSelector");
const  DecoratorManager::Registerer decorators::JunkLaserSelector::mRegisterer(decorators::JunkLaserSelector::scDecoratorName, &decorators::JunkLaserSelector::create);

decorators::FiducialDecorator *decorators::JunkLaserSelector::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		JunkLaserSelector *tJunkLaserSelector = new JunkLaserSelector(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]));
		return tJunkLaserSelector;
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::JunkLaserSelector::JunkLaserSelector(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker):
FiducialDecorator(pDecoratorManager, pMarker),
mSelection(0)
{
}

decorators::JunkLaserSelector::~JunkLaserSelector()
{
}

void decorators::JunkLaserSelector::update()
{
	if (mMarker->isPresent())
	{
		int tOrientation = (int) (mMarker->getAngle() + 0.5f);
		tOrientation += 210;
		mSelection = ((tOrientation + 360) % 360)/(360/3);
	}
}
