/*
 * PutPostRemoteJSONString.cpp
 *
 *  Created on: May 20, 2014
 *      Author: lprisan
 */

#include "PutPostRemoteJSONString.hpp"

PutPostRemoteJSONString::PutPostRemoteJSONString(
		std::string pUrl,
		Entity pEntity):
mUrl(pUrl),
mEntity(pEntity)
{

}


void PutPostRemoteJSONString::run(){

	if(mEntity == DEVICE){
//		if(stateManager->hasDeviceChanged()){//if some other thread changed the local device state, we transmit it to server
//
//			std::string jsonState = stateManager->getDeviceJSON();
//
//			std::cout << "Detected state change: " << jsonState << std::endl;
//
//			//do PUT of the JSON object to the server
//			bool success = false;
//			std::cout << "PUTting remote JSON: " << jsonState << "...";
//			success = putRemoteString(mUrl,jsonState);
//			if(success){
//				std::cout << "Success!" << std::endl;
//				stateManager->SetHasDeviceChanged(false);
//			}
//			else std::cout << "Failure!" << std::endl;
//
////			//do POST of the JSON object to the history log
////			if(success){//If we failed the PUT, there is no sense in storing into the history
////				jsonState = stateManager->getAlternateDeviceJSON();//In this case, instead of setting the meteor _id, we set the field deviceid
////				success = false;
////				std::cout << "POSTing remote JSON to history log: " << jsonState << "...";
////				success = postRemoteString(mUrlHistory,jsonState);
////				if(success){
////					std::cout << "Success!" << std::endl;
////				}
////				else std::cout << "Failure!" << std::endl;
////
////			}
//		}
	} else if(mEntity == CLASSROOM){
		if(stateManager->hasClassroomChanged()){//if some other thread changed the local classroom state, we transmit it to server

			std::string jsonState = stateManager->getClassroomJSON();

			std::cout << "Detected state change: " << jsonState << std::endl;

			//do PUT of the JSON object to the server
			bool success = false;
			std::cout << "PUTting remote JSON: " << jsonState << "...";
			success = putRemoteString(mUrl,jsonState);
			if(success){
				std::cout << "Success!" << std::endl;
				stateManager->SetHasClassroomChanged(false);
			}
			else std::cout << "Failure!" << std::endl;

//			//do POST of the JSON object to the history log
//			if(success){//If we failed the PUT, there is no sense in storing into the history
//				jsonState = stateManager->getAlternateClassroomJSON();//In this case, instead of setting the meteor _id, we set the field classroomid
//				success = false;
//				std::cout << "POSTing remote JSON to history log: " << jsonState << "...";
//				success = postRemoteString(mUrlHistory,jsonState);
//				if(success){
//					std::cout << "Success!" << std::endl;
//				}
//				else std::cout << "Failure!" << std::endl;
//
//			}

		}
	} else if(mEntity == SHOOT){
		if(stateManager->hasShootChanged()){//if some other thread changed the local classroom state, we transmit it to server

			std::string jsonState = stateManager->getShootJSON();

			std::cout << "Detected state change: " << jsonState << std::endl;

			//do PUT of the JSON object to the server
			bool success = false;
			std::cout << "PUTting remote JSON: " << jsonState << "...";
			success = putRemoteString(mUrl,jsonState);
			if(success){
				std::cout << "Success!" << std::endl;
				stateManager->SetHasShootChanged(false);
			}
			else std::cout << "Failure!" << std::endl;

//			//do POST of the JSON object to the history log
//			if(success){//If we failed the PUT, there is no sense in storing into the history
//				jsonState = stateManager->getAlternateClassroomJSON();//In this case, instead of setting the meteor _id, we set the field classroomid
//				success = false;
//				std::cout << "POSTing remote JSON to history log: " << jsonState << "...";
//				success = postRemoteString(mUrlHistory,jsonState);
//				if(success){
//					std::cout << "Success!" << std::endl;
//				}
//				else std::cout << "Failure!" << std::endl;
//
//			}

		}
	}

}



bool PutPostRemoteJSONString::putRemoteString(std::string pUrl, std::string data){

	CURLcode code(CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init();
	bool success = false;
	if (curl) {
		struct curl_slist *headers=NULL; /* init to NULL is important */
		//headers = curl_slist_append(headers, client_id_header);
	    headers = curl_slist_append(headers, "Content-Type: application/json");

   		if(CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1))
   				&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers))
   				&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, pUrl.c_str()))
   				&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"))
   				&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str()))){

   			code = curl_easy_perform(curl);
   		    if(code==CURLE_OK) success = true;

   		}

	    curl_slist_free_all(headers);
	    curl_easy_cleanup(curl);
	}
	return(success);
}

bool PutPostRemoteJSONString::postRemoteString(std::string pUrl, std::string data){

	CURLcode code(CURLE_FAILED_INIT);
	CURL* curl = curl_easy_init();
	bool success = false;
	if (curl) {
		struct curl_slist *headers=NULL; /* init to NULL is important */
		//headers = curl_slist_append(headers, client_id_header);
	    headers = curl_slist_append(headers, "Content-Type: application/json");

   		if(CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1))
   				&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers))
   				&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, pUrl.c_str()))
   				&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST"))
   				&& CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str()))){

   			code = curl_easy_perform(curl);
   		    if(code==CURLE_OK) success = true;

   		}

	    curl_slist_free_all(headers);
	    curl_easy_cleanup(curl);
	}
	return(success);
}

