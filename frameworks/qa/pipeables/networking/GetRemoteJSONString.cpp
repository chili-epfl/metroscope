/*
 * GetRemoteJSONString.cpp
 *
 *  Created on: May 20, 2014
 *      Author: lprisan
 */

#include "GetRemoteJSONString.hpp"

GetRemoteJSONString::GetRemoteJSONString(
		std::string pUrl,
		Entity pEntity):
mUrl(pUrl),
mEntity(pEntity)
{

}


void GetRemoteJSONString::run(){

	if(mEntity == DEVICE){
		if(!stateManager->hasDeviceChanged()){//if no other thread changed the local device state, we get it from server
			std::cout << "GETting remote JSON from: " << mUrl << "...";
			std::string jsonData = getRemoteString(mUrl);
			if(jsonData.length()>0){
				std::cout << "Success!" << std::endl;
				//std::cout << "Received data: " << jsonData << std::endl;
				stateManager->SetDeviceJSON(jsonData);
			}
			else std::cout << "Failure!" << std::endl;
		}
	}else if(mEntity==CLASSROOM){
		if(!stateManager->hasClassroomChanged()){//if no other thread changed the local classroom state, we get it from server
			std::cout << "GETting remote JSON from: " << mUrl << "...";
			std::string jsonData = getRemoteString(mUrl);
			if(jsonData.length()>0){
				std::cout << "Success!" << std::endl;
				//std::cout << "Received data: " << jsonData << std::endl;
				stateManager->SetClassroomJSON(jsonData);
			}
			else std::cout << "Failure!" << std::endl;
		}
	}

}



std::string GetRemoteJSONString::getRemoteString(std::string pUrl){

	std::ostringstream oss;
	std::string jsonstring;
	if(CURLE_OK == curl_read(pUrl, oss))
	{
		// Web page successfully written to string
		jsonstring = oss.str();
	}


	return jsonstring;


}



// callback function writes data to a std::ostream
size_t GetRemoteJSONString::data_write(void* buf, size_t size, size_t nmemb, void* userp)
{
	if(userp)
	{
		std::ostream& os = *static_cast<std::ostream*>(userp);
		std::streamsize len = size * nmemb;
		if(os.write(static_cast<char*>(buf), len))
			return len;
	}

	return 0;
}

/**
 * timeout is in seconds
 **/
CURLcode GetRemoteJSONString::curl_read(const std::string& url, std::ostream& os, long timeout)
{
	CURLcode code(CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init();

	if(curl)
	{
		if(CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &os))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout))
		&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str())))
		{
			code = curl_easy_perform(curl);
		}
		curl_easy_cleanup(curl);
	}
	return code;
}

