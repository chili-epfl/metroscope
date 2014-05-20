/*
 * PutRemoteJSON.cpp
 *
 *  Created on: May 20, 2014
 *      Author: lprisan
 */

#include "PutRemoteJSON.hpp"

PutRemoteJSON::PutRemoteJSON(
		std::string pUrl,
		const NetworkedState *pState):
mUrl(pUrl),
mState(pState)
{

}

PutRemoteJSON::~PutRemoteJSON() {
	delete mUrl;
}

void PutRemoteJSON::run(){

	if(mState->hasChanged()){//if some other thread changed the device state
		//copy the local state to a temp variable
		DeviceState state = *mState;

		//convert the object to JSON
		std::string jsonState = getJSONFromDevice(state);
		std::cout << "Detected state change: " << jsonState << std::endl;

		//do PUT of the JSON object to the server
		bool success = false;
		std::cout << "PUTting remote JSON...";
		success = putRemoteString(mUrl,jsonState);
		if(success) std::cout << "Success!" << std::endl;
		else std::cout << "Failure!" << std::endl;
	}

}

std::string putRemoteJSON::getJSONFromDevice(DeviceState state){

	std::string data;

	Json::Value json;
	json[scMeteorIdLabel] = state.GetMeteorId();

	//current state
	Json::Value current;

	//activity state
	Json::Value activity;
	Json::Value act_state;
	act_state[scNumeratorLabel] = state.GetActivity().currentState.numerator;
	act_state[scDenominatorLabel] = state.GetActivity().currentState.denominator;
	act_state[scValueLabel] = state.GetActivity().currentState.value;
	if(state.GetActivity().currentState.representation == circular)	act_state[scRepresentationLabel] = "circular";
	else if(state.GetActivity().currentState.representation == rectangular)	act_state[scRepresentationLabel] = "rectangular";
	else if(state.GetActivity().currentState.representation == tokens)	act_state[scRepresentationLabel] = "tokens";
	else if(state.GetActivity().currentState.representation == tangram)	act_state[scRepresentationLabel] = "tangram";

	activity[scStateLabel] = act_state;
	activity[scIdLabel] = state.GetActivity().id;
	activity[scNameLabel] = state.GetActivity().name;
	current[scActivityLabel] = activity;

	//present tags
	Json::Value presentTags;
	if(state.GetPresentTags().size()==0){
		current[scPresentTagsLabel] = Json::Value(Json::arrayValue);
	}else{
		for(int i=0;i<state.GetPresentTags().size();i++){
			presentTags.append(state.GetPresentTags().at(i));
		}
		current[scPresentTagsLabel] = presentTags;
	}

	json[scCurrentLabel] = current;

	json[scIdLabel] = state.GetId();
	json[scNameLabel] = state.GetName();

	Json::FastWriter fastWriter;
	data = fastWriter.write(json);

	//cout << "Built JSON object: " << data << endl;

	return data;
}

bool PutRemoteJSON::putRemoteString(std::string pUrl, std::string data){

	CURLcode code(CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init();
	bool success = false;
	if (curl) {
		struct curl_slist *headers=NULL; /* init to NULL is important */
		//headers = curl_slist_append(headers, client_id_header);
	    headers = curl_slist_append(headers, "Content-Type: application/json");

	    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	    curl_easy_setopt(curl, CURLOPT_URL, pUrl.c_str());
	    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); /* !!! */

	    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str()); /* data goes here */

	    code = curl_easy_perform(curl);
	    if(code==CURLE_OK) success = true;

	    curl_slist_free_all(headers);
	    curl_easy_cleanup(curl);
	}
	return(success);
}

