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
#include <iostream>
#include <string>

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

		return new decorators::BattleshipGame (pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),
				tNumPolygons, (PolyModel **) tPolyModels,
				(RotationShoot *) pDecoratorManager.loadDecorator(pSetting["rotation"]),
				(LinearShoot *) pDecoratorManager.loadDecorator(pSetting["linear_x"]),
				(LinearShoot *) pDecoratorManager.loadDecorator(pSetting["linear_y"]),
				(FiducialMarker *) pDecoratorManager.loadMarker(pSetting["shooting_mark"]));

	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::BattleshipGame::BattleshipGame(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker,
		int pNumPolygons, PolyModel ** pPolyModels,
		RotationShoot *pRotation, LinearShoot *pLinearX, LinearShoot *pLinearY, FiducialMarker *pMarkerShoot):
			FiducialDecorator(pDecoratorManager, pMarker),
			mNumPolygons(pNumPolygons),
			mPolyModels(pPolyModels),
			mRotation(pRotation),
			mLinearX(pLinearX),
			mLinearY(pLinearY),
			mMarkerShoot(pMarkerShoot)
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
	else if(tPhase.compare(scGameShoot)==0){//If we are in the shooting phase, we display the selected polygon, and the rotated/translated version

		DisplayPolygonAxes();

		if(isPolygonPresent()){

			DisplayFirstPolygon();

			if( mLinearX->isPresent() && mLinearY->isPresent()){

				DisplayTranslationArrow();

			}

			if( mRotation->isPresent() ){

				DisplayRotationAngle();

			}

			DisplayTransformedPolygon();



		}

		//We update the shooting information
		//We setup the shoot state for the next phase, with this team's id
		std::string tTeamId = stateManager->getShoot().team_id;
		shoot tShoot;
		tShoot.team_id = tTeamId;
		tShoot.rotation = (this->mRotation->GetProportion())*360;
		tShoot.translation = wykobi::make_point(this->mLinearX->proportion(),this->mLinearY->proportion());
		tShoot.polygon = this->getFirstPolygon();
		stateManager->SetShoot(tShoot);


	}

}

bool decorators::BattleshipGame::isPolygonPresent(){

	for(unsigned int i=0; i<mNumPolygons; i++){
		if(mPolyModels[i]->getMarker().isPresent()) return true;
	}
	return false;
}

wykobi::polygon<float,2> decorators::BattleshipGame::getFirstPolygon(){

	wykobi::polygon<float,2> newPoly;
	for(unsigned int i=0; i<mNumPolygons; i++){
		if(mPolyModels[i]->getMarker().isPresent()){
			return mPolyModels[i]->getCoords();
		}
	}
	return newPoly;


}


void decorators::BattleshipGame::DisplayPolygonAxes(){

	std::ostringstream tmp;
	tmp << "x";
	std::string tmp2 = tmp.str();
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarkerShoot, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
	mDecoratorManager.GetDisplay().RenderCenteredText(tmp2.c_str(), 210.0, 0.0, true, 1.0, 0.0, 0.0, 0.0, 1.0);//X axis
	mDecoratorManager.GetDisplay().RenderLine(-100.0, 0.0, 200.0, 0.0, 0.0, 0.0, 0.0, 1.0);//X axis

	std::ostringstream tmp3;
	tmp3 << "y";
	std::string tmp4 = tmp3.str();
	mDecoratorManager.GetDisplay().RenderLine(0.0, 100.0, 0.0, -200.0, 0.0, 0.0, 0.0, 1.0);//y axis - it is inverted from the coordinates!
	mDecoratorManager.GetDisplay().RenderCenteredText(tmp4.c_str(), 0.0, -210.0, true, 1.0, 0.0, 0.0, 0.0, 1.0);//y axis
//	std::cout << "Rendered lines!" << std::endl;

	mDecoratorManager.GetDisplay().PopTransformation();


}



void decorators::BattleshipGame::DisplayFirstPolygon(){

	PolyModel *tPolygon;
	for(unsigned int i=0; i<mNumPolygons; i++){
		if(mPolyModels[i]->getMarker().isPresent()) tPolygon = mPolyModels[i];
	}


	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarkerShoot, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());

	//We display the polygon
	wykobi::polygon<float, 2> poly = tPolygon->getPolygon();
	poly = wykobi::translate(-tPolygon->getOrigin().x, -tPolygon->getOrigin().y, poly);
	poly = wykobi::mirror(poly, wykobi::make_line(0.0f, 0.0f, 100.0f, 0.0f));//we flip it so that they y coordinates match the direction of the screen
	poly = wykobi::scale(scMM2DisplayMult, scMM2DisplayMult, poly);//we scale it to visible size
	std::vector<float> vertices = polygonToVertices(poly);
	mDecoratorManager.GetDisplay().RenderPolygon(vertices, 0.0f, 0.0f, 0.0f, 0.3f);


	//We display the origin marker
	mDecoratorManager.GetDisplay().RenderEllipse(0.0f, 0.0f, 10.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.3f);


	mDecoratorManager.GetDisplay().PopTransformation();

}

std::vector<float> decorators::BattleshipGame::polygonToVertices(wykobi::polygon<float, 2> polygon){
	std::vector<float> vertices;

	for(unsigned int i=0; i<polygon.size(); i++){
		vertices.push_back(polygon[i].x);
		vertices.push_back(polygon[i].y);
	}
	return vertices;
}


void decorators::BattleshipGame::DisplayTranslationArrow(){

	if(mLinearX->isPresent() && mLinearY->isPresent()){

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarkerShoot, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());

		wykobi::point2d<float> endpoint = wykobi::make_point(mLinearX->proportion(), mLinearY->proportion());//We construct the endpoint representing the translation
		endpoint = wykobi::mirror(endpoint, wykobi::make_line(0.0f, 0.0f, 100.0f, 0.0f));
		endpoint = wykobi::scale(scMM2DisplayMult*scBoard2MMMult, scMM2DisplayMult*scBoard2MMMult, endpoint);
//		std::cout << "Rendered lines to " << endpoint.x << "," << endpoint.y << std::endl;

		mDecoratorManager.GetDisplay().RenderLine(0.0f, 0.0f, endpoint.x, endpoint.y, 0.0f, 0.0f, 0.0f, 1.0f);

		mDecoratorManager.GetDisplay().PopTransformation();

	}



}

void decorators::BattleshipGame::DisplayRotationAngle(){

	float translationx;
	float translationy;
	if(!mLinearX->isPresent()){
		translationx = 0.0f;
	}else{
		translationx = mLinearX->proportion();
	}
	if(!mLinearY->isPresent()){
		translationy = 0.0f;
	}else{
		translationy = mLinearY->proportion();
	}

	if(mRotation->isPresent()){

		float rotation = 360.0f - mRotation->GetAngle();

		//We draw the two segments, and the arc in between, always starting at x axis
		wykobi::segment<float,2> line1 = wykobi::make_segment(0.0f, 0.0f, 100.0f, 0.0f);
		wykobi::segment<float,2> line2 = wykobi::rotate(rotation, line1);

		//We calculate the translation to where this will have to be drawn
		wykobi::point2d<float> endpoint = wykobi::make_point(translationx, translationy);//We construct the endpoint representing the translation
		endpoint = wykobi::mirror(endpoint, wykobi::make_line(0.0f, 0.0f, 100.0f, 0.0f));
		endpoint = wykobi::scale(scMM2DisplayMult*scBoard2MMMult, scMM2DisplayMult*scBoard2MMMult, endpoint);

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarkerShoot, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());

		mDecoratorManager.GetDisplay().RenderLine(endpoint.x, endpoint.y, endpoint.x+100.0, endpoint.y, 0.0f, 0.0f, 0.0f, 1.0f);
		mDecoratorManager.GetDisplay().RenderLine(endpoint.x, endpoint.y, endpoint.x+line2[1].x, endpoint.y+line2[1].y, 0.0f, 0.0f, 0.0f, 1.0f);
//		std::cout << "Rendered lines from " << endpoint.x << "," << endpoint.y << " to " << endpoint.x+line2[1].x << "," << endpoint.y+line2[1].y << std::endl;

		mDecoratorManager.GetDisplay().RenderArc(endpoint.x, endpoint.y, 30.0f, 30.0f, 0.0f, rotation, 0.0f, 0.0f, 0.0f, 1.0f);

		mDecoratorManager.GetDisplay().PopTransformation();



	}


}

void decorators::BattleshipGame::DisplayTransformedPolygon(){

	float translationx;
	float translationy;
	float rotation;
	if(!mLinearX->isPresent()){
		translationx = 0.0f;
	}else{
		translationx = mLinearX->proportion();
	}
	if(!mLinearY->isPresent()){
		translationy = 0.0f;
	}else{
		translationy = mLinearY->proportion();
	}
	if(mRotation->isPresent()){
		rotation = 360.0f - mRotation->GetAngle();
	}else{
		rotation = 0.0f;
	}

	if(isPolygonPresent()){

		//We calculate the translation to where this will have to be drawn
		wykobi::point2d<float> endpoint = wykobi::make_point(translationx, translationy);//We construct the endpoint representing the translation
		endpoint = wykobi::mirror(endpoint, wykobi::make_line(0.0f, 0.0f, 100.0f, 0.0f));
		endpoint = wykobi::scale(scMM2DisplayMult*scBoard2MMMult, scMM2DisplayMult*scBoard2MMMult, endpoint);


		PolyModel *tPolygon;//We get the first polygon present
		for(unsigned int i=0; i<mNumPolygons; i++){
			if(mPolyModels[i]->getMarker().isPresent()) tPolygon = mPolyModels[i];
		}


		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarkerShoot, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());

		//We display the polygon
		wykobi::polygon<float, 2> poly = tPolygon->getPolygon();
		poly = wykobi::translate(-tPolygon->getOrigin().x, -tPolygon->getOrigin().y, poly);
		poly = wykobi::mirror(poly, wykobi::make_line(0.0f, 0.0f, 100.0f, 0.0f));//we flip it so that they y coordinates match the direction of the screen
		poly = wykobi::scale(scMM2DisplayMult, scMM2DisplayMult, poly);//we scale it to visible size

		//We further translate it by the endpoint
		poly = wykobi::translate(endpoint.x, endpoint.y, poly);
		//We rotate it
		poly = wykobi::rotate(rotation,poly, endpoint);

		std::vector<float> vertices = polygonToVertices(poly);
		mDecoratorManager.GetDisplay().RenderPolygon(vertices, 0.0f, 0.0f, 0.0f, 1.0f);


		//We display the origin marker
		mDecoratorManager.GetDisplay().RenderEllipse(endpoint.x, endpoint.y, 10.0f, 10.0f, 0.0f, 0.0f, 0.0f, 1.0f);


		mDecoratorManager.GetDisplay().PopTransformation();



	}



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