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

#ifndef FractionComparisonModel_HPP
#define FractionComparisonModel_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "FractionModel.hpp"
#include "FractionActivityCard.hpp"


namespace decorators {

class FractionComparisonModel : public FractionActivityCard
{
	protected:

		virtual bool properOperatorsPresent();
		virtual bool getOperands();
		virtual bool answerIsCorrect ();
		virtual void updateHintState();
		virtual void updateActiveCards();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		FractionModel *mFractionModel;

		enum ComparisonType {GreaterThan, LesserThan, EqualTo};
		ComparisonType mComparisonType;

		FractionCard *mLHS;
		FractionCard *mRHS;

	public:
			static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
			FractionComparisonModel(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker,
									FiducialMarker *pStabilizedGraphicsMarker,
									FractionModel *pModel,
									FractionAnswerChecker *pChecker,
									ComparisonType pComparisonType,
									const char *pActivityName,
									const color *pActivityColor);

			~FractionComparisonModel();


};

}
#endif
