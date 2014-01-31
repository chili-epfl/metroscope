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
#include "WordsModel.hpp"

	//COLORS
	struct color{
		float r;
		float g;
		float b;
		float a;
		color(float pR, float pG, float pB, float pA = 1.0f) {r = pR; g = pG; b = pB; a = pA;}
	};

	//Grammatical groups, e.g. sujet, verbe, complement du verbe... and associated colors
	struct grammar_category{
		std::string name;
		color category_color;
		grammar_category(std::string pName, color pColor){name = pName; category_color = pColor;}
	};

	//Analysis = vector of [category, vector of sentence pieces in this category]
	struct analysis_element{
		grammar_category category;
		int num_pieces;
		WordsModel **pieces;
		analysis_element(grammar_category pCat, int num_pieces, WordsModel *pPieces){category = pCat; pieces = pPieces;}
	};
	struct analysis{
		analysis_element *elements;
		int num_categories;
		analysis(int pNumCategories, analysis_element *)
	};


	static const color scGREEN			(0.15f, 1.0f, 0.15f);
	static const color scBLACK			(0.0f, 0.0f, 0.0f);
	static const color scYELLOW			(0.95f, 0.95f, 0.0f);
	static const color scRED			(1.0f, 0.25f, 0.25f);
	static const color scBLUE			(0.25f, 0.25f, 1.0f);


namespace decorators {

class SentenceAnalysisModel : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		SentenceAnalysisModel(DecoratorManager &pDecoratorManager, std::string pSentence, int pNumCategories, const int pNumPieces, WordsModel **pPieces);

	protected:
		void update();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		const std::vector<grammar_category> cCategories;
};

}
#endif
