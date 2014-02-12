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

#include "GrammarChecker.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>
#include <vector>
#include <map>

const std::string decorators::GrammarChecker::scDecoratorName("GrammarChecker");
const  DecoratorManager::Registerer decorators::GrammarChecker::mRegisterer(decorators::GrammarChecker::scDecoratorName, &decorators::GrammarChecker::create);

decorators::FiducialDecorator *decorators::GrammarChecker::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tCardStrings = pSetting["pieces"];
		int tNumCards = pSetting["num_pieces"];
		WordsCard **tWordsCards = new WordsCard *[tNumCards];
		for (int i = 0; i < tNumCards; ++i) {
			tWordsCards[i] = (WordsCard *) pDecoratorManager.loadDecorator(tCardStrings[i]);
		}

		return new decorators::GrammarChecker(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), tWordsCards, tNumCards);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::GrammarChecker::GrammarChecker(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, WordsCard **pPieces, int pNumPieces):
FiducialDecorator(pDecoratorManager, pMarker),
mNumPieces(pNumPieces),
mPieces(pPieces)
{
}

void decorators::GrammarChecker::update() {
	if (mMarker->isPresent())
	{
		//check which other markers are present and within the same y-range that this one is, and construct the phrase
		std::string sentence = getAlignedMarkersSentence();

		std::string message="";
		bool isCorrect=false;

		//call the grammar analyzer
		Correct(sentence, &message, &isCorrect);

		//display message or hints
		displayMessageHint(message, isCorrect);
	}
	//TODO: implement correction timer to avoid trial and error strategy
}


void decorators::GrammarChecker::Correct(std::string pSentence, std::string *message, bool *isCorrect)
{
	//TODO: eliminate this!
	*isCorrect = true;
	*message = pSentence;

	if(mMarker->isPresent()){
		//TODO: call some kind of grammar analyzer (e.g. on the web?)

		//TODO: also, we could call google and calculate the points obtained by this phrase if it is correct

	}

}

void decorators::GrammarChecker::displayMessageHint(std::string pMessage, bool pIsCorrect){

	//We only display the correction if there is any feedback to give to the user
	if(pMessage.length()>0){
		mDecoratorManager.GetDisplay().PushTransformation();
		//display the color bar indicating if it is correct or not
		mDecoratorManager.GetDisplay().RenderQuadFilled(0.0f, mMarker->getCenter().y+scGUIDELINE_DISTANCE,
				mDecoratorManager.GetDisplay().GetWidth(), mMarker->getCenter().y+scGUIDELINE_DISTANCE,
				mDecoratorManager.GetDisplay().GetWidth(), mMarker->getCenter().y-scGUIDELINE_DISTANCE,
				0.0f, mMarker->getCenter().y-scGUIDELINE_DISTANCE,
				scGREEN.r, scGREEN.g, scGREEN.b, scGREEN.a);

		//display the message or hint - GET RID OF MAGIC NUMBERS!
		mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(pMessage.c_str(), scTEXT_DELIMITERS,
					600.0f, mMarker->getCenter().y-100.0f, 600.0f,
					false, 1.0f,
					scBLACK.r, scBLACK.g, scBLACK.b, scBLACK.a);
		mDecoratorManager.GetDisplay().PopTransformation();
	}



}

std::string decorators::GrammarChecker::getAlignedMarkersSentence(){

	std::string sentence = "";


	std::map<float, std::string> presentPieces;


	//we calculate the range of screen that we are checking (y values around the corrector marker)
	float minimum_y = mMarker->getCenter().y-scGUIDELINE_DISTANCE;
	float maximum_y = mMarker->getCenter().y+scGUIDELINE_DISTANCE;

	//get the markers present, their position, and y-position
	for(int i=0;i<mNumPieces;i++){
		if(mPieces[i]->getMarker().isPresent() && mPieces[i]->getMarker().getCenter().y>=minimum_y && mPieces[i]->getMarker().getCenter().y<=maximum_y){
			presentPieces[mPieces[i]->getMarker().getCenter().x] = mPieces[i]->GetWords();
		}
	}

	//construct a sentence with the markers in range
	for(std::map<float,std::string>::iterator i=presentPieces.begin(); i!=presentPieces.end(); ++i){
		if(sentence.length()==0) sentence += i->second;
		else sentence += " "+i->second;
	}

	return sentence;

}
