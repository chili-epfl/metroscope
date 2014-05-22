/*
 * DeviceState.cpp
 *
 *  Created on: May 13, 2014
 *      Author: lprisan
 */

#include "DeviceState.hpp"

DeviceState::DeviceState() {
	mChanged = false;
	mId = 1;
}

DeviceState::~DeviceState() {
	// TODO Auto-generated destructor stub
}

bool DeviceState::equals(DeviceState* other){

	if(this->mId != other->GetId()) return false;
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

	json[scIdLabel] = this->mId;
	json[scNameLabel] = this->mName;

	Json::FastWriter fastWriter;
	data = fastWriter.write(json);

	//cout << "Built JSON object: " << data << endl;

	return data;
}
