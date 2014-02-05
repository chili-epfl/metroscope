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

#include "SumClassificationModel.hpp"
#include <iostream>

const std::string decorators::SumClassificationModel::scDecoratorName("SumClassificationModel");
const  DecoratorManager::Registerer decorators::SumClassificationModel::mRegisterer(decorators::SumClassificationModel::scDecoratorName, &decorators::SumClassificationModel::create);


	decorators::FiducialDecorator *decorators::SumClassificationModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
	{
		try {
			const char *pActivityName;

			return new decorators::SumClassificationModel(pDecoratorManager,
					pDecoratorManager.loadMarker(pSetting["marker"]),
					(NumberModel *)pDecoratorManager.loadDecorator(pSetting["number_model"]),
					pActivityName);
		} catch(libconfig::SettingNotFoundException &e) {
			std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
		} catch(libconfig::SettingTypeException &e) {
			std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
		}
		return 0;
	}

	decorators::SumClassificationModel::SumClassificationModel(DecoratorManager &pDecoratorManager,FiducialMarker *pMarker,NumberModel *pModel,const char* pActivityName):
	NumberActivityCard(pDecoratorManager,pMarker,pModel,pActivityName)
	{

	}

	decorators::SumClassificationModel::~SumClassificationModel(){
		/*Empty*/
	}

