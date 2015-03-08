/*
 * ShootState.cpp
 *
 *  Created on: May 13, 2014
 *      Author: lprisan
 */

#include "ShootState.hpp"

ShootState::ShootState() {
	mChanged = false;
}

ShootState::~ShootState() {
	// TODO Auto-generated destructor stub
}

bool ShootState::equals(ShootState* other){

	return equalShoots(this->mShoot, other->GetShoot());

}

bool ShootState::equalShoots(shoot tShoot1, shoot tShoot2){

	if(tShoot1.rotation!=tShoot2.rotation) return false;
	else if(tShoot1.team_id.compare(tShoot2.team_id)!=0) return false;
	else if(tShoot1.translation.x!=tShoot2.translation.x || tShoot1.translation.y!=tShoot2.translation.y) return false;
	else if(tShoot1.polygon.size()!=tShoot2.polygon.size()) return false;
	else{
		for(int i=0; i<tShoot1.polygon.size(); i++){
			if(tShoot1.polygon[i].x!=tShoot2.polygon[i].x || tShoot1.polygon[i].y!=tShoot2.polygon[i].y) return false;
		}
	}


	//If all else failed, they must be equal!
	return true;

}


std::string ShootState::getJSON(){

	std::string data;

	Json::Value json;

	json[scTeamIdLabel] = this->mShoot.team_id;
	json[scRotationLabel] = this->mShoot.rotation;

	Json::Value translation = Json::Value(Json::arrayValue);
	translation.append(this->mShoot.translation.x);
	translation.append(this->mShoot.translation.y);
	json[scTranslationLabel] = translation;


	Json::Value polygon = Json::Value(Json::arrayValue);
	for(unsigned int j=0; j<this->mShoot.polygon.size(); j++){//For each polygon vertex
		Json::Value vertex = Json::Value(Json::arrayValue);
		vertex.append(this->mShoot.polygon[j].x);
		vertex.append(this->mShoot.polygon[j].y);
		polygon.append(vertex);
	}
	json[scPolygonLabel] = polygon;

	Json::FastWriter fastWriter;
	data = fastWriter.write(json);

	std::cout << "Built JSON object: " << data << std::endl;

	return data;
}

Json::Value ShootState::getJSONObject(std::string jsonMessage){

	Json::Value parsedFromString;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse(jsonMessage, parsedFromString);
	if (parsingSuccessful)
	{
		//Json::StyledWriter styledWriter;
		//std::cout << styledWriter.write(parsedFromString) << std::endl;
		//std::cout << "Parsing successful!" << std::endl;
		//std::cout << jsonMessage << std::endl;
		return(parsedFromString);
	}
	return(parsedFromString);
}

void ShootState::setJSON(std::string jsonstring){

	Json::Value value = getJSONObject(jsonstring);

	shoot tShoot;

	tShoot.team_id = value[scTeamIdLabel].asString();

	Json::Value jsonpolygon = value[scPolygonLabel];
	wykobi::polygon<float, 2> polygon;
	for(unsigned j=0; j<jsonpolygon.size(); j++){
		polygon.push_back(wykobi::make_point(jsonpolygon[j][0].asFloat(),jsonpolygon[j][1].asFloat()));
	}
	tShoot.polygon = polygon;

	tShoot.rotation = value[scRotationLabel].asInt();

	tShoot.translation = wykobi::make_point(value[scTranslationLabel][0].asFloat(),value[scTranslationLabel][1].asFloat());


	this->mShoot = tShoot;
	this->mChanged = false;//The changed value means it has NOT changed LOCALLY by another thread (rather, it comes from the server)


}
