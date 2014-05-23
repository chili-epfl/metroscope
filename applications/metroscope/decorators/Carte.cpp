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

#include "Carte.hpp"
#include <iostream>

const std::string decorators::Carte::scDecoratorName("Carte");
const DecoratorManager::Registerer decorators::Carte::mRegisterer(decorators::Carte::scDecoratorName, &decorators::Carte::create);

decorators::FiducialDecorator *decorators::Carte::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager){
	try{
		libconfig::Setting & tOriginPoint = pSetting ["origin"];
		int tEnd = pSetting ["end_num"];
		libconfig::Setting & tEndPoints = pSetting ["end"];
		int tObstacles = pSetting ["obs_num"];

		std::vector<wykobi::point2d<float>> tEndPoint;
		std::vector<wykobi::point2d<float>> tObstaclesPoint;

		if(tEnd > 0){
			int i = 0;
			while(i < tEnd*2){
				wykobi::point2d<float> tPoint;
				tPoint.x = tEndPoints[i];
				tPoint.y = tEndPoints[i+1];
				tEndPoint.push_back(tPoint);
				i+=2;
			}
		}

		if(tObstacles > 0){
			int j = 0;
			libconfig::Setting & tObstaclesPoints = pSetting ["obstacles"];
			while(j < tObstacles*2){
				wykobi::point2d<float> tObstaclePoint;
				tObstaclePoint.x = tObstaclesPoints[j];
				tObstaclePoint.y = tObstaclesPoints[j+1];
				tObstaclesPoint.push_back(tObstaclePoint);
				j+=2;
			}
		}

		if(tObstacles != 0 && tEnd!= 0)	return new decorators::Carte(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),(int)pSetting ["size"],
					(float)tOriginPoint[0], (float)tOriginPoint[1], tEnd, tEndPoint, tObstacles, tObstaclesPoint);

		else if(tObstacles == 0 && tEnd!= 0)	return new decorators::Carte(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), (int)pSetting ["size"],
					(float)tOriginPoint[0], (float)tOriginPoint[1], tEnd, tEndPoint, tObstacles);

		else if (tObstacles == 0 && tEnd == 0) return new decorators::Carte(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), (int)pSetting ["size"],
				(float)tOriginPoint[0], (float)tOriginPoint[1], tEnd, tObstacles);
	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::Carte::Carte (DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const int pSize,
		float pOriginX, float pOriginY, const int pEndNumber, const std::vector<wykobi::point2d<float>> &pEndPoint,
		const int pObstaclesNumber, const std::vector<wykobi::point2d<float>> &pObstacles):
				FiducialDecorator(pDecoratorManager, pMarker),
				mMarker(pMarker),
				mSize (pSize),
				mEndNumber (pEndNumber),
				mObstaclesNumber (pObstaclesNumber),
				mEndPoint (pEndPoint),
				mObstacles (pObstacles),
				mFinished(false)
	{
		mOriginPoint.x = pOriginX;
		mOriginPoint.y = pOriginY;
	}

decorators::Carte::Carte (DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const int pSize,
			float pOriginX, float pOriginY, const int pEndNumber, const std::vector<wykobi::point2d<float>> &pEndPoint,
			const int pObstaclesNumber):
				FiducialDecorator(pDecoratorManager, pMarker),
				mMarker(pMarker),
				mSize (pSize),
				mEndNumber (pEndNumber),
				mObstaclesNumber (pObstaclesNumber),
				mEndPoint (pEndPoint),
				mFinished(false)
	{
			mOriginPoint.x = pOriginX;
			mOriginPoint.y = pOriginY;
	}

decorators::Carte::Carte (DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const int pSize,
			float pOriginX, float pOriginY, const int pEndNumber,const int pObstaclesNumber):
				FiducialDecorator(pDecoratorManager, pMarker),
				mMarker(pMarker),
				mSize (pSize),
				mEndNumber (pEndNumber),
				mObstaclesNumber (pObstaclesNumber),
				mFinished(false)
	{
			mOriginPoint.x = pOriginX;
			mOriginPoint.y = pOriginY;
	}
decorators::Carte::~Carte(){}
void decorators::Carte::update(){

}

bool decorators::Carte::IsEmptyCell(int pPositionX, int pPositionY){
	if (mObstaclesNumber > 0){
		for(std::vector<wykobi::point2d<float>>::iterator it = mObstacles.begin(); it != mObstacles.end() ; ++it){
			if((*it).x == (float)pPositionX && (*it).y == (float)pPositionY) return false;
		}
		return true;
	}
	return true;
}
