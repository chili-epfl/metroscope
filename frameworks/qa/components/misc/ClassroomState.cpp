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

bool ClassroomState::equals(ClassroomState other){

	if(this->mId != other.GetId()) return false;
	if(this->mMeteorId != other.GetMeteorId()) return false;
	if(this->mName.compare(other.GetName())!=0) return false;
	if(this->mDevices.size() != other.GetDevices().size()) return false;
	else{//we check all vector elements for present tags
		for(int i=0;i<this->mDevices.size();i++){
			if(this->mDevices.at(i)!=other.GetDevices().at(i)){
				return false;
			}
		}
	}
	//check the global class variables state
	if(this->mGlobal.paused!=other.GetGlobal().paused) return false;

	//If all else failed, they must be equal!
	return true;
}
