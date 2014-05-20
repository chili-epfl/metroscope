/*
 * ClassroomState.hpp
 *
 *  Created on: May 14, 2014
 *      Author: lprisan
 */

#ifndef NETWORKEDSTATE_HPP_
#define NETWORKEDSTATE_HPP_

//Labels are for the JSON serialization
	  static const std::string scNumeratorLabel = "numerator";
	  static const std::string scDenominatorLabel = "denominator";
	  static const std::string scValueLabel = "value";
	  static const std::string scRepresentationLabel = "representation";
	  static const std::string scIdLabel = "id";
	  static const std::string scNameLabel = "name";
	  static const std::string scStateLabel = "state";
		static const std::string scMeteorIdLabel="_id";
		static const std::string scCurrentLabel="current";
		static const std::string scActivityLabel="activity";
		static const std::string scPresentTagsLabel="presentTags";
		static const std::string scDevicesLabel="devices";
		static const std::string scGlobalClassVariables="global";
		static const std::string scPaused="paused";



class NetworkedState
{
    public:
		NetworkedState():mChanged(false){}
		virtual ~NetworkedState(){}

		bool hasChanged() {return mChanged;}

		void SetHasChanged(bool changed) {mChanged = changed;}

		//A method children classes have to implement, to check for equality/changes in the state data
		virtual bool equals(NetworkedState other){return(false);}


    protected:
		bool mChanged;


};
#endif
