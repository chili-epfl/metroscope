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
#include <qa/utils/Time.hpp>

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
mPieces(pPieces),
mPhase(Placez),
mInactiveTimestamp(0)
{
}

void decorators::SentenceAnalysisModel::update() {
	if (mMarker->isPresent())
	{
		mInactiveTimestamp=0;
		if (!allPiecesPresent() && mPhase == Placez){
			displayInitialMessage();
		}else{
			mPhase=Fonction;
			displayPlacementInterface();
		}
	}else{//if the marker is not present and we had not already detected inactivity, we initialize the reset timestamp
		if(mInactiveTimestamp==0) mInactiveTimestamp = Time::MillisTimestamp();

		long currentTime = Time::MillisTimestamp();
		//if enough time has passed, we reset to the initial state of the activity
		if(currentTime>(mInactiveTimestamp+scRESET_TIMER)) mPhase = Placez;
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

			int boxWidth = static_cast<int>(mDecoratorManager.GetDisplay().GetWidth()/mNumCategories);
			int boxHeigth = static_cast<int>(mDecoratorManager.GetDisplay().GetHeight()/4);//25% of the screen

			for (int i=0;i<mNumCategories;i++){
				//Output the category boxes
				mDecoratorManager.GetDisplay().RenderQuadFilled(i*boxWidth,mDecoratorManager.GetDisplay().GetHeight()-boxHeigth,
						((i+1)*boxWidth)-1,mDecoratorManager.GetDisplay().GetHeight()-boxHeigth,
						((i+1)*boxWidth)-1,mDecoratorManager.GetDisplay().GetHeight()-1,
						i*boxWidth,mDecoratorManager.GetDisplay().GetHeight()-1,
						mCategories[i].mColor.r, mCategories[i].mColor.g, mCategories[i].mColor.b, mCategories[i].mColor.a);
				//Output the category names
				mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(mCategories[i].mName.c_str(), scTEXT_DELIMITERS,
						(i*boxWidth)+(boxWidth/2),
						mDecoratorManager.GetDisplay().GetHeight()-boxHeigth-scLINE_SPACE,boxWidth*0.9,true,1.0f,
						scGrammarBLACK.r, scGrammarBLACK.g, scGrammarBLACK.b, scGrammarBLACK.a);
			}




			//This is used to put the text on a marker?
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMessagePositionMarker, scGrammarREAL_WORLD_MARKER_WIDTH_MM,
					scGrammarREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());

			//OUtput the instruction message
			mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(scQUESTION_MESSAGE.c_str(), scTEXT_DELIMITERS,
						scGrammarMESSAGE_OFFSET_X-40, scGrammarMESSAGE_OFFSET_Y+(2*scLINE_SPACE), scGrammarMESSAGE_WIDTH,
						false, scGrammarMESSAGE_SCALE,
						scGrammarBLACK.r, scGrammarBLACK.g, scGrammarBLACK.b, scGrammarBLACK.a);




			mDecoratorManager.GetDisplay().PopTransformation();
	}
}

void decorators::SentenceAnalysisModel::Correct()
{
	//TODO: revise/delete this and implement correction
//	int tSelectedLaser = mCurrentLaser->Selection();
//	if (mAngle->Angle() < 0.0f || mAngle->Angle() > 180.0f) return;
//	mLastLaserShotTimestamp = Time::MillisTimestamp();
//	mLastLaserShotAngle = mAngle->Angle();
//	AngleModel::NormalizeAngle(mLastLaserShotAngle);
//	if (mCurrentJunk < mNJunks)
//	{
//		float tLaserX = scEarthRadius*cos(scAngleOffsets[tSelectedLaser]*wykobi::PIDiv180);
//		float tLaserY = scEarthRadius*sin(scAngleOffsets[tSelectedLaser]*wykobi::PIDiv180);
//		wykobi::vector2d<float> tJunkVector = wykobi::make_vector(mJunkXs[mCurrentJunk]-tLaserX, mJunkYs[mCurrentJunk]-tLaserY);
//		float tErrorMargin = atan2(mJunkRadiuses[mCurrentJunk], wykobi::vector_norm(tJunkVector))*wykobi::_180DivPI;
//		float tJunkAngle = wykobi::cartesian_angle(tJunkVector) - scAngleOffsets[tSelectedLaser] + 90.0f;
//		float tDifference = AngleModel::AbsoluteDifference(mLastLaserShotAngle, tJunkAngle);
//
//		if (tDifference <= tErrorMargin)
//		{
//			mJunkStates[mCurrentJunk++] = DESTROYED;
//			if (mCurrentJunk < mNJunks) mJunkStates[mCurrentJunk] = TARGET;
//		}
//	}

	if(mMarker->isPresent()){
			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarker, scGrammarREAL_WORLD_MARKER_WIDTH_MM, scGrammarREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
			mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth("Correct!", scTEXT_DELIMITERS,
						scGrammarMESSAGE_OFFSET_X, scGrammarMESSAGE_OFFSET_Y, 100.0f,
						false, scGrammarMESSAGE_SCALE,
						scGrammarBLACK.r, scGrammarBLACK.g, scGrammarBLACK.b, scGrammarBLACK.a);
			mDecoratorManager.GetDisplay().PopTransformation();
	}

}

