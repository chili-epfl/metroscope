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

#ifndef AngleViewMeasure_HPP
#define AngleViewMeasure_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "AngleModel.hpp"
#include <fstream>

namespace decorators {

class AngleViewMeasure : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		AngleViewMeasure(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, AngleModel *pAngleModel);
		AngleViewMeasure(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, AngleModel *pAngleModel, float pExpectedAngle, float pTol_correct, float pTol_close);

	protected:
		void update();

		AngleModel *mAngleModel;
		bool mColors;
		float mExpectedAngle;
		float mCorrectTolerance;
		float mCloseTolerance;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
};

}
#endif
