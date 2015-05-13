/*
 * ClassroomState.cpp
 *
 *  Created on: May 14, 2014
 *      Author: lprisan
 */

#include "ClassroomState.hpp"

ClassroomState::ClassroomState() {//The initialization always sets paused to false, so that by default the behavior is as usual
	struct global_class global;
	global.paused=false;
//	global.pauserDevice="";
	global.phase="";
	global.turn=0;
	global.title="";
//	global.num_virus_cells=0;
	mGlobal = global;
}

ClassroomState::~ClassroomState() {
	// TODO Auto-generated destructor stub
}

bool ClassroomState::equals(ClassroomState* other){

	//if(this->mId != other->GetId()) return false;
	if(this->mMeteorId != other->GetMeteorId()) return false;
//	if(this->mName.compare(other->GetName())!=0) return false;
//	if(this->mDevices.size() != other->GetDevices().size()) return false;
//	else{//we check all vector elements for present tags
//		for(unsigned int i=0;i<this->mDevices.size();i++){
//			if(this->mDevices.at(i).compare(other->GetDevices().at(i))!=0){
//				return false;
//			}
//		}
//	}
	//check the global class variables state
	//if(this->mGlobal.paused!=other->GetGlobal().paused || this->mGlobal.pauserDevice.compare(other->GetGlobal().pauserDevice)!=0) return false;

	if(this->mGlobal.paused!=other->GetGlobal().paused) return false;
	if(this->mGlobal.phase.compare(other->GetGlobal().phase)!=0) return false;
	if(this->mGlobal.title.compare(other->GetGlobal().title)!=0) return false;
	if(this->mGlobal.turn != other->GetGlobal().turn) return false;

//	if(this->mGlobal.num_virus_cells != other->GetGlobal().num_virus_cells) return false;
//
//	for(int i=0; i<4; i++){
//		if(this->mGlobal.team_points[i] != other->GetGlobal().team_points[i]) return false;
//	}
//
//	for(int i=0; i<201; i++){
//		for(int j=0; j<201; j++){
//			if(this->mGlobal.board[i][j] != other->GetGlobal().board[i][j]) return false;
//		}
//	}

	//If all else failed, they must be equal!
	return true;
}

void ClassroomState::setJSON(std::string jsonstring){

	//std::cout << "Trying to set classroom state with data: " << jsonstring << std::endl;

	Json::Value value = getJSONObject(jsonstring);

	//The JSON object can be a one-element array with the object, or only the object (depending on the Meteor API used).
	//We ensure that we have the object
	if(value.isArray()) value = value[0];

	Json::Value meteorId = value[scMeteorIdLabel];
	mMeteorId = meteorId.asString();

	//Json::Value id = value[scIdLabel];
	//mId = id.asInt();

//	Json::Value name = value[scNameLabel];
//	mName = name.asString();

//	Json::Value devices = value[scDevicesLabel];
//	//std::cout << "devices element: is array? " << devices.isArray() << "; elements: " << devices.size() << std::endl;
//	std::vector<std::string> devs;
//	for (unsigned int i=0;i<devices.size();i++){
//		//std::cout << "Parsing array element " << i << ": " << devices[i].asInt() << std::endl;
//		devs.push_back(devices[i].asString());
//	}
//	mDevices = devs;


	Json::Value global = value[scCurrentStateClassVariables];
	global_class classVariables;
	classVariables.paused = !(global[scActive].asBool());
//	classVariables.pauserDevice = global[scPauserDevice].asString();
//	classVariables.phase = global[scMasterHint].asString();
	classVariables.phase = global[scPhase].asString();
	classVariables.title = value[scTitle].asString();
	classVariables.turn = global[scTurn].asInt();
//	classVariables.num_virus_cells = global[scNumVirusLabel].asInt();
//	Json::Value rows = global[scBoardLabel];
//	if(!rows.isArray()){
//		std::cout << "Something's wrong, the board is not an array!" << std::endl;
//		return;
//	}else{
//		for(int i=0; i<rows.size(); i++){
//			Json::Value cells = rows[i];
//			if(!cells.isArray()){
//					std::cout << "Something's wrong, the board rows are not an array!" << std::endl;
//					return;
//				}else{
//					for(int j=0;j<rows[i].size();j++) classVariables.board[i][j] = cells[j].asInt();
//				}
//		}
//	}
//
//	for(int i=1; i<=4; i++){
//		std::ostringstream variable;
//		variable << scTeamPointsLabel1 << i << scTeamPointsLabel2;
//		classVariables.team_points[i-1] = global[variable.str()].asInt();
//	}

	mGlobal = classVariables;

	this->mChanged = false;

	//std::cout << "Classroom state set. Title " << mGlobal.title << std::endl;

}

std::string ClassroomState::getJSON(bool pAlternate){

	std::string data;

	Json::Value json;
	//json[scIdLabel] = this->mId;
	//json[scNameLabel] = this->mName;

	if(!pAlternate)	json[scMeteorIdLabel] = this->mMeteorId;//The normal encoding sets the meteor _id
	else json[scClassroomIdLabel] = this->mMeteorId;//The alternate encoding sets the classroomid

	json[scTitle] = this->mGlobal.title;


	//devices
/*
	Json::Value devices;
	if(this->mDevices.size()==0){
		json[scDevicesLabel] = Json::Value(Json::arrayValue);
	}else{
		for(unsigned int i=0;i<this->mDevices.size();i++){
			devices.append(this->mDevices.at(i));
		}
		json[scDevicesLabel] = devices;
	}
*/

	//global variables
	Json::Value global;
	global[scActive] = !(this->mGlobal.paused);
//	global[scPauserDevice] = this->mGlobal.pauserDevice;
	global[scPhase] = this->mGlobal.phase;
	global[scTurn] = this->mGlobal.turn;
//	global[scNumVirusLabel] = this->mGlobal.num_virus_cells;
//	for(int i=1; i<=4; i++){
//		std::ostringstream variable;
//		variable << scTeamPointsLabel1 << i << scTeamPointsLabel2;
//		global[variable.str()] = this->mGlobal.team_points[i-1];
//	}
//
//	Json::Value board;
//	for(int i=0; i<201; i++){
//		Json::Value row;
//		for(int j=0;j<201;j++){
//			row.append(this->mGlobal.board[i][j]);
//		}
//		board.append(row);
//	}
//	json[scBoardLabel] = board;

	json[scCurrentStateClassVariables] = global;


	Json::FastWriter fastWriter;
	data = fastWriter.write(json);

	std::cout << "Built JSON object: " << data << std::endl;

	return data;
}



Json::Value ClassroomState::getJSONObject(std::string jsonMessage){

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
