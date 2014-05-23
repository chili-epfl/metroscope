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
	  std::string pauserDevice;
};


class ClassroomState {
public:
	ClassroomState();
	virtual ~ClassroomState();



	std::string GetMeteorId() {return mMeteorId;}
    //int GetId() {return mId;};
	std::string GetName() {return mName;};
	global_class GetGlobal() {return mGlobal;};
	std::vector<std::string> GetDevices() {return mDevices;};
	bool hasChanged() {return mChanged;}

	void SetMeteorId(std::string meteorId) {mMeteorId = meteorId;}
	//void SetId(int id) {mId = id;}
	void SetName(std::string name) {mName = name;}
	void SetGlobal(global_class global) {mGlobal = global;}
	void SetDevices(std::vector<std::string> devices) {mDevices = devices;}
	void SetHasChanged(bool changed) {mChanged = changed;}

	bool equals(ClassroomState* other);
	std::string getJSON();

	void setJSON(std::string jsonstring);
	Json::Value getJSONObject(std::string jsonMessage);


private:
	std::string mMeteorId;

	//int mId;

	std::string mName;

	global_class mGlobal;

	std::vector<std::string> mDevices;

	bool mChanged;

};

#endif /* CLASSROOMSTATE_HPP_ */
