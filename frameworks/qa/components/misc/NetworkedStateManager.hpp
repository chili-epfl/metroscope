/*
 * NetworkedStateManager.h
 *
 *  Created on: May 22, 2014
 *      Author: lprisan
 */

#ifndef NETWORKEDSTATEMANAGER_HPP_
#define NETWORKEDSTATEMANAGER_HPP_

#include <qa/components/misc/DeviceState.hpp>
#include <qa/components/misc/ClassroomState.hpp>
#include <pthread.h>

enum Entity{CLASSROOM, DEVICE};


class NetworkedStateManager {
public:
	NetworkedStateManager();
	NetworkedStateManager(std::string pDeviceMeteorId, std::string pClassroomMeteorId);
	virtual ~NetworkedStateManager();

	bool hasDeviceChanged();
	bool hasClassroomChanged();

	std::string getDeviceJSON();//gets a JSON string with the current state of the device
	std::string getClassroomJSON();//gets a JSON string with the current state of the classroom

	std::string getPauserDevice();
	std::string getDeviceId();

	void SetDeviceMeteorId(std::string pId);
	void SetClassroomMeteorId(std::string pId);

	void SetHasDeviceChanged(bool changed);
	void SetHasClassroomChanged(bool changed);

	void SetClassroomPaused(bool changed);
	bool isClassroomPaused();

	void addMarkerToDeviceState(std::string tagName);
	void removeMarkerFromDeviceState(std::string tagName);

	void SetClassroomJSON(std::string jsonData);
	void SetDeviceJSON(std::string jsonData);

protected:
	DeviceState* mDeviceState;
	pthread_mutex_t devstate_mutex;

	ClassroomState* mClassroomState;
	pthread_mutex_t classstate_mutex;

};

extern NetworkedStateManager* stateManager;



#endif /* NETWORKEDSTATEMANAGER_HPP_ */
