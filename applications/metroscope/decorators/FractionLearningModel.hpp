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

#ifndef FRACTIONLEARNINGMODE_HPP_
#define FRACTIONLEARNINGMODE_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "KillBugModel.hpp"
#include "FractionComparisonView.hpp"
#include "FractionViewMeasure.hpp"
#include "EquivalentFractionView.hpp"

namespace decorators{

class FractionLearningModel : public FiducialDecorator
{
	public:
		static FiducialDecorator *create (libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		FractionLearningModel(DecoratorManager &pDecoratorManager , KillBugModel *pKillBugModel , FractionComparisonView *pComparisonGreater,
				FractionComparisonView *pComparisonSmaller, FractionComparisonView *pComparisonEqual, FractionViewMeasure **pFractionView, EquivalentFractionView *pEquivalentFraction,
				int pConstructionActivity);
		~FractionLearningModel();

	protected:
		void update();
		bool IsPresentFractionComparison();
		bool IsPresentFractionConstruction();
		void SetFractionComparison(bool pIsCurrentActivity);
		void SetFractionConstruction(bool pIsCurrentActivity);


	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		FractionViewMeasure **mFractionConstruction;
		EquivalentFractionView *mFractionEquivalent;
		FractionComparisonView *mFractionComparisonGreater;
		FractionComparisonView *mFractionComparisonSmaller;
		FractionComparisonView *mFractionComparisonEqual;
		KillBugModel *mKillBugGame;
		int mConstructionActivity;
};

}

#endif /* FRACTIONLEARNINGMODE_HPP_ */

