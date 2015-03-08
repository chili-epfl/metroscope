/*
 * DeviceState.hpp
 *
 *  Created on: May 13, 2014
 *      Author: lprisan
 */

#ifndef SHOOTSTATE_HPP_
#define SHOOTSTATE_HPP_

#include <string>
#include <vector>
#include <json/json.h>
#include <wykobi/wykobi_utilities.hpp>
#include "NetworkedState.hpp"



struct shoot{

	std::string team_id;
	int rotation;
	wykobi::point2d<float> translation;
	wykobi::polygon<float, 2> polygon;

};


//struct shoot_state {
//  std::vector<move> currentMove;
//  std::vector<move> moves;
//};


class ShootState {
public:
	ShootState();
	virtual ~ShootState();

	shoot GetShoot() {return mShoot;};
	bool hasChanged() {return mChanged;}

	void SetShoot(shoot pShoot) {mShoot = pShoot;}
	void SetHasChanged(bool changed) {mChanged = changed;}

	bool equals(ShootState* other);
	bool equalShoots(shoot pShoot1, shoot pShoot2);

	std::string getJSON();

	void setJSON(std::string jsonstring);
	Json::Value getJSONObject(std::string jsonMessage);

private:

	shoot mShoot;

	bool mChanged;


};

#endif /* DEVICESTATE_HPP_ */
