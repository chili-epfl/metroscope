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

#include "QuadModel.hpp"
#include <iostream>

const std::string decorators::QuadModel::scDecoratorName("QuadModel");
const  DecoratorManager::Registerer decorators::QuadModel::mRegisterer(decorators::QuadModel::scDecoratorName, &decorators::QuadModel::create);

decorators::FiducialDecorator *decorators::QuadModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
			libconfig::Setting & tCorners = pSetting["corners"];
			return new decorators::QuadModel(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),
					tCorners[0], tCorners[1], tCorners[2], tCorners[3], tCorners[4], tCorners[5], tCorners[6], tCorners[7]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
		}
		return 0;
}

decorators::QuadModel::QuadModel(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker,
		float pX1, float pY1, float pX2, float pY2, float pX3, float pY3, float pX4, float pY4):
FiducialDecorator(pDecoratorManager, pMarker)
{
	mQuadix[0].x = pX1;
	mQuadix[0].y = pY1;
	mQuadix[1].x = pX2;
	mQuadix[1].y = pY2;
	mQuadix[2].x = pX3;
	mQuadix[2].y = pY3;
	mQuadix[3].x = pX4;
	mQuadix[3].y = pY4;
}
