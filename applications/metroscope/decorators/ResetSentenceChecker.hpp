/*******************************************************************************
*   Copyright 2014 EPFL                                                        *
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

#ifndef ResetSentenceChecker_HPP
#define ResetSentenceChecker_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "Flipper.hpp"
#include "GrammarChecker.hpp"
#include "../FractionsConstants.hpp"

static const std::string scRESET_QUESTION = "êtes-vous sûr que vous voulez réinitialiser l'archive des phrases?";
static const std::string scRESET_MESSAGE = "l'archive a été réinitialisé";
static const float scRESET_SCALE = 1.0f;
static const float scRESET_MESSAGE_OFFSET_X = 0.0f;
static const float scRESET_MESSAGE_OFFSET_Y = 40.0f;
static const float scRESET_MESSAGE_WIDTH = 600.0f;
static const float scRESET_MESSAGE_TIMER = 3.0f*1000.0f;

namespace decorators {

class ResetSentenceChecker : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		ResetSentenceChecker(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, Flipper * pFlipper, GrammarChecker *pChecker);


	protected:
		void update();
		void displayConfirmationQuestion();
		void displayConfirmationReset(int numSentencesDeleted);

		Flipper *mFlipper;
		GrammarChecker *mChecker;
		long mLastShot;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
};

}
#endif
