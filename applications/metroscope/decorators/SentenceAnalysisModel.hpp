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

#ifndef SentenceAnalysisModel_HPP
#define SentenceAnalysisModel_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "../FractionsConstants.hpp"



	static const color scGrammarGREEN			(0.15f, 1.0f, 0.15f);
	static const color scGrammarBLACK			(0.0f, 0.0f, 0.0f);
	static const color scGrammarYELLOW			(0.95f, 0.95f, 0.0f);
	static const color scGrammarRED				(1.0f, 0.25f, 0.25f);
	static const color scGrammarBLUE			(0.25f, 0.25f, 1.0f);

	static const float scGrammarREAL_WORLD_MARKER_WIDTH_MM = 20.0f;//FIXME These constants are taken from FractionsConstants.hpp, shouldn't we just include it?
	static const float scGrammarREAL_WORLD_MARKER_HEIGHT_MM = 20.0f;

	static const float scGrammarMESSAGE_OFFSET_X = 0.0f;
	static const float scGrammarMESSAGE_OFFSET_Y = 0.0f;
	static const float scGrammarMESSAGE_WIDTH = 1000.0f;
	static const float scGrammarMESSAGE_SCALE = 1.0f;


namespace decorators {

class SentenceAnalysisModel : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		SentenceAnalysisModel(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, std::string pSentence,  FiducialMarker *pTextPosition);

	protected:
		void update();
		void displayInitialMessage();


	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		const std::string mSentence;
		const FiducialMarker *mMessagePositionMarker;

};

}
#endif
