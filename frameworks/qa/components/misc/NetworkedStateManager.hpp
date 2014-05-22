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

class NetworkedStateManager {
public:
	NetworkedStateManager();
	NetworkedStateManager(std::string pDeviceMeteorId, std::string pClassroomMeteorId);
	virtual ~NetworkedStateManager();

	bool hasDeviceChanged();
	bool hasClassroomChanged();

	std::string getDeviceJSON();

	void SetDeviceMeteorId(std::string pId);
	void SetClassroomMeteorId(std::string pId);

	void SetHasDeviceChanged(bool changed);
	void addMarkerToDeviceState(std::string tagName);
	void removeMarkerFromDeviceState(std::string tagName);
	void SetClassroomJSON(std::string jsonData);

protected:
	DeviceState* mDeviceState;
	pthread_mutex_t devstate_mutex;

	ClassroomState* mClassroomState;
	pthread_mutex_t classstate_mutex;

};



#endif /* NETWORKEDSTATEMANAGER_HPP_ */
