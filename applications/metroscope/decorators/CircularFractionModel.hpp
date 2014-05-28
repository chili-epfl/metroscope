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

		bool isPresent();
		//const wykobi::point2d<float> &StartPoint() const {return mStartPoint;}
		//const wykobi::point2d<float> &EndPoint() const {return mEndPoint;}
		float Proportion() const {return mProportion;}
		int Numerator() const {return mNumerator;}
		int Denominator() const {return mDenominator;}
		wykobi::point2d<float> GetCenter() {return mAngleModel->getMarker().getCenter();}
		const FiducialMarker& getMarker() const { return mAngleModel->getMarker(); }

	protected:
		void update();
		//wykobi::point2d<float> mStartPoint;
		//wykobi::point2d<float> mEndPoint;
		float mProportion;
		int mNumerator;
		int mDenominator;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		AngleModel * mAngleModel;

};
}



#endif /* CIRCULARFRACTIONMODEL_HPP_ */
