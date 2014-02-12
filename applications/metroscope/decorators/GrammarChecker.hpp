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

#ifndef GrammarChecker_HPP
#define GrammarChecker_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "../FractionsConstants.hpp"
#include "WordsCard.hpp"



namespace decorators {

class GrammarChecker : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		GrammarChecker(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, WordsCard **pPieces, int pNumPieces);

		void Correct(std::string pSentence, std::string *message, bool *isCorrect);
		void displayMessageHint(std::string pMessage, bool pIsCorrect);
		std::string getAlignedMarkersSentence();

	protected:
		void update();


	private:

		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		const int mNumPieces;
		WordsCard **mPieces; //array of pointers to all of the WordsCard objects
};

}
#endif
