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

#include "PolyModel.hpp"
#include <iostream>

const std::string decorators::PolyModel::scDecoratorName("PolyModel");
const  DecoratorManager::Registerer decorators::PolyModel::mRegisterer(decorators::PolyModel::scDecoratorName, &decorators::PolyModel::create);

decorators::FiducialDecorator *decorators::PolyModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
			libconfig::Setting & tCorners = pSetting["corners"];
			libconfig::Setting & tCoords = pSetting["coords"];

			int pNumVertices = pSetting["num_vertices"];

			std::vector<float> polygonVertices;

			for(int i=0; i<pNumVertices; i++){
				polygonVertices.push_back(tCorners[2*i]);
				polygonVertices.push_back(tCorners[(2*i)+1]);
			}

			std::vector<float> polygonCoords;

			for(int i=0; i<pNumVertices; i++){
				polygonCoords.push_back(tCoords[2*i]);
				polygonCoords.push_back(tCoords[(2*i)+1]);
			}

			libconfig::Setting & tOrigin = pSetting["origin"];

			std::vector<float> originCoords;
			originCoords.push_back(tOrigin[0]);
			originCoords.push_back(tOrigin[1]);

			return new decorators::PolyModel(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),pNumVertices, polygonVertices, originCoords, polygonCoords);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
		}
		return 0;
}

decorators::PolyModel::PolyModel(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, int pNumVertices,
		std::vector<float> pPolygon, std::vector<float> pOrigin, std::vector<float> pCoords):
FiducialDecorator(pDecoratorManager, pMarker),
mNumVertices(pNumVertices)
{

	mOrigin.x = pOrigin[0];
	mOrigin.y = pOrigin[1];

	for(int i=0; i<pNumVertices; i++){

		mPolygon.push_back(wykobi::make_point<float>(pPolygon[2*i],pPolygon[(2*i)+1]));
//		mPolygon[i].x = pCoords[2*i];
//		mPolygon[i].y = pCoords[(2*i)+i];
		mPolyCoords.push_back(wykobi::make_point<float>(pCoords[2*i],pCoords[(2*i)+1]));
	}

}
