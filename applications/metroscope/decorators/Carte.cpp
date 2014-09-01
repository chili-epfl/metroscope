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
		int tObstacles = pSetting ["obs_num"];

		std::vector<wykobi::point2d<float>> tEndPoint;
		std::vector<wykobi::point2d<float>> tObstaclesPoint;

		if(tEnd > 0){
			int i = 0;
			libconfig::Setting & tEndPoints = pSetting ["end"];
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

		//Obstacles and end
		if(tObstacles != 0 && tEnd!= 0)	return new decorators::Carte(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),(int)pSetting ["size"],
					(float)tOriginPoint[0], (float)tOriginPoint[1], tEnd, tEndPoint, tObstacles, tObstaclesPoint);

		//No obstacles but with end
		else if(tObstacles == 0 && tEnd!= 0)	return new decorators::Carte(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), (int)pSetting ["size"],
					(float)tOriginPoint[0], (float)tOriginPoint[1], tEnd, tEndPoint, tObstacles);

		//No obstacles and no end
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

		CreateStepMap();

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

			CreateStepMap();
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

			CreateStepMap();

	}
decorators::Carte::~Carte(){ }

void decorators::Carte::update(){ }

bool decorators::Carte::IsEmptyCell(int pPositionX, int pPositionY){
	if (mObstaclesNumber > 0){
		for(std::vector<wykobi::point2d<float>>::iterator it = mObstacles.begin(); it != mObstacles.end() ; ++it){
			if((*it).x == (float)pPositionX && (*it).y == (float)pPositionY) return false;
		}
		return true;
	}
	return true;
}

bool decorators::Carte::IsEndCell(int pPositionX, int pPositionY){
	if (mObstaclesNumber > 0){
		for(std::vector<wykobi::point2d<float>>::iterator it = mEndPoint.begin(); it != mEndPoint.end() ; ++it){
			if((*it).x == (float)pPositionX && (*it).y == (float)pPositionY) return true;
		}
		return false;
	}
	return false;
}

bool decorators::Carte::IsInsideMap(int pPointX, int pPointY){
	return ((pPointX >= 0 && pPointX < mSize)&&(pPointY >= 0 && pPointY < mSize));
}

int decorators::Carte::GetMinDistanceNeighbours(int pX,int pY){
	// Each cell has 8 neighbors (4 diagonals + 4 sides)
	int tMinDistance = 10000; //We start with the minimum distance = 10000

	for(int i = -1 ; i < 2 ; i++){
		for (int j = -1 ; j < 2 ; j++){
			wykobi::point2d<int> tPoint = wykobi::make_point(pX + j,pY+i);
			if(!(tPoint == wykobi::make_point(pX,pY))){
				if(IsInsideMap(tPoint.x,tPoint.y) && mStepMap[tPoint.y][tPoint.x] < tMinDistance
						&& std::find(mVisitedVertexes.begin(), mVisitedVertexes.end(), tPoint)!= mVisitedVertexes.end()){
					tMinDistance = mStepMap[tPoint.y][tPoint.x] + 1;
				}
			}
		}
	}

	return tMinDistance;
}

/*
 * Creates the...
 */
void decorators::Carte::CreateStepMap(){
	for(std::vector<wykobi::point2d<float>>::iterator it = mObstacles.begin() ; it != mObstacles.end() ; ++it){
		int tY = (*it).y;
		int tX = (*it).x;
		mStepMap[tY][tX] = 10000;
		mVisitedVertexes.push_back(wykobi::make_point(tX,tY));
	}
	for(std::vector<wykobi::point2d<float>>::iterator it = mEndPoint.begin() ; it != mEndPoint.end() ; ++it){
		int tY = (*it).y;
		int tX = (*it).x;
		mStepMap[tY][tX] = 0;
		mVisitedVertexes.push_back(wykobi::make_point(tX,tY));
		AddNeighbours(tX,tY);
	}

		for(std::vector<wykobi::point2d<float>>::iterator it = mEndPoint.begin(); it != mEndPoint.end() ; ++it){

			while(mNotVisitedVertexes.size() > 0){
				std::vector<wykobi::point2d<int>>::iterator it = mNotVisitedVertexes.begin();

				int tX = (*it).x;
				int tY = (*it).y;

				mStepMap[(*it).y][(*it).x] = GetMinDistanceNeighbours(tX, tY);

				mVisitedVertexes.push_back(wykobi::make_point(tX,tY));
				AddNeighbours(tX,tY);

				mNotVisitedVertexes.erase(mNotVisitedVertexes.begin());
		}
	}
}

void decorators::Carte::AddNeighbours(int pX,int pY){

	for(int i = -1 ; i < 2 ; i++){
		for (int j = -1 ; j < 2 ; j++){
			wykobi::point2d<int> tPoint = wykobi::make_point(pX + j,pY+i);
			if(!(tPoint == wykobi::make_point(pX,pY))){
				if(std::find(mVisitedVertexes.begin(), mVisitedVertexes.end(), tPoint) == mVisitedVertexes.end() && IsInsideMap(tPoint.x,tPoint.y)
						&& std::find(mNotVisitedVertexes.begin(), mNotVisitedVertexes.end(), tPoint) == mNotVisitedVertexes.end()){
					mNotVisitedVertexes.push_back(tPoint); // Up
				}
			}
		}
	}
}

bool decorators::Carte::AreVertexesNotVisited(int pRadius,std::vector<wykobi::point2d<float>>::iterator pIterator){
	int tCount = 0;

	for(int j = 0 ; j < 2*pRadius + 1 ; j++){ // Horizontal side of the square
		int tX = (*pIterator).x - pRadius + j;
		int tYUpper = (*pIterator).y - pRadius;
		int tYBottom = (*pIterator).y + pRadius;
		if(mVisitedVertex[tYUpper][tX] && IsInsideMap(tX,tYUpper))	tCount++;
		if(mVisitedVertex[tYBottom][tX] && IsInsideMap(tX,tYBottom))	tCount++;

		if(j >0 && j < 2*pRadius){ // Vertical side of the square
			int tXRight = (*pIterator).x + pRadius;
			int tXLeft = (*pIterator).x - pRadius;
			int tY = (*pIterator).y - pRadius + j;
			if(mVisitedVertex[tY][tXRight] && IsInsideMap(tXRight,tY))	tCount++;
			if(mVisitedVertex[tY][tXLeft] && IsInsideMap(tXLeft,tY))	tCount++;
		}
	}

	return !(tCount == GetNumberCellInsideMap(pRadius, (*pIterator).x,(*pIterator).y));
}

int decorators::Carte::GetNumberCellInsideMap(int pRadius, int pX, int pY){
	int tCount = 0;

	for(int j = 0 ; j < 2*pRadius + 1 ; j++){ // Horizontal side of the square
		int tX = pX - pRadius + j;
		int tYUpper = pY - pRadius;
		int tYBottom = pY + pRadius;
		if(IsInsideMap(tX,tYUpper))	tCount++;
		if(IsInsideMap(tX,tYBottom))	tCount++;

		if(j >0 && j < 2*pRadius){ // Vertical side of the square
			int tXRight = pX + pRadius;
			int tXLeft = pX - pRadius;
			int tY = pY - pRadius + j;
			if(IsInsideMap(tXRight,tY))	tCount++;
			if(IsInsideMap(tXLeft,tY))	tCount++;
		}
	}

	return tCount;
}

