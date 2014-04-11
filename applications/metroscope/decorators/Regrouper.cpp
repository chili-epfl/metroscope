/*
 * Regrouper.cpp
 *
 *  Created on: Apr 11, 2014
 *      Author: daniela
 */

#include "Regrouper.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>

const std::string decorators::Regrouper::scDecoratorName("Regrouper");
const  DecoratorManager::Registerer decorators::Regrouper::mRegisterer(decorators::Regrouper::scDecoratorName, &decorators::Regrouper::create);

decorators::FiducialDecorator *decorators::Regrouper::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager){
	try{
	return new decorators::Regrouper(pDecoratorManager,
									pDecoratorManager.loadMarker(pSetting["marker"]),
									pSetting["regrouper_type"]);
	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;

}

decorators::Regrouper::Regrouper(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, int pType):
		FiducialDecorator(pDecoratorManager,pMarker),
		mType(pType),
		mLastPresenceTimestamp(0)
		{
		}

decorators::Regrouper::~Regrouper(){
}

void decorators::Regrouper::update(){
	if(mMarker->isPresent()){
		mLastPresenceTimestamp = Time::MillisTimestamp();
	}
}

