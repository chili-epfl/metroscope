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

#ifndef NumberDial_HPP
#define NumberDial_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "FractionModel.hpp"
#include "VariableFractionCard.hpp"



namespace decorators {

class NumberDial : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		NumberDial(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker,
					FiducialMarker *pStabilizedGraphicsMarker,
					const int pMin,
					const int pRange,
					const int pNumbersInCircle,
					const int pStartNumber,
					const color *pHighlightColor);
		~NumberDial();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;





		FiducialMarker *mStabilizedGraphicsMarker;

		const int mMin;
		const int mRange;
		const int mNumbersInCircle;


		float mNotchesFromTop;


		void drawDialSelectorBox(wykobi::point2d<float> tCenter, float pAngle);
		int modulo(int x, int y);
		float fmodulo(float x, float y);

	protected:
		virtual void update();
		void updateValue(float pAngle);
		void displayDialNumbers(float pAngle);


		const color *mHighlightColor;

		int mCurrentNumber;

};

}
#endif
