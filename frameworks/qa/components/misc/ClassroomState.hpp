/*
 * ClassroomState.hpp
 *
 *  Created on: May 14, 2014
 *      Author: lprisan
 */

#ifndef CLASSROOMSTATE_HPP_
#define CLASSROOMSTATE_HPP_


#include <string>
#include <vector>
#include <json/json.h>
#include "NetworkedState.hpp"


struct global_class{

	  bool paused;
};


class ClassroomState {
public:
	ClassroomState();
	virtual ~ClassroomState();



	std::string GetMeteorId() {return mMeteorId;}
    int GetId() {return mId;};
	std::string GetName() {return mName;};
	global_class GetGlobal() {return mGlobal;};
	std::vector<int> GetDevices() {return mDevices;};
	bool hasChanged() {return mChanged;}

	void SetMeteorId(std::string meteorId) {mMeteorId = meteorId;}
	void SetId(int id) {mId = id;}
	void SetName(std::string name) {mName = name;}
	void SetGlobal(global_class global) {mGlobal = global;}
	void SetDevices(std::vector<int> devices) {mDevices = devices;}
	void SetHasChanged(bool changed) {mChanged = changed;}

	bool equals(ClassroomState* other);
	void setJSON(std::string jsonstring);
	Json::Value getJSONObject(std::string jsonMessage);


private:
	std::string mMeteorId;

	int mId;

	std::string mName;

	global_class mGlobal;

	std::vector<int> mDevices;

	bool mChanged;

};

#endif /* CLASSROOMSTATE_HPP_ */
