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

#ifndef SQUAREFRACTIONCARD_HPP_
#define SQUAREFRACTIONCARD_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>

namespace decorators{
	class SquareFractionCard : public FiducialDecorator{

	public:
		static FiducialDecorator *create (libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		SquareFractionCard (DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const int pDenom, int pType);

		bool IsPresent() const {return mMarker->isPresent();}

		wykobi::point2d<float> GetLocation() const {
					wykobi::point2d<float> tLocation;
					if (mMarker->isPresent())return mMarker->getCenter();
					else return tLocation;
		}

		const FiducialMarker *GetMarker() const {return mMarker;}
		//bool pointIsInRange(wykobi::point2d<float> tReferencePoint, wykobi::point2d<float> tPoint);
		bool pointIsInRange(wykobi::point2d<float> tPoint);

		int getDenom(){return mDenom;}
		int getType(){return mType;}

		bool isInCorrectPlace(FiducialMarker *pMarker);

	protected:
		void update();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		std::vector<wykobi::point2d<float>> mExpectedLocation;
		std::vector<float> mPosibleAngle;

		int mDenom;
		int mType;
	};
}

#endif /* SQUAREFRACTIONCARD_HPP_ */
