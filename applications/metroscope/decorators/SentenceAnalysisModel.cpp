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

		return new decorators::SentenceAnalysisModel(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), pSetting["sentence"], pDecoratorManager.loadMarker(pSetting["position"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::SentenceAnalysisModel::SentenceAnalysisModel(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, std::string pSentence,  FiducialMarker *pTextPosition):
FiducialDecorator(pDecoratorManager, pMarker),
mSentence(pSentence),
mMessagePositionMarker(pTextPosition)
{
}

void decorators::SentenceAnalysisModel::update() {
	if (mMarker->isPresent())
	{
		displayInitialMessage();
	}
}

/*Just displays a text message next to the marker*/
void decorators::SentenceAnalysisModel::displayInitialMessage(){
	if(mMarker->isPresent()){
			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMessagePositionMarker, scGrammarREAL_WORLD_MARKER_WIDTH_MM, scGrammarREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
			mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(mSentence.c_str(), scTEXT_DELIMITERS,
						scGrammarMESSAGE_OFFSET_X, scGrammarMESSAGE_OFFSET_Y, scGrammarMESSAGE_WIDTH,
						false, scGrammarMESSAGE_SCALE,
						scGrammarBLACK.r, scGrammarBLACK.g, scGrammarBLACK.b, scGrammarBLACK.a);
			mDecoratorManager.GetDisplay().PopTransformation();
	}
}
