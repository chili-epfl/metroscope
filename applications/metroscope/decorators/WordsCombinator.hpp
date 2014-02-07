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

#ifndef WordsCombinator_HPP
#define WordsCombinator_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "../FractionsConstants.hpp"
#include "WordsCard.hpp"
#include "SentenceAnalysisModel.hpp"


	static const std::string scCOMBINATOR_INTRO = "Quelles combinations sont correctes?";
	static const float scCOMBINATIONS_SCALE = 0.7f;


namespace decorators {

class WordsCombinator : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		WordsCombinator(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pTextPosition, WordsCard **pPieces, int pNumPieces);

	protected:
		void update();
		void displayCombinations();
		void getPiecesPresent();
		void generateCombinations();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		const FiducialMarker *mMessagePositionMarker;

		const int mNumPieces;
		WordsCard **mPieces; //array of pointers to all of the WordsCard objects

		int mNumPresentPieces;
		WordsCard **mPresentPieces;//array of pieces present at any given moment

		int mNumCombinations;
		std::string *mCombinations;//array of combined phrases

		int factorial(int n);
};

}
#endif
