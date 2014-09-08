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

/*
 * Map with obstacle(s) and ending point(s)
 */
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

/*
 * Map without obstacles
 */
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

/*
 * Map without obstacle(s) and ending point(s)
 */
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

/*
 * Check if it has an obstacle
 * @param X and Y position of the cell to be checked
 * @return true if there's no obstacle, false if there is
 */
bool decorators::Carte::IsEmptyCell(int pPositionX, int pPositionY){
	if (mObstaclesNumber > 0){
		for(std::vector<wykobi::point2d<float>>::iterator it = mObstacles.begin(); it != mObstacles.end() ; ++it){
			if((*it).x == (float)pPositionX && (*it).y == (float)pPositionY) return false;
		}
		return true;
	}
	return true;
}

/*
 * Check if the cell is an ending cell
 * @param X and Y position of the cell to be checked
 * @return true if is an ending point, false if not.
 */
bool decorators::Carte::IsEndCell(int pPositionX, int pPositionY){
	if (mObstaclesNumber > 0){
		for(std::vector<wykobi::point2d<float>>::iterator it = mEndPoint.begin(); it != mEndPoint.end() ; ++it){
			if((*it).x == (float)pPositionX && (*it).y == (float)pPositionY) return true;
		}
		return false;
	}
	return false;
}

/*
 * Check if the cell is inside the map (to check borders)
 * @param X and Y position of the cell to be checked
 * @return true if is inside the borders of the map
 */
bool decorators::Carte::IsInsideMap(int pPointX, int pPointY){
	return ((pPointX >= 0 && pPointX < mSize)&&(pPointY >= 0 && pPointY < mSize));
}

/*
 * Calculates the minimum distance of a vertex with their neighbors
 * @param X and Y position of the cell to be checked
 * @return the minimum distance
 */
int decorators::Carte::GetMinDistanceNeighbors(int pX,int pY){
	// Each cell has 8 neighbors (4 diagonals + 4 sides)
	int tMinDistance = 10000; //We start with the minimum distance = 10000

	// Check in all directions
	for(int i = -1 ; i < 2 ; i++){
		for (int j = -1 ; j < 2 ; j++){
			wykobi::point2d<int> tPoint = wykobi::make_point(pX + j,pY+i);
			if(!(tPoint == wykobi::make_point(pX,pY))){
				if(IsInsideMap(tPoint.x,tPoint.y) && mStepMap[tPoint.y][tPoint.x] < tMinDistance // If a minimum has been found
						&& std::find(mVisitedVertexes.begin(), mVisitedVertexes.end(), tPoint)!= mVisitedVertexes.end()){ // And the vertexes is already visited
					tMinDistance = mStepMap[tPoint.y][tPoint.x] + 1; // Add up 1 (weight of the edge)
				}
			}
		}
	}

	return tMinDistance;
}

/*
 * Creates the map that stores the distance of each cell to the closest ending point (based on Dijkstra algorithm, with modifications)
 */
void decorators::Carte::CreateStepMap(){
	for(std::vector<wykobi::point2d<float>>::iterator it = mObstacles.begin() ; it != mObstacles.end() ; ++it){ // For each obstacle
		int tY = (*it).y;
		int tX = (*it).x;
		mStepMap[tY][tX] = 10000; // Set the distance to the end as 10000
		mVisitedVertexes.push_back(wykobi::make_point(tX,tY)); // And add it to the visited vertexes (to avoid checking later)
	}
	for(std::vector<wykobi::point2d<float>>::iterator it = mEndPoint.begin() ; it != mEndPoint.end() ; ++it){ // For each ending point
		int tY = (*it).y;
		int tX = (*it).x;
		mStepMap[tY][tX] = 0;	// Set the distance to the end as 0
		mVisitedVertexes.push_back(wykobi::make_point(tX,tY));	//Add it to the visited vertexes (to avoid checking again later)
		AddNeighbors(tX,tY);	// Add the neighbors to the vertexes-to-visit list
	}

		for(std::vector<wykobi::point2d<float>>::iterator it = mEndPoint.begin(); it != mEndPoint.end() ; ++it){ // For each ending point
			while(mNotVisitedVertexes.size() > 0){ // While there's vertexes to visit

				std::vector<wykobi::point2d<int>>::iterator it = mNotVisitedVertexes.begin(); // Take the first in the list

				int tX = (*it).x; // X component
				int tY = (*it).y; // Y component

				mStepMap[(*it).y][(*it).x] = GetMinDistanceNeighbors(tX, tY); // Calculate the minimum distance with neighbors

				mVisitedVertexes.push_back(wykobi::make_point(tX,tY)); // Set it as visited vertex
				AddNeighbors(tX,tY); // Add the neighbors to the vertexes-to-visit list

				mNotVisitedVertexes.erase(mNotVisitedVertexes.begin());	// Delete the vertex from the vertexes-to-visit list...
		}
	}
}

/*
 * Adds the neighbors of a given cell (up, left, right, down, upper-left,
 * upper-right, bottom-left, bottom-right)
 * @param X and Y position of the cell
 */
void decorators::Carte::AddNeighbors(int pX,int pY){
	// Check from 1 cell before and after (X and Y position)
	for(int i = -1 ; i < 2 ; i++){
		for (int j = -1 ; j < 2 ; j++){
			wykobi::point2d<int> tPoint = wykobi::make_point(pX + j,pY+i);
			if(!(tPoint == wykobi::make_point(pX,pY))){

				if(std::find(mVisitedVertexes.begin(), mVisitedVertexes.end(), tPoint) == mVisitedVertexes.end() // If the neighbor hasn't been visited
						&& IsInsideMap(tPoint.x,tPoint.y)	// If the neighbor is inside the map (i.e. not checking outside the boundaries
						&& std::find(mNotVisitedVertexes.begin(), mNotVisitedVertexes.end(), tPoint) == mNotVisitedVertexes.end()){ // Is not already in the vertexes to visit
					mNotVisitedVertexes.push_back(tPoint);
				}
			}
		}
	}
}
