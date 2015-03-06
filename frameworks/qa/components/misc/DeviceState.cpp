/*
 * DeviceState.cpp
 *
 *  Created on: May 13, 2014
 *      Author: lprisan
 */

#include "DeviceState.hpp"

DeviceState::DeviceState() {
	mChanged = false;
}

DeviceState::~DeviceState() {
	// TODO Auto-generated destructor stub
}

bool DeviceState::equals(DeviceState* other){

	//We check the current moves
	if(this->mDevice.currentMove.size() != other->GetDevice().currentMove.size()) return false;
	for(unsigned int i=0; i < this->mDevice.currentMove.size(); i++){
		if(!equalMoves(this->mDevice.currentMove.at(i),other->GetDevice().currentMove.at(i))) return false;
	}

	if(this->mDevice.moves.size() != other->GetDevice().moves.size()) return false;
	for(unsigned int i=0; i < this->mDevice.moves.size(); i++){
		if(!equalMoves(this->mDevice.moves.at(i),other->GetDevice().moves.at(i))) return false;
	}


	//If all else failed, they must be equal!
	return true;
}


bool DeviceState::equalMoves(move move1, move move2){

	//std::cout << "Comparing " << moveToString(move1);
	if(move1.activity_id != move2.activity_id) return false;
	else if(move1.illegal != move2.illegal) return false;
	else if(move1.meteorId.compare(move2.meteorId)!=0) return false;
	else if(move1.origin.x != move2.origin.x || move1.origin.y != move2.origin.y) return false;
	else if(move1.rotation != move2.rotation) return false;
	else if(move1.team != move2.team) return false;
	else if(move1.turn != move2.turn) return false;
	else if(move1.translation.x != move2.translation.x || move1.translation.y != move2.translation.y) return false;
	else if(move1.polygon.size() != move2.polygon.size()) return false;
	for(unsigned int i=0; i < move1.polygon.size(); i++){//We check each polygon vertex
		if(move1.polygon[i].x != move2.polygon[i].x || move1.polygon[i].y != move2.polygon[i].y) return false;
	}
	//If all else failed, they must be equal!
	return true;
}


std::string DeviceState::getJSON(){

	std::string data;

	Json::Value json = Json::Value(Json::arrayValue); //The moves json is always an array

	if(this->mDevice.moves.size()!=0){//There are some moves

		for(unsigned int i=0; i < this->mDevice.moves.size(); i++){
			Json::Value move;

			//Convert the move structure to json
			move[scActivityIdLabel] = this->mDevice.moves.at(i).activity_id;
			move[scTurn] = this->mDevice.moves.at(i).turn;
			move[scTeamLabel] = this->mDevice.moves.at(i).team;

			Json::Value origin = Json::Value(Json::arrayValue);
			origin.append(this->mDevice.moves.at(i).origin.x);
			origin.append(this->mDevice.moves.at(i).origin.y);
			move[scOriginLabel] = origin;

			Json::Value polygon = Json::Value(Json::arrayValue);
			for(unsigned int j=0; j<this->mDevice.moves.at(i).polygon.size(); j++){//For each polygon vertex
				Json::Value vertex = Json::Value(Json::arrayValue);
				vertex.append(this->mDevice.moves.at(i).polygon[j].x);
				vertex.append(this->mDevice.moves.at(i).polygon[j].y);
				polygon.append(vertex);
			}
			move[scPolygonLabel] = polygon;

			move[scRotationLabel] = this->mDevice.moves.at(i).rotation;

			Json::Value translation = Json::Value(Json::arrayValue);
			translation.append(this->mDevice.moves.at(i).translation.x);
			translation.append(this->mDevice.moves.at(i).translation.y);
			move[scTranslationLabel] = translation;

			move[scIllegalLabel] = this->mDevice.moves.at(i).illegal;
			move[scMeteorIdLabel] = this->mDevice.moves.at(i).meteorId;

			json.append(move);
		}

	}



	Json::FastWriter fastWriter;
	data = fastWriter.write(json);

	std::cout << "Built JSON object: " << data << std::endl;

	return data;
}

Json::Value DeviceState::getJSONObject(std::string jsonMessage){

	Json::Value parsedFromString;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(jsonMessage, parsedFromString);
	if (parsingSuccessful)
	{
		//Json::StyledWriter styledWriter;
		//std::cout << styledWriter.write(parsedFromString) << std::endl;
		//std::cout << "Parsing successful!" << std::endl;
		//std::cout << jsonMessage << std::endl;
		return(parsedFromString);
	}
	return(parsedFromString);
}

void DeviceState::setJSON(std::string jsonstring, int pTurn){

	Json::Value value = getJSONObject(jsonstring);

	//The JSON object should always be an array

	if(!value.isArray()){
		std::cout << "Error! The moves in the device should be an array!" << std::endl;
		return;
	}else{
		std::vector<move> moves;
		std::vector<move> currentMove;
		for(unsigned int i=0; i<value.size(); i++){
			Json::Value jsonmove = value[i];
			move parsedMove;

			parsedMove.activity_id = jsonmove[scActivityIdLabel].asInt();
			parsedMove.turn = jsonmove[scTurn].asInt();
			parsedMove.team = jsonmove[scTeamLabel].asInt();

			parsedMove.origin = wykobi::make_point(jsonmove[scOriginLabel][0].asFloat(),jsonmove[scOriginLabel][1].asFloat());

			Json::Value jsonpolygon = jsonmove[scPolygonLabel];
			wykobi::polygon<float, 2> polygon;
			for(unsigned j=0; j<jsonpolygon.size(); j++){
				polygon.push_back(wykobi::make_point(jsonpolygon[j][0].asFloat(),jsonpolygon[j][1].asFloat()));
			}
			parsedMove.polygon = polygon;

			parsedMove.rotation = jsonmove[scRotationLabel].asInt();

			parsedMove.translation = wykobi::make_point(jsonmove[scTranslationLabel][0].asFloat(),jsonmove[scTranslationLabel][1].asFloat());

			parsedMove.illegal = jsonmove[scIllegalLabel].asBool();
			parsedMove.meteorId = jsonmove[scMeteorIdLabel].asString();

			//TODO: We only add to the state the turns until current one -1 (we do not want to plot the current one in case it has been posted already
			if(jsonmove[scTurn].asInt() < pTurn){
				moves.push_back(parsedMove);
			}else if(jsonmove[scTurn].asInt() == pTurn){
				//set the current (last turn) value? we just equal it to current turn (it may be empty/null!)
				currentMove.push_back(parsedMove);
			}//And if it's greater we just ignore them!

		}

		//We add the move to the device state
		device_state tState;
		tState.moves = moves;
		tState.currentMove = currentMove;
		this->mDevice = tState;
		this->mChanged = false;//The changed value means it has NOT changed LOCALLY by another thread (rather, it comes from the server)
	}


}
