/*
 * CURLHelper.cpp
 *
 *  Created on: Feb 18, 2014
 *      Author: lprisan
 */

#include "CURLHelper.hpp"

CURLHelper::CURLHelper () {
      curl = curl_easy_init();
      if(!curl)
        throw std::string ("Curl did not initialize!");

}

void CURLHelper::makeRequest(std::string pUrl) {

	if(!curl)
	  throw std::string ("Curl did not initialize!");

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &CURLHelper::curlWriter);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlBuffer);
	curl_easy_setopt(curl, CURLOPT_URL, pUrl.c_str());
	curl_easy_perform(curl);


}

void CURLHelper::cleanup(){

	// Clean up
	curl_easy_cleanup(curl);
	curl_global_cleanup();

}

std::string CURLHelper::urlEncode (std::string pString){

	char * encodedChars;
	encodedChars = curl_easy_escape( curl , pString.c_str() , 0 );
	std::string encodedString(encodedChars);
	return encodedString;

}
