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

#ifndef CIRCULARFRACTIONMODEL_HPP_
#define CIRCULARFRACTIONMODEL_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "AngleModel.hpp"

namespace decorators{

class CircularFractionModel : public FiducialDecorator{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		CircularFractionModel (DecoratorManager &pDecoratorManager, AngleModel * pAngleModel);
		~CircularFractionModel() {}

		bool IsPresent();
		float GetProportion() const {return mProportion;}
		int Numerator() const {return mNumerator;}
		int Denominator() const {return mDenominator;}
		float GetAngle() const {return mAngle;}
		wykobi::point2d<float> GetCenter() {return mAngleModel->getMarker().getCenter();}
		const FiducialMarker& GetMarker() const { return mAngleModel->getMarker(); }
		void CalculateFractionFromDecimal();
		float SimplifyDecimal();

	protected:
		void update();
		float mProportion;
		int mNumerator;
		int mDenominator;
		float mAngle;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		AngleModel * mAngleModel;

};
}

#endif /* CIRCULARFRACTIONMODEL_HPP_ */
