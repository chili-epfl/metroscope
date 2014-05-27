/*
 * PutPostRemoteJSON.hpp
 *
 *  Created on: May 20, 2014
 *      Author: lprisan
 */

#ifndef PUTPOSTREMOTEJSONSTRING_HPP_
#define PUTPOSTREMOTEJSONSTRING_HPP_

#include <string>
#include <iostream>
#include <curl/curl.h>
#include <qa/pipeables/misc/DecoratorManager.hpp>

class PutPostRemoteJSONString: public Pipeable {
	    public:
			PutPostRemoteJSONString(std::string pUrl, std::string pUrlHistory, Entity pEntity = DEVICE);

	    protected:
	        void run();

	        bool putRemoteString(std::string pUrl, std::string data);

	        bool postRemoteString(std::string pUrl, std::string data);

	        std::string mUrl;

	        std::string mUrlHistory;

	        Entity mEntity;//This marks the mode of operation of the pipeable (translate what kind of object into JSON)

};

#endif /* PUTPOSTREMOTEJSONSTRING_HPP_ */
