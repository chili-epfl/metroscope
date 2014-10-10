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

#ifndef CARTE_HPP_
#define CARTE_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <fstream>

namespace decorators {
class Carte : public FiducialDecorator
{
	public:
		static FiducialDecorator *create (libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		Carte (DecoratorManager &pDecoratorManager, FiducialMarker *pMarker,
				const int pSize, float pOriginX, float pOriginY,
				const int pEndNumber, const std::vector<wykobi::point2d<float>> &pEndPoint,
				const int pObstaclesNumber, const std::vector<wykobi::point2d<float>> &pObstacles,
				const int pId);

		Carte (DecoratorManager &pDecoratorManager, FiducialMarker *pMarker,
						const int pSize, float pOriginX, float pOriginY,
						const int pEndNumber, const std::vector<wykobi::point2d<float>> &pEndPoint,
						const int pObstaclesNumber, const int pId);

		Carte (DecoratorManager &pDecoratorManager, FiducialMarker *pMarker,
				const int pSize, float pOriginX, float pOriginY, const int pEndNumber,
				const int pObstaclesNumber, const int pId);

		~Carte();

		int getSize() {return mSize;}
		int getEndNumber() {return mEndNumber;}
		int getObstacleNumber() {return mObstaclesNumber;}
		std::vector<wykobi::point2d<float>> & getEndPoint(){return mEndPoint;}
		std::vector<wykobi::point2d<float>> & getObstaclesPoint() {return mObstacles;}
		bool isPresent() {return mMarker->isPresent();}
		wykobi::point2d<float> & getOriginPoint(){return mOriginPoint;}
		bool IsFinished() {return mFinished;}
		void FinishMap() {mFinished = true;}
		bool IsEmptyCell(int pPositionX, int pPositionY);
		bool IsEndCell(int pPositionX, int pPositionY);
		int CalculateDistanceToTarget(wykobi::point2d<int> bugPosition);
		bool IsInsideMap(int pPointX, int pPointY);
		int GetMinDistanceNeighbors(int pX,int pY);
		int GetNumberCellInsideMap(int pRadius, int pX, int pY);
		void CheckNeighbors(int pRow,int pCol);
		void AddNeighbors(int pX,int pY);
		int GetStepsToGo(int pX, int pY) {return mStepMap[pY][pX];}
		int GetId() {return mId;}

	protected:
		void update();
		void CreateStepMap();
		void InitializeStepMap();
		bool IsDiagonalObstacle(int pPointX, int pPointY, int pDiagonal); //pDiagonal = 0 - UL ; 1 - UR ; 2 - BL ; 3 - BR


	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		FiducialMarker *mMarker;
		int mSize;
		int mEndNumber;
		int mObstaclesNumber;
		std::vector<wykobi::point2d<float>> mEndPoint;
		std::vector<wykobi::point2d<float>> mObstacles;
		wykobi::point2d<float> mOriginPoint;
		bool mFinished;
		int mStepMap[20][20]; //I assume that every map's size is less than 20
		std::vector<wykobi::point2d<int>> mNotVisitedVertexes;
		std::vector<wykobi::point2d<int>> mVisitedVertexes;
		bool mVisitedVertex[20][20]; //I assume that every map's size is less than 20

		int mId;

	};
}

#endif /* Carte_HPP_ */
