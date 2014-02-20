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

#ifndef WordsCluster_HPP
#define WordsCluster_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <wykobi/wykobi.hpp>
#include "../FractionsConstants.hpp"
#include "WordsCard.hpp"


	static const std::string scCLUSTER_CORRECT_MSG = "Ça y est! Les groupes sont correctes :)";
	static const float scCLUSTER_MSG_SCALE = 1.0f;
	static const float scCLUSTER_MSG_WIDTH = 600.0f;

	static const float scCLUSTER_RANGE = 250.0f;
	static const float scCLUSTER_OVERFLOW = 100.0f;

namespace decorators {

class WordsCluster : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		WordsCluster(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pTextPosition, WordsCard **pPieces, int pNumPieces);

	protected:
		void update();
		void displayMessage();
		void displayClusters();
		void getPresentPieces();
		void getClusters();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		const FiducialMarker *mMessagePositionMarker;

		const int mNumPieces;
		WordsCard **mPieces; //array of pointers to all of the WordsCard objects

		std::vector<WordsCard *> mPresentPieces;//array of pieces present at any given moment

		std::vector<std::vector<WordsCard *>> mClusters;//array of word card clusters

};

}
#endif
