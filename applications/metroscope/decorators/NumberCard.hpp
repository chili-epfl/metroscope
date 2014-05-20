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

#ifndef NumberCard_HPP
#define NumberCard_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <stdio.h>
#include <stdlib.h>


namespace decorators {

class NumberCard : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		NumberCard(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const int pNumber);
		virtual ~NumberCard(){};

		int GetNumber() const {return mNumber;}
		bool IsPresent()const {return mMarker->isPresent();}
		bool NumberIsInRectangle() {return isInsideRectangle;}
		void setCardInsideRectangle(bool pInsideRectangle){
			isInsideRectangle = pInsideRectangle;
		}
		wykobi::point2d<float> GetLocation() const {
			wykobi::point2d<float> tLocation;
			if (mMarker->isPresent())return mMarker->getCenter();
			else return tLocation;
		}

		const FiducialMarker *GetMarker() const {return mMarker;}

	protected:
		void update(){}
		int mNumber;
		bool isInsideRectangle;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;


};

}
#endif