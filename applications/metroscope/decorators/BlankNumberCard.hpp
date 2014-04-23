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

#ifndef BLANKNUMBERCARD_HPP_
#define BLANKNUMBERCARD_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "../ArithmeticsConstants.hpp"


namespace decorators{

	class BlankNumberCard : public FiducialDecorator
	{
	public:
		static FiducialDecorator *create (libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		BlankNumberCard (DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, int pType); //pType: 0= units, 1=tens and 2= cents
		~BlankNumberCard (){}

		//void SetNumber (int pNumber) { mNumber = pNumber; }
		void SetNumber (int pNumber);
		void SetOperator (int pOperator) { mOperator = pOperator; }
		int GetNumber () const {return mNumber;}
		char * GetNumberText () const {return mNumberText;}
		bool IsPresent()const {return mMarker->isPresent();}
		wykobi::point2d<float> GetLocation() const {
			wykobi::point2d<float> tLocation;
			if (mMarker->isPresent())return mMarker->getCenter();
			else return tLocation;
		}
		//void DisplayNumber(const char *pNumber, float pXOffset, float pYOffset);

		const FiducialMarker *GetMarker() const {return mMarker;}
		bool IsCardInsideRectangle(); //int pType
		bool IsCardInsideBigRectangles();
		float mR;
		float mG;
		float mB;
		int mType;

		void DisplayNumber(bool tStackedCard, bool tHasChange, int tChange);

	protected:
		void update();

	private:

		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		int mNumber;
		int mOperator; //0 first operator or 1 second operator
		bool mIsInsideRectangle;
		char * mNumberText;

	};
}

#endif /* BLANKNUMBERCARD_HPP_ */
