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
#include <qa/pipeables/misc/DecoratorManager.hpp>

class PutRemoteJSONString: public Pipeable {
	    public:
			PutRemoteJSONString(std::string pUrl);

	    protected:
	        void run();

	        bool putRemoteString(std::string pUrl, std::string data);

	        std::string mUrl;

};

#endif /* PUTREMOTEJSONSTRING_HPP_ */
