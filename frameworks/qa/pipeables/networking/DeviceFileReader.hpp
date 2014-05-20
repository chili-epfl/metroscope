/*
 * DeviceFileReader.hpp
 *
 *  Created on: May 20, 2014
 *      Author: lprisan
 */

#ifndef DEVICEFILEREADER_HPP_
#define DEVICEFILEREADER_HPP_

#include <string>
#include <fstream>
#include <sstream>
#include <json/json.h>
#include <qa/components/misc/DeviceState.hpp>

class DeviceFileReader: public Pipeable {
	    public:
			DeviceFileReader(std::string file, DeviceState *pState);
			~DeviceFileReader();

	    protected:
	        void run();

	        std::string get_file_contents(const char *filename);
	        DeviceState getDeviceFromJSON(Json::Value value);
	        Json::Value getJSON(std::string jsonMessage);

	        std::string mFile;
	        DeviceState *mState;

};

#endif /* DEVICEFILEREADER_HPP_ */
