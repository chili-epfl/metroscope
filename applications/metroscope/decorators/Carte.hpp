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
		Carte (DecoratorManager &pDecoratorManager,
				FiducialMarker *pMarker,
				const int pSize,
				float pOriginX, float pOriginY,
				const int pEndNumber,
				const std::vector<wykobi::point2d<float>> &pEndPoint,
				const int pObstaclesNumber,
				const std::vector<wykobi::point2d<float>> &pObstacles);

		Carte (DecoratorManager &pDecoratorManager,
						FiducialMarker *pMarker,
						const int pSize,
						float pOriginX, float pOriginY,
						const int pEndNumber,
						const std::vector<wykobi::point2d<float>> &pEndPoint,
						const int pObstaclesNumber);

		int getSize() {return mSize;}
		int getEndNumber() {return mEndNumber;}
		int getObstacleNumber() {return mObstaclesNumber;}
		std::vector<wykobi::point2d<float>> & getEndPoint(){return mEndPoint;}
		std::vector<wykobi::point2d<float>> & getObstaclesPoint() {return mObstacles;}
		bool isPresent() {return mMarker->isPresent();}
		wykobi::point2d<float> & getOriginPoint(){return mOriginPoint;}

	protected:
		void update();

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
	};
}

#endif /* Carte_HPP_ */
