/*
 * RegroupDigits.cpp
 *
 *  Created on: Apr 1, 2014
 *      Author: daniela
 */

#include "RegroupDigits.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>
#include <wykobi/wykobi_utilities.hpp>

const std::string decorators::RegroupDigits::scDecoratorName("RegroupDigits");
const DecoratorManager::Registerer decorators::RegroupDigits::mRegisterer(decorators::RegroupDigits::scDecoratorName, &decorators::RegroupDigits::create);

decorators::FiducialDecorator *decorators::RegroupDigits::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager){

	try{
		int tNumFlippers = pSetting["regroupers_number"];
		libconfig::Setting & tRegroupersStrings = pSetting["regroupers"];
		Regrouper **tFlippers = new Regrouper *[tNumFlippers];

		for (int i = 0; i < tNumFlippers; i++) {
			tFlippers[i] = (Regrouper *)pDecoratorManager.loadDecorator(tRegroupersStrings[i]);
		}
				return new decorators::RegroupDigits(pDecoratorManager,tFlippers);

	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::RegroupDigits::RegroupDigits(DecoratorManager &pDecoratorManager, Regrouper ** pRegroupers):
		FiducialDecorator(pDecoratorManager,0),
		mRegroupers(pRegroupers),
		mLastShotUnits2Tens (Time::MillisTimestamp()),
		mLastShotTens2Cents (Time::MillisTimestamp())
		{
		}

decorators::RegroupDigits::~RegroupDigits(){
	delete [] mRegroupers;
}

void decorators::RegroupDigits::update(){

}



bool decorators::RegroupDigits::Regroup(int pType, int pDigit, float pOriginPoint_x, float pOriginPoint_y, float pEndPoint_x, float pEndPoint_y, bool pIsNecessary){
	int mDigitToArrange = (pType == 0) ?  pDigit*10 : pDigit*100;
	bool tFull;
	Regrouper *mRegrouper = (pType == 0) ? mRegroupers[0] : mRegroupers[1];
	long mLastShot = (pType == 0) ? mLastShotUnits2Tens : mLastShotTens2Cents;



	static const float scX1 = -1.0f;
	static const float scX2 = 4.5f;
	static const float scY1 = 6.5f;
	static const float scY2 = 7.0f;


	static const long cShotPreparationTime = 10l*800l;
	long tElapsedTime = Time::MillisTimestamp() - mLastShot;

	if (mRegrouper->IsPresent() && tElapsedTime > cShotPreparationTime) {
		mLastShot = Time::MillisTimestamp();
	}

	if (mRegrouper->IsPresent())
	{
		float tPartialX2 = scX1+(scX2-scX1)*(tElapsedTime/(float)cShotPreparationTime);
		tFull= false;

		if (tPartialX2 >= scX2){
			tPartialX2 = scX2;
			tFull = true;
		}

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates((mRegrouper->getMarker()), mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
		//si esta lleno y es true entonces q muestre un verde
		//si esta lleno y es false entonces q muestre rojo
		//si no esta lleno, entonces q muestre gris
		float tR = 0.0f;
		float tG = 0.0f;
		float tB = 0.0f;

		if(tFull){
			tR = pIsNecessary ? 0.0f : 255.0f;
			tG = pIsNecessary ? 153.0f : 0.0f;
		}else{
			tR = 96.0f;
			tG = 96.0f;
			tB = 96.0f;
		}
		mDecoratorManager.GetDisplay().RenderQuadFilled(scX1, scY1,tPartialX2, scY1,tPartialX2, scY2,scX1, scY2,tR,tG,tB);
		mDecoratorManager.GetDisplay().RenderText(tFull?" ":"Checking ", tFull?0.5f:-1.0f, 6.0f, 0.04f);
		mDecoratorManager.GetDisplay().PopTransformation();
	}

	return(tFull);

}

bool decorators::RegroupDigits::IsGrouperPresent(){
	return (IsGrouperPresent(0) || IsGrouperPresent(1));
}

bool decorators::RegroupDigits::IsGrouperPresent(int pType){
	return (mRegroupers[pType]->IsPresent());
}
