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

	//if(this->mId != other->GetId()) return false;
	if(this->mMeteorId != other->GetMeteorId()) return false;
	if(this->mName.compare(other->GetName())!=0) return false;
	if(this->mPresentTags.size() != other->GetPresentTags().size()) return false;
	else{//we check all vector elements for present tags
		for(unsigned int i=0;i<this->mPresentTags.size();i++){
			if(this->mPresentTags.at(i).compare(other->GetPresentTags().at(i))!=0){
				return false;
			}
		}
	}
	//check the activity state
	if(this->mActivity.id!=other->GetActivity().id || this->mActivity.name.compare(other->GetActivity().name)!=0) return false;
	else{//we check the activity state
		if(this->mActivity.currentState.denominator != other->GetActivity().currentState.denominator ||
				this->mActivity.currentState.numerator != other->GetActivity().currentState.numerator ||
				this->mActivity.currentState.value != other->GetActivity().currentState.value ||
				this->mActivity.currentState.representation != other->GetActivity().currentState.representation) return false;
		//TODO: Maybe add a tolerance margin in value, since it is a float?
	}

	//If all else failed, they must be equal!
	return true;
}

std::string DeviceState::getJSON(){

	std::string data;

	Json::Value json;
	json[scMeteorIdLabel] = this->mMeteorId;

	//current state
	Json::Value current;

	//activity state
	Json::Value activity;
	Json::Value act_state;
	act_state[scNumeratorLabel] = this->mActivity.currentState.numerator;
	act_state[scDenominatorLabel] = this->mActivity.currentState.denominator;
	act_state[scValueLabel] = this->mActivity.currentState.value;
	if(this->mActivity.currentState.representation == circular)	act_state[scRepresentationLabel] = "circular";
	else if(this->mActivity.currentState.representation == rectangular)	act_state[scRepresentationLabel] = "rectangular";
	else if(this->mActivity.currentState.representation == tokens)	act_state[scRepresentationLabel] = "tokens";
	else if(this->mActivity.currentState.representation == tangram)	act_state[scRepresentationLabel] = "tangram";

	activity[scStateLabel] = act_state;
	activity[scIdLabel] = this->mActivity.id;
	activity[scNameLabel] = this->mActivity.name;
	current[scActivityLabel] = activity;

	//present tags
	Json::Value presentTags;
	if(this->mPresentTags.size()==0){
		current[scPresentTagsLabel] = Json::Value(Json::arrayValue);
	}else{
		for(unsigned int i=0;i<this->mPresentTags.size();i++){
			presentTags.append(this->mPresentTags.at(i));
		}
		current[scPresentTagsLabel] = presentTags;
	}

	json[scCurrentLabel] = current;

	//json[scIdLabel] = this->mId;
	json[scNameLabel] = this->mName;

	Json::FastWriter fastWriter;
	data = fastWriter.write(json);

	//std::cout << "Built JSON object: " << data << std::endl;

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

void DeviceState::setJSON(std::string jsonstring){

	Json::Value value = getJSONObject(jsonstring);

	//The JSON object can be a one-element array with the object, or only the object (depending on the Meteor API used).
	//We ensure that we have the object
	if(value.isArray()) value = value[0];


	Json::Value meteorId = value[scMeteorIdLabel];
	this->mMeteorId = meteorId.asString();

	//Json::Value id = value[scIdLabel];
	//this->mId = id.asInt();

	Json::Value name = value[scNameLabel];
	this->mName = name.asString();

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
	this->mPresentTags = tags;


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
	this->mActivity = activity;

	this->mChanged = false;//The changed value means it has NOT changed LOCALLY by another thread (rather, it comes from the server)

}
