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
		int tNumFlippers = pSetting["flippers_number"];
		libconfig::Setting & tFlippersStrings = pSetting["flippers"];
		Flipper **tFlippers = new Flipper *[tNumFlippers];

		for (int i = 0; i < tNumFlippers; i++) {
			tFlippers[i] = (Flipper *)pDecoratorManager.loadDecorator(tFlippersStrings[i]);
		}

				return new decorators::RegroupDigits(pDecoratorManager,tFlippers,tNumFlippers);

	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::RegroupDigits::RegroupDigits(DecoratorManager &pDecoratorManager, Flipper ** pFlippers, int pFlipperNumber):
		FiducialDecorator(pDecoratorManager,0),
		mFlippers(pFlippers),
		mLastShotUnits2Tens (Time::MillisTimestamp()),
		mLastShotTens2Cents (Time::MillisTimestamp()),
		mFlipperNumber(pFlipperNumber)
		{
		}

decorators::RegroupDigits::~RegroupDigits(){
	delete [] mFlippers;
}

void decorators::RegroupDigits::update(){

}

void decorators::RegroupDigits::Regroup(int pType, int pDigit, float pOriginPoint_x, float pOriginPoint_y, float pEndPoint_x, float pEndPoint_y){
	int mDigitToArrange = (pType == 0) ?  pDigit*10 : pDigit*100;

	Flipper *mFlipper = (pType == 0) ? mFlippers[0] : mFlippers[1];
	long mLastShot = (pType == 0) ? mLastShotUnits2Tens : mLastShotTens2Cents;



	static const float scX1 = pOriginPoint_x;
	static const float scX2 = pEndPoint_x;
	static const float scY1 = pOriginPoint_y;
	static const float scY2 = pEndPoint_y;


	static const long cShotPreparationTime = 10l*800l;
	long tElapsedTime = Time::MillisTimestamp() - mLastShot;

	if (mFlipper->IsFlipped() && tElapsedTime > cShotPreparationTime && mFlipper->IsSecondSideUp()) {
		mLastShot = Time::MillisTimestamp();
	}

	if (mFlipper->IsPresent())
			{
			if(mFlipper->IsSecondSideUp()){
				float tPartialX2 = scX1+(scX2-scX1)*(tElapsedTime/(float)cShotPreparationTime);
				float tPartialY2 = scY1+(scY2-scY1)*(tElapsedTime/(float)cShotPreparationTime);
								bool tFull = false;
								if (tPartialX2 >= scX2 && tPartialY2 >= scY2)
								{
									tPartialX2 = scX2;
									tPartialY2 = scY2;
									tFull = true;
								}

								mDecoratorManager.GetDisplay().PushTransformation();
								//mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(*mFlipper->GetCurrentSide(), mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
								//mDecoratorManager.GetDisplay().RenderQuadFilled(
										//scX1, scY1,
										//tPartialX2, tPartialY2,
										//tPartialX2, tPartialY2,
										//scX1, scY2,
										//tFull?0.0f:1.0f, 1.0f, 0.0f);
								mDecoratorManager.GetDisplay().RenderText("  *",tPartialX2,tPartialY2,2.5f,125.0f,125.0f,150.0f,1.0f);
								//mDecoratorManager.GetDisplay().RenderText(tFull?"Prêt!":"En charge...", tFull?0.5f:-1.0f, 6.0f, 0.04f);
								mDecoratorManager.GetDisplay().PopTransformation();
			}

			}

}
