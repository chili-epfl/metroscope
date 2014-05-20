/*
 * PutRemoteJSON.hpp
 *
 *  Created on: May 20, 2014
 *      Author: lprisan
 */

#ifndef PUTREMOTEJSON_HPP_
#define PUTREMOTEJSON_HPP_

#include <string>
#include <iostream>
#include <json/json.h>
#include <curl/curl.h>
#include <qa/components/misc/DeviceState.hpp>

class PutRemoteJSON: public Pipeable {
	    public:
			PutRemoteJSON(std::string pUrl, const NetworkedState *pState);

	    protected:
	        void run();

	        std::string getJSONFromDevice(DeviceState state);
	        bool putRemoteString(std::string pUrl, std::string data);

	        std::string mUrl;
	        NetworkedState *mState;

};

#endif /* PUTREMOTEJSON_HPP_ */
