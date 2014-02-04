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

#include "SentenceAnalysisModel.hpp"
#include <iostream>

const std::string decorators::SentenceAnalysisModel::scDecoratorName("SentenceAnalysisModel");
const  DecoratorManager::Registerer decorators::SentenceAnalysisModel::mRegisterer(decorators::SentenceAnalysisModel::scDecoratorName, &decorators::SentenceAnalysisModel::create);

decorators::FiducialDecorator *decorators::SentenceAnalysisModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tCardStrings = pSetting["pieces"];
		int tNumCards = pSetting["num_pieces"];
		WordsCard **tWordsCards = new WordsCard *[tNumCards];
		for (int i = 0; i < tNumCards; ++i) {
			tWordsCards[i] = (WordsCard *) pDecoratorManager.loadDecorator(tCardStrings[i]);
		}

		//TODO: extract the pieces in the sentence's analysis
		//TODO: Throw an error if the analysis in the configuration file does not have the right number or type of parameters

		return new decorators::SentenceAnalysisModel(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), pSetting["sentence"], pDecoratorManager.loadMarker(pSetting["position"]), tWordsCards, tNumCards);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::SentenceAnalysisModel::SentenceAnalysisModel(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, std::string pSentence,  FiducialMarker *pTextPosition, WordsCard **pPieces, int pNumPieces):
FiducialDecorator(pDecoratorManager, pMarker),
mSentence(pSentence),
mMessagePositionMarker(pTextPosition),
mNumPieces(pNumPieces),
mPieces(pPieces)
{
}

void decorators::SentenceAnalysisModel::update() {
	if (mMarker->isPresent())
	{
		if (!allPiecesPresent()){
			displayInitialMessage();
		}else{
			displayPlacementInterface();
		}
	}
}

/*Just displays a text message in the designated position marker, with the sentence*/
void decorators::SentenceAnalysisModel::displayInitialMessage(){
	if(mMarker->isPresent()){
			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMessagePositionMarker, scGrammarREAL_WORLD_MARKER_WIDTH_MM, scGrammarREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
			mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(scINITIAL_MESSAGE.c_str(), scTEXT_DELIMITERS,
						scGrammarMESSAGE_OFFSET_X, scGrammarMESSAGE_OFFSET_Y, scGrammarMESSAGE_WIDTH,
						false, scGrammarMESSAGE_SCALE,
						scGrammarBLACK.r, scGrammarBLACK.g, scGrammarBLACK.b, scGrammarBLACK.a);
			mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(mSentence.c_str(), scTEXT_DELIMITERS,
						scGrammarMESSAGE_OFFSET_X, scGrammarMESSAGE_OFFSET_Y+scLINE_SPACE, scGrammarMESSAGE_WIDTH,
						false, scGrammarMESSAGE_SCALE,
						scGrammarBLACK.r, scGrammarBLACK.g, scGrammarBLACK.b, scGrammarBLACK.a);
			mDecoratorManager.GetDisplay().PopTransformation();
	}
}

bool decorators::SentenceAnalysisModel::allPiecesPresent(){
	bool all=true;
	for(int i=0;i<mNumPieces;i++){
		if(!mPieces[i]->getMarker().isPresent()){
			all=false;
			break;
		}
	}
	return all;
}

/*Just displays the placement exercise */
void decorators::SentenceAnalysisModel::displayPlacementInterface(){
	if(mMarker->isPresent()){
			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMessagePositionMarker, scGrammarREAL_WORLD_MARKER_WIDTH_MM, scGrammarREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
			//OUtput the instruction message
			mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(scQUESTION_MESSAGE.c_str(), scTEXT_DELIMITERS,
						scGrammarMESSAGE_OFFSET_X, scGrammarMESSAGE_OFFSET_Y+(2*scLINE_SPACE), scGrammarMESSAGE_WIDTH,
						false, scGrammarMESSAGE_SCALE,
						scGrammarBLACK.r, scGrammarBLACK.g, scGrammarBLACK.b, scGrammarBLACK.a);

			int boxWidth = static_cast<int>(mDecoratorManager.GetDisplay().GetWidth()/mNumCategories);
			int boxHeigth = static_cast<int>(mDecoratorManager.GetDisplay().GetHeight()/4);//25% of the screen

			//Output the category boxes
			/*mDecoratorManager.GetDisplay().RenderQuadFilled(0,mDecoratorManager.GetDisplay().GetHeight()-boxHeigth,
					boxWidth,mDecoratorManager.GetDisplay().GetHeight()-boxHeigth,
					boxWidth,mDecoratorManager.GetDisplay().GetHeight()-1,
					0,mDecoratorManager.GetDisplay().GetHeight()-1,
					mCategories[0].mColor.r, mCategories[0].mColor.g, mCategories[0].mColor.b, mCategories[0].mColor.a);*/
			mDecoratorManager.GetDisplay().RenderQuadFilled(0,0,
								10,0,
								10,10,
								0,10,
								mCategories[0].mColor.r, mCategories[0].mColor.g, mCategories[0].mColor.b, mCategories[0].mColor.a);
			//Output the category names

			mDecoratorManager.GetDisplay().PopTransformation();
	}
}



