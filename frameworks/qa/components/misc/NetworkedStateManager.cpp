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
	//SetDeviceMeteorId(pDeviceMeteorId);
	SetClassroomMeteorId(pClassroomMeteorId);
}

NetworkedStateManager::~NetworkedStateManager() {
	// TODO Auto-generated destructor stub
}

//void NetworkedStateManager::SetDeviceMeteorId(std::string pId){
//
//	pthread_mutex_lock(&devstate_mutex);
//	std::string oldId = mDeviceState->GetMeteorId();
//	if(oldId.compare(pId)!=0){
//		mDeviceState->SetMeteorId(pId);
//		mDeviceState->SetHasChanged(true);
//	}
//	pthread_mutex_unlock(&devstate_mutex);
//
//}

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

std::string NetworkedStateManager::getClassroomJSON(){
	std::string json;
	pthread_mutex_lock(&classstate_mutex);
	json = mClassroomState->getJSON();
	pthread_mutex_unlock(&classstate_mutex);
	return json;
}

std::string NetworkedStateManager::getAlternateDeviceJSON(){

	std::string json;
	pthread_mutex_lock(&devstate_mutex);
	json = mDeviceState->getJSON();
	pthread_mutex_unlock(&devstate_mutex);
	return json;
}

std::string NetworkedStateManager::getAlternateClassroomJSON(){
	std::string json;
	pthread_mutex_lock(&classstate_mutex);
	json = mClassroomState->getJSON(true);
	pthread_mutex_unlock(&classstate_mutex);
	return json;
}




void NetworkedStateManager::SetHasDeviceChanged(bool changed){

	pthread_mutex_lock(&devstate_mutex);
	mDeviceState->SetHasChanged(changed);
	pthread_mutex_unlock(&devstate_mutex);
}

void NetworkedStateManager::SetHasClassroomChanged(bool changed){

	pthread_mutex_lock(&classstate_mutex);
	mClassroomState->SetHasChanged(changed);
	pthread_mutex_unlock(&classstate_mutex);
}


void NetworkedStateManager::SetClassroomJSON(std::string jsonData){

	pthread_mutex_lock(&classstate_mutex);
	mClassroomState->setJSON(jsonData);
	pthread_mutex_unlock(&classstate_mutex);
}

void NetworkedStateManager::SetDeviceJSON(std::string jsonData){

	pthread_mutex_lock(&devstate_mutex);
	mDeviceState->setJSON(jsonData, this->getTurn());
	pthread_mutex_unlock(&devstate_mutex);
}

void NetworkedStateManager::SetClassroomPaused(bool paused){

	pthread_mutex_lock(&classstate_mutex);
	bool oldPause = mClassroomState->GetGlobal().paused;
//	std::string oldPauserDevice = mClassroomState->GetGlobal().pauserDevice;
//	std::string oldMasterHint = mClassroomState->GetGlobal().masterHint;
	std::string oldPhase = mClassroomState->GetGlobal().phase;
	int oldTurn = mClassroomState->GetGlobal().turn;

	//if currently it is unpaused and we want to pause it, we just do it
	if(!oldPause && paused){
		global_class newGlobal;
		newGlobal.paused = paused;
//		std::string newPauser(mDeviceState->GetMeteorId());//TODO: shouldn-t we be getting the device mutex also? this value doesn-t actually change over the execution, but...
//		newGlobal.pauserDevice = newPauser;
//		newGlobal.masterHint = oldMasterHint;
		newGlobal.phase = oldPhase;
		newGlobal.turn = oldTurn;
		mClassroomState->SetGlobal(newGlobal);
		mClassroomState->SetHasChanged(true);
	}
	//if currently it is paused and we want to unpause it, We only change pause if the pauserDevice matches the one that paused the class
	//TODO: shouldn-t we be getting the device mutex also? this value doesn-t actually change over the execution, but...
//	else if(oldPause && !paused && oldPauserDevice.compare(mDeviceState->GetMeteorId())==0){

	//We do not check the pauserdevice
	else if(oldPause && !paused){
		global_class newGlobal;
		newGlobal.paused = paused;
//		newGlobal.pauserDevice = "";
//		newGlobal.masterHint = oldMasterHint;
		newGlobal.phase = oldPhase;
		newGlobal.turn = oldTurn;
		mClassroomState->SetGlobal(newGlobal);
		mClassroomState->SetHasChanged(true);
	}
	//else, we do nothing

	pthread_mutex_unlock(&classstate_mutex);
}


//void NetworkedStateManager::SetMasterHint(std::string pHint){
//
//	pthread_mutex_lock(&classstate_mutex);
//	std::string oldHint = mClassroomState->GetGlobal().masterHint;
//	bool oldPaused = mClassroomState->GetGlobal().paused;
//	std::string oldPauserDevice = mClassroomState->GetGlobal().pauserDevice;
//
//	//if the current hint is different from the one passed, we change it
//	if(oldHint.compare(pHint)!=0){
//		global_class newGlobal;
//		newGlobal.paused = oldPaused;
//		newGlobal.pauserDevice = oldPauserDevice;
//		newGlobal.masterHint = pHint;
//		mClassroomState->SetGlobal(newGlobal);
//		mClassroomState->SetHasChanged(true);
//	}
//
//	//else, we do nothing
//
//	pthread_mutex_unlock(&classstate_mutex);
//}

void NetworkedStateManager::SetPhase(std::string pPhase){

	pthread_mutex_lock(&classstate_mutex);
	std::string oldPhase = mClassroomState->GetGlobal().phase;
	bool oldPaused = mClassroomState->GetGlobal().paused;
	int oldTurn = mClassroomState->GetGlobal().turn;

	//if the current hint is different from the one passed, we change it
	if(oldPhase.compare(pPhase)!=0){
		global_class newGlobal;
		newGlobal.paused = oldPaused;
		newGlobal.phase = pPhase;
		newGlobal.turn = oldTurn;
		mClassroomState->SetGlobal(newGlobal);
		mClassroomState->SetHasChanged(true);
	}

	//else, we do nothing

	pthread_mutex_unlock(&classstate_mutex);
}

void NetworkedStateManager::SetTurn(int pTurn){

	pthread_mutex_lock(&classstate_mutex);
	std::string oldPhase = mClassroomState->GetGlobal().phase;
	bool oldPaused = mClassroomState->GetGlobal().paused;
	int oldTurn = mClassroomState->GetGlobal().turn;

	//if the current hint is different from the one passed, we change it
	if(oldTurn != pTurn){
		global_class newGlobal;
		newGlobal.paused = oldPaused;
		newGlobal.phase = oldPhase;
		newGlobal.turn = pTurn;
		mClassroomState->SetGlobal(newGlobal);
		mClassroomState->SetHasChanged(true);
	}

	//else, we do nothing

	pthread_mutex_unlock(&classstate_mutex);
}


//void NetworkedStateManager::SetActivityCompletedMaps(int pCompleted){
//	pthread_mutex_lock(&devstate_mutex);
//
//	int oldCompleted = mDeviceState->GetActivity().currentState.completedMaps;
//	if(pCompleted != oldCompleted){
//		state newState = mDeviceState->GetActivity().currentState;
//		newState.completedMaps=pCompleted;
//		activity_state newActivityState = mDeviceState->GetActivity();
//		newActivityState.currentState = newState;
//		mDeviceState->SetActivity(newActivityState);
//		mDeviceState->SetHasChanged(true);
//	}
//
//	pthread_mutex_unlock(&devstate_mutex);
//}

//void NetworkedStateManager::IncrementCompletedMaps(){
//	pthread_mutex_lock(&devstate_mutex);
//
//	int oldCompleted = mDeviceState->GetActivity().currentState.completedMaps;
//	state newState = mDeviceState->GetActivity().currentState;
//	newState.completedMaps=(oldCompleted+1);
//	activity_state newActivityState = mDeviceState->GetActivity();
//	newActivityState.currentState = newState;
//	mDeviceState->SetActivity(newActivityState);
//	mDeviceState->SetHasChanged(true);
//
//	pthread_mutex_unlock(&devstate_mutex);
//}


//void NetworkedStateManager::SetActivityHintPresent(std::string pHint){
//	pthread_mutex_lock(&devstate_mutex);
//
//	std::string oldHint = mDeviceState->GetActivity().currentState.hintPresent;
//	if(pHint.compare(oldHint) != 0){
//		state newState = mDeviceState->GetActivity().currentState;
//		newState.hintPresent=pHint;
//		activity_state newActivityState = mDeviceState->GetActivity();
//		newActivityState.currentState = newState;
//		mDeviceState->SetActivity(newActivityState);
//		mDeviceState->SetHasChanged(true);
//	}
//	pthread_mutex_unlock(&devstate_mutex);
//}

//void NetworkedStateManager::UnsetActivityHintPresent(std::string pHint){
//	pthread_mutex_lock(&devstate_mutex);
//
//	std::string oldHint = mDeviceState->GetActivity().currentState.hintPresent;
//	if(pHint.compare(oldHint) == 0){//It eliminates this hint, if it was the one present before
//		state newState = mDeviceState->GetActivity().currentState;
//		newState.hintPresent="";
//		activity_state newActivityState = mDeviceState->GetActivity();
//		newActivityState.currentState = newState;
//		mDeviceState->SetActivity(newActivityState);
//		mDeviceState->SetHasChanged(true);
//	}
//	pthread_mutex_unlock(&devstate_mutex);
//}

//void NetworkedStateManager::SetActivityStepsDone(int pSteps){
//	pthread_mutex_lock(&devstate_mutex);
//
//	int oldSteps = mDeviceState->GetActivity().currentState.stepsDone;
//	if(pSteps != oldSteps){
//		state newState = mDeviceState->GetActivity().currentState;
//		newState.stepsDone=pSteps;
//		activity_state newActivityState = mDeviceState->GetActivity();
//		newActivityState.currentState = newState;
//		mDeviceState->SetActivity(newActivityState);
//		mDeviceState->SetHasChanged(true);
//	}
//
//	pthread_mutex_unlock(&devstate_mutex);
//}
//
//void NetworkedStateManager::SetActivityStepsToGo(int pSteps){
//	pthread_mutex_lock(&devstate_mutex);
//
//	int oldSteps = mDeviceState->GetActivity().currentState.stepsToGo;
//	if(pSteps != oldSteps){
//		state newState = mDeviceState->GetActivity().currentState;
//		newState.stepsToGo=pSteps;
//		activity_state newActivityState = mDeviceState->GetActivity();
//		newActivityState.currentState = newState;
//		mDeviceState->SetActivity(newActivityState);
//		mDeviceState->SetHasChanged(true);
//	}
//
//	pthread_mutex_unlock(&devstate_mutex);
//}

//void NetworkedStateManager::SetActivityWrongMoves(int pWrong){
//	pthread_mutex_lock(&devstate_mutex);
//
//	int oldWrong = mDeviceState->GetActivity().currentState.stepsDone;
//	if(pWrong != oldWrong){
//		state newState = mDeviceState->GetActivity().currentState;
//		newState.wrongMoves=pWrong;
//		activity_state newActivityState = mDeviceState->GetActivity();
//		newActivityState.currentState = newState;
//		mDeviceState->SetActivity(newActivityState);
//		mDeviceState->SetHasChanged(true);
//	}
//
//	pthread_mutex_unlock(&devstate_mutex);
//}
//
//void NetworkedStateManager::IncrementWrongMoves(){
//	pthread_mutex_lock(&devstate_mutex);
//
//	int oldWrong = mDeviceState->GetActivity().currentState.stepsDone;
//	state newState = mDeviceState->GetActivity().currentState;
//	newState.wrongMoves=oldWrong+1;
//	activity_state newActivityState = mDeviceState->GetActivity();
//	newActivityState.currentState = newState;
//	mDeviceState->SetActivity(newActivityState);
//	mDeviceState->SetHasChanged(true);
//
//	pthread_mutex_unlock(&devstate_mutex);
//}


bool NetworkedStateManager::isClassroomPaused(){
	bool paused;
	pthread_mutex_lock(&classstate_mutex);
	paused = mClassroomState->GetGlobal().paused;
	pthread_mutex_unlock(&classstate_mutex);
	return paused;
}

//std::string NetworkedStateManager::getPauserDevice(){
//
//	pthread_mutex_lock(&classstate_mutex);
//	std::string device(mClassroomState->GetGlobal().pauserDevice);
//	pthread_mutex_unlock(&classstate_mutex);
//	return device;
//}

//std::string NetworkedStateManager::getMasterHint(){
//	pthread_mutex_lock(&classstate_mutex);
//	std::string hint(mClassroomState->GetGlobal().masterHint);
//	pthread_mutex_unlock(&classstate_mutex);
//	return hint;
//}
std::string NetworkedStateManager::getPhase(){
	pthread_mutex_lock(&classstate_mutex);
	std::string phase(mClassroomState->GetGlobal().phase);
	pthread_mutex_unlock(&classstate_mutex);
	return phase;
}

int NetworkedStateManager::getTurn(){
	pthread_mutex_lock(&classstate_mutex);
	int turn = mClassroomState->GetGlobal().turn;
	pthread_mutex_unlock(&classstate_mutex);
	return turn;
}


std::vector<move> NetworkedStateManager::getMoves(){
	pthread_mutex_lock(&devstate_mutex);
	std::vector<move> moves(mDeviceState->GetDevice().moves);
	pthread_mutex_unlock(&devstate_mutex);
	return moves;
}

std::vector<move> NetworkedStateManager::getCurrentMove(){
	pthread_mutex_lock(&devstate_mutex);
	std::vector<move> current(mDeviceState->GetDevice().currentMove);
	pthread_mutex_unlock(&devstate_mutex);
	return current;
}

//std::string NetworkedStateManager::getDeviceId(){
//	pthread_mutex_lock(&devstate_mutex);
//	std::string device(mDeviceState->GetMeteorId());
//	pthread_mutex_unlock(&devstate_mutex);
//	return device;
//}
