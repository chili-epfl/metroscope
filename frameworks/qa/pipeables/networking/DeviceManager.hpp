/*
 * DeviceManager.hpp
 *
 *  Created on: May 20, 2014
 *      Author: lprisan
 */

#ifndef DEVICEMANAGER_HPP_
#define DEVICEMANAGER_HPP_

#include <qa/components/misc/DeviceState.hpp>

class DeviceManager: public Pipeable {
	    public:
	        DeviceManager():mDevice() {}
	        const DeviceState *GetDevice(){ return &mDevice; }
	    protected:
	        void run() { }
	        DeviceState mDevice;
};



#endif /* DEVICEMANAGER_HPP_ */
