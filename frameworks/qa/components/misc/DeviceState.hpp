/*
 * DeviceState.hpp
 *
 *  Created on: May 13, 2014
 *      Author: lprisan
 */

#ifndef DEVICESTATE_HPP_
#define DEVICESTATE_HPP_

#include <string>
#include <vector>
#include <json/json.h>
#include <wykobi/wykobi_utilities.hpp>
#include "NetworkedState.hpp"



struct move{

	int activity_id;
	int turn;
	int team;
	wykobi::point2d<float> origin;
	wykobi::polygon<float, 2> polygon;
	int rotation;
	wykobi::point2d<float> translation;
	bool illegal;
	std::string meteorId;

};


struct device_state {
  std::vector<move> currentMove;
  std::vector<move> moves;
};


class DeviceState {
public:
	DeviceState();
	virtual ~DeviceState();

	device_state GetDevice() {return mDevice;};
	bool hasChanged() {return mChanged;}

	void SetDevice(device_state device) {mDevice = device;}
	void SetHasChanged(bool changed) {mChanged = changed;}

	bool equals(DeviceState* other);
	bool equalMoves(move move1, move move2);

	std::string getJSON();

	void setJSON(std::string jsonstring, int pTurn);
	Json::Value getJSONObject(std::string jsonMessage);

private:

	device_state mDevice;

	bool mChanged;


};

#endif /* DEVICESTATE_HPP_ */
