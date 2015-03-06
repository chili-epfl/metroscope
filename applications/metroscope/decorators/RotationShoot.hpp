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

#ifndef ROTATIONSHOOT_HPP_
#define ROTATIONSHOOT_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "AngleShoot.hpp"
#include "../FractionsConstants.hpp"


namespace decorators{

static const bool scUNIT_FRACTION = true;
static const bool scUNIT_DEGREES = false;



class RotationShoot : public FiducialDecorator{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		RotationShoot (DecoratorManager &pDecoratorManager, AngleShoot * pAngleModel, bool pUnit);
		~RotationShoot() {}

		bool isPresent();
		//const wykobi::point2d<float> &StartPoint() const {return mStartPoint;}
		//const wykobi::point2d<float> &EndPoint() const {return mEndPoint;}
		float GetProportion() const {return mProportion;}
		int Numerator() const {return mNumerator;}
		int Denominator() const {return mDenominator;}
		float GetAngle() const {return mAngle;}
		wykobi::point2d<float> GetCenter() {return mAngleModel->getMarker().getCenter();}
		const FiducialMarker& getMarker() const { return mAngleModel->getMarker(); }
		//void CalculateFractionFromDecimal();
		//float SimplifyDecimal();

	protected:
		void update();
		void DisplayValue();
		//wykobi::point2d<float> mStartPoint;
		//wykobi::point2d<float> mEndPoint;
		float mProportion;
		int mNumerator;
		int mDenominator;
		float mAngle;
		bool mUnit;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		AngleShoot * mAngleModel;

};
}



#endif /* RotationShoot_HPP_ */
