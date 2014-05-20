/*
 * DeviceFileReader.cpp
 *
 *  Created on: May 20, 2014
 *      Author: lprisan
 */

#include "DeviceFileReader.hpp"

DeviceFileReader::DeviceFileReader(
		std::string pFile,
		DeviceState *pState):
mFile(pFile),
mState(pState)
{

}

DeviceFileReader::~DeviceFileReader() {
}

void DeviceFileReader::run(){

	//for testing, here we can load a JSON from a manually editable file, to emulate other thread changing the local state
	std::string jsonFile = get_file_contents(mFile.c_str());
	Json::Value json = getJSON(jsonFile);
	DeviceState tempState = getDeviceFromJSON(json);
	if(!localDevState.equals(tempState)){
		tempState.SetHasChanged(true);
		SetLocalDeviceState(tempState);
	}

}

std::string DeviceFileReader::get_file_contents(const char *filename)
{
	std::ifstream in(filename);
	std::string s((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	return(s);
}

Json::Value DeviceFileReader::getJSON(std::string jsonMessage){

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

DeviceState DeviceFileReader::getDeviceFromJSON(Json::Value value){

	//Json::FastWriter fastWriter;
	//cout << fastWriter.write(value) << endl;



	DeviceState device = DeviceState();
	Json::Value meteorId = value[scMeteorIdLabel];
	device.SetMeteorId(meteorId.asString());

	Json::Value id = value[scIdLabel];
	device.SetId(id.asInt());

	Json::Value name = value[scNameLabel];
	device.SetName(name.asString());

	activity_state activity;
	Json::Value currObj = value[scCurrentLabel];
	Json::Value present = currObj[scPresentTagsLabel];
	std::vector<std::string> tags;
	for (unsigned int i=0;i<present.size();i++){
		//cout << "Parsing array element " << i << ": " << present[i].asString() << endl;
		if(present[i].asString().length()>0){
			tags.push_back(present[i].asString());
		}
	}
	device.SetPresentTags(tags);


	Json::Value actObj = currObj[scActivityLabel];
	activity.id = actObj[scIdLabel].asInt();
	activity.name = actObj[scNameLabel].asString();
	Json::Value stateObj = actObj[scStateLabel];
	state st;
	st.denominator = stateObj[scDenominatorLabel].asInt();
	st.numerator = stateObj[scNumeratorLabel].asInt();
	st.value = stateObj[scValueLabel].asFloat();
	if(stateObj[scRepresentationLabel].asString().compare("circular")==0){
		st.representation = circular;
	} else if(stateObj[scRepresentationLabel].asString().compare("rectangular")==0){
		st.representation = rectangular;
	} else if(stateObj[scRepresentationLabel].asString().compare("tokens")==0){
		st.representation = tokens;
	} else if(stateObj[scRepresentationLabel].asString().compare("tangram")==0){
		st.representation = tangram;
	}
	activity.currentState = st;
	device.SetActivity(activity);

	device.SetHasChanged(false);

	return(device);
}
