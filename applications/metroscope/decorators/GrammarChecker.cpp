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
#include "../CURLHelper.hpp"

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
		//call some kind of grammar analyzer (e.g. on the web?), possibly using http://curl.haxx.se/libcurl/c/simple.html
		//Correctors: http://wiki.languagetool.org/http-server
		//TODO: since there is a limitation in the number of requests, we can eventually run a local checker server, as described in http://wiki.languagetool.org/http-server
		//TODO: also, find a way of not blocking the interface for 1 sec while the request is sent and processed! for now, just comment this out and always say it is correct
//		CURLHelper request;
//		std::string urlEncodedSentence = request.urlEncode(pSentence);
//		request.makeRequest("https://languagetool.org:8081/?language=fr&text="+urlEncodedSentence);
//		std::string response = request.getData();
//		*message = response;
//		request.cleanup();

		//TODO: also, we could call google and calculate the points obtained by this phrase if it is correct
	}

}

void decorators::GrammarChecker::displayMessageHint(std::string pMessage, bool pIsCorrect){

	wykobi::point2d<float> sentenceCenter = getActiveSentenceCenter();

	//We only display the correction if there is any feedback to give to the user
	if(pMessage.length()>0){
		mDecoratorManager.GetDisplay().PushTransformation();
		//display the color bar indicating if it is correct or not
		/*mDecoratorManager.GetDisplay().RenderQuadFilled(0.0f, sentenceCenter.y+scGUIDELINE_DISTANCE,
				mDecoratorManager.GetDisplay().GetWidth(), sentenceCenter.y+scGUIDELINE_DISTANCE,
				mDecoratorManager.GetDisplay().GetWidth(), sentenceCenter.y-scGUIDELINE_DISTANCE,
				0.0f, sentenceCenter.y-scGUIDELINE_DISTANCE,
				scGREEN.r, scGREEN.g, scGREEN.b, 0.3f);*/
		mDecoratorManager.GetDisplay().RenderQuad(0.0f, sentenceCenter.y+scGUIDELINE_DISTANCE,
				mDecoratorManager.GetDisplay().GetWidth(), sentenceCenter.y+scGUIDELINE_DISTANCE,
				mDecoratorManager.GetDisplay().GetWidth(), sentenceCenter.y-scGUIDELINE_DISTANCE,
				0.0f, sentenceCenter.y-scGUIDELINE_DISTANCE,
				scGREEN.r, scGREEN.g, scGREEN.b, 1.0f);

		//display the message or hint - GET RID OF MAGIC NUMBERS!
		mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(pMessage.c_str(), scTEXT_DELIMITERS,
					600.0f, sentenceCenter.y-100.0f, 600.0f,
					false, 1.0f,
					scBLACK.r, scBLACK.g, scBLACK.b, scBLACK.a);

		//display the message or hint - GET RID OF MAGIC NUMBERS!
		mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth("C'est correct!", scTEXT_DELIMITERS,
					600.0f, sentenceCenter.y+100.0f, 600.0f,
					false, 1.0f,
					scGREEN.r, scGREEN.g, scGREEN.b, scGREEN.a);
		mDecoratorManager.GetDisplay().PopTransformation();
	}



}

void decorators::GrammarChecker::getPresentPieces(){
	mPresentPieces.clear();

	for(int i=0; i<mNumPieces;i++){
		if(mPieces[i]->getMarker().isPresent()){
			mPresentPieces.push_back(mPieces[i]);
		}
	}

}

void decorators::GrammarChecker::getActivePieces(){

	activePieces.clear();

	//we calculate the range of screen that we are checking (y values around the corrector marker)
	float minimum_y = mMarker->getCenter().y-scGUIDELINE_DISTANCE;
	float maximum_y = mMarker->getCenter().y+scGUIDELINE_DISTANCE;

	//get the markers present, their position, and y-position
	for(int i=0;i<mPresentPieces.size();i++){
		if(mPresentPieces[i]->getMarker().getCenter().y>=minimum_y && mPresentPieces[i]->getMarker().getCenter().y<=maximum_y){
			activePieces[mPresentPieces[i]->getMarker().getCenter().x] = mPresentPieces[i];
		}
	}

}


std::string decorators::GrammarChecker::getAlignedMarkersSentence(){

	std::string sentence = "";

	getPresentPieces();

	getActivePieces();

	//construct a sentence with the markers in range
	for(std::map<float,WordsCard *>::iterator i=activePieces.begin(); i!=activePieces.end(); ++i){
		if(sentence.length()==0) sentence += i->second->GetWords();
		else sentence += " "+i->second->GetWords();
	}

	return sentence;

}

wykobi::point2d<float> decorators::GrammarChecker::getActiveSentenceCenter(){

	//we calculate the cluster's center and radius
	std::vector<wykobi::point2d<float>> sentencePoints;
	for(std::map<float,WordsCard *>::iterator i=activePieces.begin(); i!=activePieces.end(); ++i) sentencePoints.push_back(i->second->GetLocation());

	//Depending on the number of points, we create a polygon... or just a point or a line
	wykobi::point2d<float> center;
	if(sentencePoints.size()==1) center = sentencePoints[0];
	else if(sentencePoints.size()==2){
		center.x = (sentencePoints[0].x+sentencePoints[1].x)/2;
		center.y = (sentencePoints[0].y+sentencePoints[1].y)/2;
	}else{
		wykobi::polygon<float,2> cardsPolygon = wykobi::make_polygon<float>(sentencePoints);
		center = wykobi::centroid(cardsPolygon);
	}

	return center;
}

