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
		libconfig::Setting & tFlippersStrings = pSetting["flippers"];
		//libconfig::Setting & tRegroupersStrings = pSetting["regroupers"];
		Flipper **tFlippers = new Flipper *[tNumFlippers];
		//Regrouper **tFlippers = new Regrouper *[tNumFlippers];

		for (int i = 0; i < tNumFlippers; i++) {
			tFlippers[i] = (Flipper *)pDecoratorManager.loadDecorator(tFlippersStrings[i]);
			//tFlippers[i] = (Regrouper *)pDecoratorManager.loadDecorator(tRegroupersStrings[i]);
		}
		return new decorators::RegroupDigits(pDecoratorManager,tFlippers,tNumFlippers);
				//return new decorators::RegroupDigits(pDecoratorManager,tFlippers);

	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::RegroupDigits::RegroupDigits(DecoratorManager &pDecoratorManager, Flipper ** pFlippers, int pFlipperNumber):
		FiducialDecorator(pDecoratorManager,0),
		mLastShotUnits2Tens (Time::MillisTimestamp()),
		mLastShotTens2Cents (Time::MillisTimestamp()),
		mFlippers(pFlippers),
		mIsAnimationStarted(false),
		mFlipperNumber(pFlipperNumber)
		{
		}

decorators::RegroupDigits::~RegroupDigits(){
	//delete [] mRegroupers;
	delete [] mFlippers;
}

void decorators::RegroupDigits::update(){
	//mLastShotUnits2Tens = Time::MillisTimestamp();
}

void decorators::RegroupDigits::UpdateLastShot(int pType){
	switch(pType){
	case 0:
		mLastShotUnits2Tens = Time::MillisTimestamp();
		break;
	case 1:
		mLastShotTens2Cents = Time::MillisTimestamp();
	}
}

bool decorators::RegroupDigits::RegroupAnimation(int pType, bool pIsNecessary, bool pAreCardsFromStacked, bool pAreCardsToStacked){

	Flipper *mFlipper = (pType == 0) ? mFlippers[0] : mFlippers[1];
	bool tFull;
	static const float scX1Bar = -1.0f;
	static const float scX2Bar = 4.5f;
	static const float scY1Bar = 6.5f;
	static const float scY2Bar = 7.0f;

	long tElapsedTime = Time::MillisTimestamp() - mLastShotUnits2Tens;

	if (mFlipper->IsFlipped() && tElapsedTime > cShotPreparationTime && mFlipper->IsSecondSideUp()) {
		mLastShotUnits2Tens = Time::MillisTimestamp();
		mIsAnimationStarted = false;
	}

	if (mFlipper->IsPresent())
	{
		if(tElapsedTime > cShotPreparationTime && mFlipper->IsSecondSideUp() && mIsAnimationStarted){
			tFull = true;

			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(*mFlipper->GetCurrentSide(), mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
			mDecoratorManager.GetDisplay().RenderQuadFilled(scX1Bar, scY1Bar,
						scX2Bar, scY1Bar,
						scX2Bar, scY2Bar,
						scX1Bar, scY2Bar,
						tFull?0.0f:1.0f, 1.0f, 0.0f);

			mDecoratorManager.GetDisplay().RenderText(tFull?"Prêt!":"En charge...", tFull?0.5f:-1.0f, 6.0f, 0.04f);
			mDecoratorManager.GetDisplay().PopTransformation();
			return tFull;
		}
		if(mFlipper->IsSecondSideUp() && tElapsedTime <= cShotPreparationTime){

			float tPartialX2 = scX1Bar+(scX2Bar-scX1Bar)*(tElapsedTime/(float)cShotPreparationTime);
			float tPartialY2 = scY1Bar+(scY2Bar-scY1Bar)*(tElapsedTime/(float)cShotPreparationTime);
			tFull = false;

			if (tPartialX2 >= scX2Bar){
				tPartialX2 = scX2Bar;
				tFull = true;
			}

			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(*mFlipper->GetCurrentSide(), mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
			mDecoratorManager.GetDisplay().RenderQuadFilled(scX1Bar, scY1Bar,
						tPartialX2, scY1Bar,
						tPartialX2, scY2Bar,
						scX1Bar, scY2Bar,
						tFull?0.0f:1.0f, 1.0f, 0.0f);

			mDecoratorManager.GetDisplay().RenderText(tFull?"Prêt!":"En charge...", tFull?0.5f:-1.0f, 6.0f, 0.04f);
			mDecoratorManager.GetDisplay().PopTransformation();

			mIsAnimationStarted = true;

			return tFull;
		}
	}
	return tFull;
}

bool decorators::RegroupDigits::IsGrouperPresent(){
	return (IsGrouperPresent(0) || IsGrouperPresent(1));
}

bool decorators::RegroupDigits::IsGrouperPresent(int pType){
	return (mFlippers[pType]->IsPresent());
}

