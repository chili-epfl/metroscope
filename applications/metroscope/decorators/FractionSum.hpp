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

#ifndef FRACTIONSUM_HPP_
#define FRACTIONSUM_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "CircularFractionModel.hpp"

namespace decorators{

class FractionSum : public FiducialDecorator{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		FractionSum (DecoratorManager &pDecoratorManager,FiducialMarker *pMarker, CircularFractionModel** pCircularModel, int pCircularManipulatives); //For now this, later we'll see if we need more

	protected:
		void update();
		int mCircularNumber;
		float mSum;
		int mNumerator;
		int mDenominator;
		float GetProportion() const {return mSum;}
		int Numerator() const {return mNumerator;}
		int Denominator() const {return mDenominator;}
		//wykobi::point2d<float> GetCenter(int pManipulative) {return mCircularModel[pManipulative]->GetCenter();}
		//const FiducialMarker& getMarker(int pManipulative) const {return mCircularModel[pManipulative]->getMarker();}
		void CheckManipulativesPresent();
		bool mActivityPresent;
		//void CalculateFractionFromDecimal();
		//float SimplifyDecimal();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		CircularFractionModel** mCircularModel;
		std::vector<CircularFractionModel*> mActiveCircularModel;
};
}



#endif /* FRACTIONSUM_HPP_ */
