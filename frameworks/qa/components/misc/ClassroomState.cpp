/*
 * ClassroomState.cpp
 *
 *  Created on: May 14, 2014
 *      Author: lprisan
 */

#include "ClassroomState.hpp"

ClassroomState::ClassroomState() {
	// TODO Auto-generated constructor stub

}

ClassroomState::~ClassroomState() {
	// TODO Auto-generated destructor stub
}

bool ClassroomState::equals(ClassroomState* other){

	if(this->mId != other->GetId()) return false;
	if(this->mMeteorId != other->GetMeteorId()) return false;
	if(this->mName.compare(other->GetName())!=0) return false;
	if(this->mDevices.size() != other->GetDevices().size()) return false;
	else{//we check all vector elements for present tags
		for(int i=0;i<this->mDevices.size();i++){
			if(this->mDevices.at(i)!=other->GetDevices().at(i)){
				return false;
			}
		}
	}
	//check the global class variables state
	if(this->mGlobal.paused!=other->GetGlobal().paused) return false;

	//If all else failed, they must be equal!
	return true;
}

void ClassroomState::setJSON(std::string jsonstring){

	Json::Value json = getJSONObject(jsonstring);

	Json::Value meteorId = value[scMeteorIdLabel];
	mMeteorId = meteorId.asString();

	Json::Value id = value[scIdLabel];
	mId = id.asInt();

	Json::Value name = value[scNameLabel];
	mName = name.asString();

	Json::Value devices = value[scDevicesLabel];
	std::vector<int> devs;
	for (unsigned int i=0;i<devices.size();i++){
		//cout << "Parsing array element " << i << ": " << present[i].asString() << endl;
		devs.push_back(devices[i].asInt());
	}
	mDevices = devs;


	Json::Value global = value[scGlobalClassVariables];
	global_class classVariables;
	classVariables.paused = global[scPaused].asBool();
	mGlobal = classVariables;

	mChanged = false;

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
