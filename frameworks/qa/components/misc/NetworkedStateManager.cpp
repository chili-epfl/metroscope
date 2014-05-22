/*
 * NetworkedStateManager.cpp
 *
 *  Created on: May 22, 2014
 *      Author: lprisan
 */

#include "NetworkedStateManager.hpp"


NetworkedStateManager::NetworkedStateManager() {
	mDeviceState = new DeviceState;
	devstate_mutex = PTHREAD_MUTEX_INITIALIZER;
	mClassroomState = new ClassroomState;
	classstate_mutex = PTHREAD_MUTEX_INITIALIZER;
}

NetworkedStateManager::NetworkedStateManager(std::string pDeviceMeteorId, std::string pClassroomMeteorId) {
	mDeviceState = new DeviceState;
	devstate_mutex = PTHREAD_MUTEX_INITIALIZER;
	mClassroomState = new ClassroomState;
	classstate_mutex = PTHREAD_MUTEX_INITIALIZER;
	SetDeviceMeteorId(pDeviceMeteorId);
	SetClassroomMeteorId(pClassroomMeteorId);
}

NetworkedStateManager::~NetworkedStateManager() {
	// TODO Auto-generated destructor stub
}

void NetworkedStateManager::SetDeviceMeteorId(std::string pId){

	pthread_mutex_lock(&devstate_mutex);
	std::string oldId = mDeviceState->GetMeteorId();
	if(oldId.compare(pId)!=0){
		mDeviceState->SetMeteorId(pId);
		mDeviceState->SetHasChanged(true);
	}
	pthread_mutex_unlock(&devstate_mutex);

}

void NetworkedStateManager::SetClassroomMeteorId(std::string pId){

	pthread_mutex_lock(&classstate_mutex);
	std::string oldId = mClassroomState->GetMeteorId();
	if(oldId.compare(pId)!=0){
		mClassroomState->SetMeteorId(pId);
		mClassroomState->SetHasChanged(true);
	}
	pthread_mutex_unlock(&classstate_mutex);

}


bool NetworkedStateManager::hasDeviceChanged(){

	bool change;
	pthread_mutex_lock(&devstate_mutex);
	change = mDeviceState->hasChanged();
	pthread_mutex_unlock(&devstate_mutex);
	return change;
}

bool NetworkedStateManager::hasClassroomChanged(){

	bool change;
	pthread_mutex_lock(&classstate_mutex);
	change = mClassroomState->hasChanged();
	pthread_mutex_unlock(&classstate_mutex);
	return change;
}

std::string NetworkedStateManager::getDeviceJSON(){

	std::string json;
	pthread_mutex_lock(&devstate_mutex);
	json = mDeviceState->getJSON();
	pthread_mutex_unlock(&devstate_mutex);
	return json;
}

void NetworkedStateManager::SetHasDeviceChanged(bool changed){

	pthread_mutex_lock(&devstate_mutex);
	mDeviceState->SetHasChanged(changed);
	pthread_mutex_unlock(&devstate_mutex);
}

void NetworkedStateManager::addMarkerToDeviceState(std::string tagName){

	bool found=false;
	pthread_mutex_lock(&devstate_mutex);
	std::vector<std::string> tags = mDeviceState->GetPresentTags();
	for(unsigned int i=0;i<tags.size();i++){
		if(tags.at(i).compare(tagName)==0){
			found=true;//if the tag is already in the vector, later we will do nothing
		}
	}
	//if we did not find the tag in the vector, we add the tag to it
	if(!found){
		tags.push_back(tagName);
		mDeviceState->SetPresentTags(tags);
		mDeviceState->SetHasChanged(true);
	}
	pthread_mutex_unlock(&devstate_mutex);
}

void NetworkedStateManager::removeMarkerFromDeviceState(std::string tagName){
	pthread_mutex_lock(&devstate_mutex);
	std::vector<std::string> tags = mDeviceState->GetPresentTags();

	bool changed = false;

	for(std::vector<std::string>::iterator it = tags.begin(); it != tags.end(); ){
		if(it->compare(tagName)==0){
			//if we find the tag, we pop it out
			tags.erase(it);
			changed = true;
		} else {
			++it;
		}
	}

	if(changed){
		mDeviceState->SetPresentTags(tags);
		mDeviceState->SetHasChanged(true);
	}
	//if we did not find the tag in the vector, we do nothing
	pthread_mutex_unlock(&devstate_mutex);
}

void NetworkedStateManager::SetClassroomJSON(std::string jsonData){

	pthread_mutex_lock(&classstate_mutex);
	mClassroomState->setJSON(jsonData);
	pthread_mutex_unlock(&classstate_mutex);
}
