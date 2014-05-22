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
#include "NetworkedState.hpp"



enum Representations {circular, rectangular, tokens, tangram};

struct state{


	  int numerator;
	  int denominator;
	  float value;
	  Representations representation;

  };

struct activity_state {
  int id;
  std::string name;
  state currentState;
};


class DeviceState {
public:
	DeviceState();
	virtual ~DeviceState();

	std::string GetMeteorId() {return mMeteorId;}
    int GetId() {return mId;};
	std::string GetName() {return mName;};
	activity_state GetActivity() {return mActivity;};
	std::vector<std::string> GetPresentTags() {return mPresentTags;};
	bool hasChanged() {return mChanged;}

	void SetMeteorId(std::string meteorId) {mMeteorId = meteorId;}
	void SetId(int id) {mId = id;}
	void SetName(std::string name) {mName = name;}
	void SetActivity(activity_state activity) {mActivity = activity;}
	void SetPresentTags(std::vector<std::string> presentTags) {mPresentTags = presentTags;}
	void SetHasChanged(bool changed) {mChanged = changed;}

	bool equals(DeviceState* other);
	std::string getJSON();

private:
	std::string mMeteorId;

	int mId;

	std::string mName;

	activity_state mActivity;

	std::vector<std::string> mPresentTags;

	bool mChanged;


};

#endif /* DEVICESTATE_HPP_ */
