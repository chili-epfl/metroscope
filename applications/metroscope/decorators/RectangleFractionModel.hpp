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

#ifndef RectangleModel_HPP_
#define RectangleModel_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>

namespace decorators{

class RectangleFractionModel : public FiducialDecorator{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		RectangleFractionModel (DecoratorManager &pDecoratorManager, FiducialMarker *pStartRectangle, FiducialMarker *pEndRectangle);

		bool isPresent();
		const wykobi::point2d<float> &StartPoint() const {return mStartPoint;}
		const wykobi::point2d<float> &EndPoint() const {return mEndPoint;}
		void CalculateFractionFromDecimal();
		float proportion() const {return mProportion;}
		int Numerator() const {return mNumerator;}
		int Denominator() const {return mDenominator;}

	protected:
		void update();

		FiducialMarker *mStart;
		FiducialMarker *mEnd;

		wykobi::point2d<float> mStartPoint;
		wykobi::point2d<float> mEndPoint;
		float mProportion;
		int mDenominator;
		int mNumerator;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
};
}


#endif /* RECTANGLEMODEL_HPP_ */
