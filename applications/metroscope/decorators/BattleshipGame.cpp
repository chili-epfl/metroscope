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

#include "BattleshipGame.hpp"
#include <qa/utils/Time.hpp>
#include <wykobi/wykobi_utilities.hpp>

const std::string decorators::BattleshipGame::scDecoratorName("BattleshipGame");
const DecoratorManager::Registerer decorators::BattleshipGame::mRegisterer(decorators::BattleshipGame::scDecoratorName, &decorators::BattleshipGame::create);

decorators::FiducialDecorator *decorators::BattleshipGame::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager){
	try{
		int tNumPolygons = pSetting["num_polygons"];

		libconfig::Setting &tPolygonsString = pSetting ["polygons"];

		//TODO: maybe do it as a vector?
		PolyModel **tPolyModels = new PolyModel *[tNumPolygons];

		for(int i = 0 ; i < tNumPolygons ; ++i){
			tPolyModels[i] = (PolyModel *) pDecoratorManager.loadDecorator(tPolygonsString[i]);
		}

		return new decorators::BattleshipGame (pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), tNumPolygons,
				(PolyModel **) tPolyModels);

	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::BattleshipGame::BattleshipGame(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker,
		int pNumPolygons, PolyModel ** pPolyModels):
			FiducialDecorator(pDecoratorManager, pMarker),
			mNumPolygons(pNumPolygons),
			mPolyModels(pPolyModels)
{
}

decorators::BattleshipGame::~BattleshipGame(){ /*Empty*/}

void decorators::BattleshipGame::update(){

	//We display the phase message in the top of the screen
	DisplayPhase();
	//If we are in the new turn, main menu or game over or game win, we grey out the screen
	std::string tPhase = stateManager->getPhase();
	if(tPhase.compare(scGameNewTurn)==0 ||
			tPhase.compare(scMainMenu)==0 ||
			tPhase.compare(scGameOver)==0 ||
			tPhase.compare(scGameWin)==0 ||
			tPhase.compare(scGameResolve)==0) GreyOutScreen();


}

void decorators::BattleshipGame::DisplayPhase(){

	std::ostringstream tMessage;
	//We generate the title for this phase
	int tTurn = stateManager->getTurn();
	std::string tPhase = stateManager->getPhase();
	if(tTurn!=0){
		tMessage << "Turn " << tTurn << " - ";
	}

	if(tPhase.compare(scGameNewTurn)==0){
		tMessage << "New Turn";
	} else if(tPhase.compare(scMainMenu)==0){
		tMessage << "";
	} else if(tPhase.compare(scGameOver)==0){
		tMessage << "Game Over";
	} else if(tPhase.compare(scGameWin)==0){
		tMessage << "Well Done!";
	} else if(tPhase.compare(scGameAnalysis)==0){
		tMessage << "Analysis Phase";
	} else if(tPhase.compare(scGameShoot)==0){
		tMessage << "Shooting Phase";
	} else if(tPhase.compare(scGameResolve)==0){
		tMessage << "Results";
	}

	const std::string tmp = tMessage.str();

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarker, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
		mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(tmp.c_str(), scTEXT_DELIMITERS,
				0.0, 100.0, 1200,
				false, 3,
				0.0, 0.0, 0.0, 1.0);
//std::cout << "Just printed " << tmp.c_str() << " at " << mMarker->getCenter().x << "," << mMarker->getCenter().y;
		mDecoratorManager.GetDisplay().PopTransformation();

}

void decorators::BattleshipGame::GreyOutScreen(){
	mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().RenderQuadFilled(0,0,
				mDecoratorManager.GetDisplay().GetWidth(),0,
				mDecoratorManager.GetDisplay().GetWidth(),mDecoratorManager.GetDisplay().GetHeight(),
				0,mDecoratorManager.GetDisplay().GetHeight(),
				scBLACK.r, scBLACK.g, scBLACK.b, 0.5f);
		mDecoratorManager.GetDisplay().PopTransformation();

}

//void decorators::BattleshipGame::UpdateDeviceStatus(){
//	//We update the device's networked state
//	int distance = CalculateDistanceToTarget(mMapSize, mBugPosition, mActualCarte->getEndPoint(), mActualCarte->getObstaclesPoint());
//	stateManager->SetActivityStepsToGo(distance);
//
//	if(IsHintPresent()){
//		std::ostringstream convert;   // stream used for the conversion
//		convert << mActualHint->GetHintType();      // insert the textual representation of 'Number' in the characters in the stream
//		std::string hint = convert.str(); // set 'Result' to the contents of the stream
//		stateManager->SetActivityHintPresent(hint);
//	}else stateManager->SetActivityHintPresent("");
//
//	stateManager->SetActivityStepsDone(mSteps);
//}
