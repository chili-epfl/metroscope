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

#ifndef FractionAnswerChecker_HPP
#define FractionAnswerChecker_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <qa/utils/Time.hpp>
#include "FractionModel.hpp"
#include "Flipper.hpp"

namespace decorators {

class FractionAnswerChecker : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		FractionAnswerChecker(DecoratorManager &pDecoratorManager, Flipper *pFlipper);
		~FractionAnswerChecker();


		bool IsFlipped() const {return mFlipper->IsFlipped();}
		bool IsChecking() const {return Time::MillisTimestamp() < mFlipper->GetTimeOfLastFlip() + scANSWER_CHECKING_TIME;}
		void SetCorrectFormat(bool pIsIncorrect) {mCorrectFormat = pIsIncorrect;}
		void SetMessage(const char *pMessage){mMessage = pMessage;}

	protected:
		void update();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		FiducialMarker *mMarker2;

		Flipper *mFlipper;

		bool mCorrectFormat;

		const char *mMessage;

		void displayCheckingGraphics(const FiducialMarker *pMarker);
		void displayMessage(const FiducialMarker *pMarker);
};

}
#endif
