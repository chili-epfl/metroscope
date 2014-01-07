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

#include "AxisModel.hpp"
#include <iostream>

const std::string decorators::AxisModel::scDecoratorName("AxisModel");
const  DecoratorManager::Registerer decorators::AxisModel::mRegisterer(decorators::AxisModel::scDecoratorName, &decorators::AxisModel::create);

decorators::FiducialDecorator *decorators::AxisModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
			libconfig::Setting & tPoints = pSetting["points"];
			return new decorators::AxisModel(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),
					tPoints[0], tPoints[1], tPoints[2], tPoints[3]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
		}
		return 0;
}
