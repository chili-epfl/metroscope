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

#include "WordsCombinator.hpp"
#include <iostream>
#include <algorithm>

const std::string decorators::WordsCombinator::scDecoratorName("WordsCombinator");
const  DecoratorManager::Registerer decorators::WordsCombinator::mRegisterer(decorators::WordsCombinator::scDecoratorName, &decorators::WordsCombinator::create);

decorators::FiducialDecorator *decorators::WordsCombinator::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tCardStrings = pSetting["possible_words"];
		int tNumCards = pSetting["num_possible_words"];
		WordsCard **tWordsCards = new WordsCard *[tNumCards];
		for (int i = 0; i < tNumCards; ++i) {
			tWordsCards[i] = (WordsCard *) pDecoratorManager.loadDecorator(tCardStrings[i]);
		}

		return new decorators::WordsCombinator(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), pDecoratorManager.loadMarker(pSetting["position"]), tWordsCards, tNumCards);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::WordsCombinator::WordsCombinator(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pTextPosition, WordsCard **pPieces, int pNumPieces):
FiducialDecorator(pDecoratorManager, pMarker),
mMessagePositionMarker(pTextPosition),
mNumPieces(pNumPieces),
mPieces(pPieces)
{
}

void decorators::WordsCombinator::update() {
	if (mMarker->isPresent())
	{
			displayCombinations();
	}

}

/*Just displays a text message in the designated position marker, with the sentence*/
void decorators::WordsCombinator::displayCombinations(){
	if(mMarker->isPresent()){

		getPresentPieces();

		getActivePieces();

		if(mActivePieces.size()>0){

			mDecoratorManager.GetDisplay().PushTransformation();
			//mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMessagePositionMarker, scGrammarREAL_WORLD_MARKER_WIDTH_MM, scGrammarREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
			mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(scCOMBINATOR_INTRO.c_str(), scTEXT_DELIMITERS,
					    mDecoratorManager.GetDisplay().GetWidth()/2, scGUIDELINE_DISTANCE, scGrammarMESSAGE_WIDTH,
						false, scGrammarMESSAGE_SCALE,
						scGrammarBLACK.r, scGrammarBLACK.g, scGrammarBLACK.b, scGrammarBLACK.a);

			generateCombinations();

			for(int i=0;i<std::min(mNumCombinations,scDISPLAYED_COMBINATIONS);i++){
				mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(mCombinations[i].c_str(), scTEXT_DELIMITERS,
										mDecoratorManager.GetDisplay().GetWidth()/2, scGUIDELINE_DISTANCE+((i+1)*scLINE_SPACE), scGrammarMESSAGE_WIDTH,
										false, scCOMBINATIONS_SCALE,
										scGrammarBLACK.r, scGrammarBLACK.g, scGrammarBLACK.b, scGrammarBLACK.a);
			}

			mDecoratorManager.GetDisplay().PopTransformation();
		}
	}
}

void decorators::WordsCombinator::getPresentPieces(){
	mPresentPieces.clear();

	for(int i=0; i<mNumPieces;i++){
		if(mPieces[i]->getMarker().isPresent()){
			mPresentPieces.push_back(mPieces[i]);
		}
	}

}

void decorators::WordsCombinator::getActivePieces(){

	mActivePieces.clear();

	//we calculate the range of screen that we are checking (y values around the corrector marker)
	float minimum_y = mMessagePositionMarker->getCenter().y-scGUIDELINE_DISTANCE;
	float maximum_y = mMessagePositionMarker->getCenter().y+scGUIDELINE_DISTANCE;

	//get the markers present, their position, and y-position
	for(int i=0;i<mPresentPieces.size();i++){
		if(mPresentPieces[i]->getMarker().getCenter().y>=minimum_y && mPresentPieces[i]->getMarker().getCenter().y<=maximum_y){
			mActivePieces[mPresentPieces[i]->getMarker().getCenter().x] = mPresentPieces[i];
		}
	}

}

void decorators::WordsCombinator::generateCombinations(){
	mNumCombinations=0;
	mCombinations = NULL;

	if(mActivePieces.size()>0){
		mNumCombinations = factorial(mActivePieces.size());
		mCombinations = new std::string[mNumCombinations];

		std::string activeWords[mActivePieces.size()];

		//We put the map into a vector
		int i=0;
	    for( std::map<float,WordsCard *>::iterator it = mActivePieces.begin(); it != mActivePieces.end(); ++it ) {
	    	activeWords[i] = it->second->GetWords();
	    	i++;
	    }

		std::sort (activeWords,activeWords+mActivePieces.size());

		//we do the permutations themselves
		int currentIndex=0;
		do
		  {
			mCombinations[currentIndex]="";
		    for (int it=0; it<mActivePieces.size(); ++it)
		    {
		    	mCombinations[currentIndex]=mCombinations[currentIndex]+activeWords[it]+" ";
		    }
	        currentIndex++;
		  } while (std::next_permutation(activeWords, activeWords + mActivePieces.size()));


	}


}

int decorators::WordsCombinator::factorial(int n)
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

