/*
 * PutRemoteJSON.hpp
 *
 *  Created on: May 20, 2014
 *      Author: lprisan
 */

#ifndef PUTREMOTEJSONSTRING_HPP_
#define PUTREMOTEJSONSTRING_HPP_

#include <string>
#include <iostream>
#include <curl/curl.h>
#include <qa/pipeables/Pipeable.hpp>
#include <qa/components/misc/NetworkedStateManager.hpp>

class PutRemoteJSONString: public Pipeable {
	    public:
			PutRemoteJSONString(std::string pUrl, Entity pEntity = DEVICE);

	    protected:
	        void run();

	        bool putRemoteString(std::string pUrl, std::string data);

	        std::string mUrl;

	        Entity mEntity;//This marks the mode of operation of the pipeable (translate what kind of object into JSON)

};

#endif /* PUTREMOTEJSONSTRING_HPP_ */
