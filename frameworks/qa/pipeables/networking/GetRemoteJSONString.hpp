/*
 * GetRemoteJSONString.hpp
 *
 *  Created on: May 20, 2014
 *      Author: lprisan
 */

#ifndef GETREMOTEJSONSTRING_HPP_
#define GETREMOTEJSONSTRING_HPP_

#include <string>
#include <iostream>
#include <curl/curl.h>
#include <qa/pipeables/misc/DecoratorManager.hpp>

class GetRemoteJSONString: public Pipeable {
	    public:
			GetRemoteJSONString(std::string pUrl, Entity pEntity = DEVICE);

	    protected:
	        void run();

	        std::string getRemoteString(std::string pUrl);
	        CURLcode curl_read(const std::string& url, std::ostream& os, long timeout = 30);
	        static size_t data_write(void* buf, size_t size, size_t nmemb, void* userp);

	        std::string mUrl;

	        Entity mEntity;//This marks the mode of operation of the pipeable (translate what kind of object into JSON)

};

#endif /* GETREMOTEJSONSTRING_HPP_ */
