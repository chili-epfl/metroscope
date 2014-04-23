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

//decorators::RegroupDigits::RegroupDigits(DecoratorManager &pDecoratorManager, Regrouper ** pRegroupers):
decorators::RegroupDigits::RegroupDigits(DecoratorManager &pDecoratorManager, Flipper ** pFlippers, int pFlipperNumber):
		FiducialDecorator(pDecoratorManager,0),
		mLastShotUnits2Tens (Time::MillisTimestamp()),
		mLastShotTens2Cents (Time::MillisTimestamp()),
		mFlippers(pFlippers),
		mIsAnimationStarted(false)
		//mRegroupers(pRegroupers)
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

	//Regrouper *mRegrouper = (pType == 0) ? mRegroupers[0] : mRegroupers[1];

	Flipper *mFlipper = (pType == 0) ? mFlippers[0] : mFlippers[1];
	bool tFull;
	static const float scX1Bar = -1.0f;
	static const float scX2Bar = 4.5f;
	static const float scY1Bar = 6.5f;
	static const float scY2Bar = 7.0f;

	//static const long cShotPreparationTime = 10l*1000l;

	long tElapsedTime = Time::MillisTimestamp() - mLastShotUnits2Tens;

	if (mFlipper->IsFlipped() && tElapsedTime > cShotPreparationTime && mFlipper->IsSecondSideUp()) {
		mLastShotUnits2Tens = Time::MillisTimestamp();
		mIsAnimationStarted = false;
		//tFull = false;
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

								//mDecoratorManager.GetDisplay().RenderText("  *",tPartialX2,tPartialY2,2.5f,125.0f,125.0f,150.0f,1.0f);
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

				//mDecoratorManager.GetDisplay().RenderText("  *",tPartialX2,tPartialY2,2.5f,125.0f,125.0f,150.0f,1.0f);
				mDecoratorManager.GetDisplay().RenderText(tFull?"Prêt!":"En charge...", tFull?0.5f:-1.0f, 6.0f, 0.04f);
				mDecoratorManager.GetDisplay().PopTransformation();

				mIsAnimationStarted = true;

			return tFull;
		}
	}
	/*
	if (mRegrouper->IsPresent()) {
		float tPartialX2 = scX1Bar+((scX2Bar-scX1Bar)*(tElapsedTime/(float)cShotPreparationTime));
		tFull= false;

		if (tPartialX2 >= scX2Bar){
			tPartialX2 = scX2Bar;
			tFull = true;
		}

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates((mRegrouper->getMarker()), mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());

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

		mDecoratorManager.GetDisplay().RenderQuadFilled(scX1Bar, scY1Bar,tPartialX2, scY1Bar,tPartialX2, scY2Bar,scX1Bar, scY2Bar,tR,tG,tB);
		mDecoratorManager.GetDisplay().RenderText(tFull?" ":"Checking ", tFull?0.5f:-1.0f, 6.0f, 0.04f);
		mDecoratorManager.GetDisplay().PopTransformation();

		}

		return(tFull);*/
}

bool decorators::RegroupDigits::Regroup(int pType, int pDigit, float pOriginPoint_x, float pOriginPoint_y, float pEndPoint_x, float pEndPoint_y, bool pIsNecessary){

	/*
	int mDigitToArrange = (pType == 0) ? pDigit*10 : pDigit*100;
	bool tFull;
	Regrouper *mRegrouper = (pType == 0) ? mRegroupers[0] : mRegroupers[1];

	static const float scX1 = -1.0f;
	static const float scX2 = 4.5f;
	static const float scY1 = 6.5f;
	static const float scY2 = 7.0f;

	static const long cShotPreparationTime = 10l*1000l;

	long tElapsedTime = Time::MillisTimestamp() - mLastShotUnits2Tens;

	if (mRegrouper->IsPresent() && tElapsedTime > cShotPreparationTime) {
		mLastShotUnits2Tens = Time::MillisTimestamp();
		tFull = false;
	}

	if (mRegrouper->IsPresent()) {
			float tPartialX2 = scX1+((scX2-scX1)*(tElapsedTime/(float)cShotPreparationTime));
			tFull= false;

			if (tPartialX2 >= scX2){
				tPartialX2 = scX2;
				tFull = true;
			}

			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates((mRegrouper->getMarker()), mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());

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
*/

	/*bool tFull = false;
	int mDigitToArrange = (pType == 0) ?  pDigit*10 : pDigit*100;
	bool tFullCharging;
	Regrouper *mRegrouper = (pType == 0) ? mRegroupers[0] : mRegroupers[1];
	long mLastShot = (pType == 0) ? mLastShotUnits2Tens : mLastShotTens2Cents;

	static const float scX1Bar = -1.0f;
	static const float scX2Bar = 4.5f;
	static const float scY1Bar = 6.5f;
	static const float scY2Bar = 7.0f;

	long tElapsedTime = Time::MillisTimestamp() - mLastShot;

	if(mRegrouper->IsPresent()){
		if(tElapsedTime > cShotPreparationTime){
			mLastShotUnits2Tens = Time::MillisTimestamp();
			//return false;
			//tElapsedTime = Time::MillisTimestamp() - mLastShotUnits2Tens;
			//tElapsedTime = 0;
		}

		float tPartialX2 = scX1Bar + (scX2Bar - scX1Bar)*(tElapsedTime/(float)cShotPreparationTime);
		tFull = false;

		if(tPartialX2 >= scX2Bar){
			tPartialX2 = scX2Bar;
			tFull = true;
		}

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates((mRegrouper->getMarker()), mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
		mDecoratorManager.GetDisplay().RenderQuadFilled(
									scX1Bar, scY1Bar,
									tPartialX2, scY1Bar,
									tPartialX2, scY2Bar,
									scX1Bar, scY2Bar,
									tFull?0.0f:1.0f, 1.0f, 0.0f);
		mDecoratorManager.GetDisplay().RenderText(tFull?"Prêt!":"En charge...", tFull?0.5f:-1.0f, 6.0f, 0.04f);
		mDecoratorManager.GetDisplay().PopTransformation();

		if(tFull) return true;

	}*/
	/*
	long tElapsedTime = Time::MillisTimestamp() - mLastShot;

	if(mRegrouper->IsPresent() && tElapsedTime > cShotPreparationTime) {

		mLastShotUnits2Tens = Time::MillisTimestamp();
	}

	if(mRegrouper->IsPresent() ){
			float tPartialX2 = scX1Bar+(scX2Bar-scX1Bar)*(tElapsedTime/(float)cShotPreparationTime);
					//bool tFull = false;
				tFull = false;
					if (tPartialX2 >= scX2Bar) {
						tPartialX2 = scX2Bar;
						tFull = true;
					}

					mDecoratorManager.GetDisplay().PushTransformation();
					mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates((mRegrouper->getMarker()), mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
					mDecoratorManager.GetDisplay().RenderQuadFilled(
							scX1Bar, scY1Bar,
							tPartialX2, scY1Bar,
							tPartialX2, scY2Bar,
							scX1Bar, scY2Bar,
							tFull?0.0f:1.0f, 1.0f, 0.0f);
					mDecoratorManager.GetDisplay().RenderText(tFull?"Prêt!":"En charge...", tFull?0.5f:-1.0f, 6.0f, 0.04f);
					mDecoratorManager.GetDisplay().PopTransformation();


		}
*/


	//return tFull;



	//return tFullCharging;

		/*switch(pType){
		case 0:
			bool tFull;
			if(mRegrouper->IsPresent()){
				//bool tTraslationDone = false;
				if(tElapsedTime > cShotPreparationTime){
					//tTraslationDone = TranslateNumber(pType,mDigitToArrange,pOriginPoint_x,pOriginPoint_y,pEndPoint_x,pEndPoint_y, Time::MillisTimestamp());
					//tTraslationDone = TranslateNumber(pType,mDigitToArrange,pOriginPoint_x,pOriginPoint_y,pEndPoint_x,pEndPoint_y);
					mLastShotUnits2Tens = Time::MillisTimestamp();
				}

				float tPartialX2U2T = scX1Bar+(scX2Bar-scX1Bar)*(tElapsedTime/(float)cShotPreparationTime);
				tFull= false;

				if (tPartialX2U2T >= scX2Bar){
					tPartialX2U2T = scX2Bar;
					tFull = true;
				}

				mDecoratorManager.GetDisplay().PushTransformation();
				mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates((mRegrouper->getMarker()), mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());

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

				mDecoratorManager.GetDisplay().RenderQuadFilled(scX1Bar, scY1Bar,
						tPartialX2U2T, scY1Bar,
						tPartialX2U2T, scY2Bar,
						scX1Bar, scY2Bar,tR,tG,tB);
				mDecoratorManager.GetDisplay().RenderText(tFull?" ":"Checking ", tFull?0.5f:-1.0f, 6.0f, 0.04f);
				mDecoratorManager.GetDisplay().PopTransformation();

				//return (tFull);

			}

			return (tFull);

			break;
		case 1:
			if(mRegrouper->IsPresent() && tElapsedTime > cShotPreparationTime) mLastShotTens2Cents = Time::MillisTimestamp();

			if(mRegrouper->IsPresent()){
				float tPartialX2T2C = scX1Bar+(scX2Bar-scX1Bar)*(tElapsedTime/(float)cShotPreparationTime);
				bool tFull= false;

				if (tPartialX2T2C >= scX2Bar){
					tPartialX2T2C = scX2Bar;
					tFull = true;
				}

				mDecoratorManager.GetDisplay().PushTransformation();
				mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates((mRegrouper->getMarker()), mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
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
				mDecoratorManager.GetDisplay().RenderQuadFilled(scX1Bar, scY1Bar,tPartialX2T2C, scY1Bar,tPartialX2T2C, scY2Bar,scX1Bar, scY2Bar,tR,tG,tB);
				mDecoratorManager.GetDisplay().RenderText(tFull?" ":"Checking ", tFull?0.5f:-1.0f, 6.0f, 0.04f);
				mDecoratorManager.GetDisplay().PopTransformation();
			}
			return false;
			break;
		}

return 0;*/
}

bool decorators::RegroupDigits::IsGrouperPresent(){
	return (IsGrouperPresent(0) || IsGrouperPresent(1));
}

bool decorators::RegroupDigits::IsGrouperPresent(int pType){
	//return (mRegroupers[pType]->IsPresent());
	return (mFlippers[pType]->IsPresent());
}

bool decorators::RegroupDigits::TranslateNumber(int pType, int pDigit, float pOriginPoint_x, float pOriginPoint_y, float pEndPoint_x, float pEndPoint_y){
/*
	static const long cTranslationTime = 10l*1500l;
	Regrouper *mRegrouper = mFlippers[pType];
	bool tIsTraslationDone;

	long tElapsedTime = Time::MillisTimestamp() - mTraslationTime; //FIXME

	wykobi::point2d<float> tOrigin;
	tOrigin.x = pOriginPoint_x;
	tOrigin.y = pOriginPoint_y;
	wykobi::point2d<float> tEnd;
	tEnd.x = pEndPoint_x;
	tEnd.y = pEndPoint_y;

	mDecoratorManager.GetCam2World().InterpolatedMap(tOrigin);
	mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);
	mDecoratorManager.GetCam2World().InterpolatedMap(tEnd);
	mDecoratorManager.GetWorld2Proj().InterpolatedMap(tEnd);
	mDecoratorManager.GetDisplay().RenderLine(tOrigin.x, tOrigin.y, tEnd.x, tEnd.y, 0.0f, 0.0f, 0.0f);
*/
	/*
	const long tInitialTime = Time::MillisTimestamp();

	wykobi::point2d<float> tOrigin;
	tOrigin.x = pOriginPoint_x;
	tOrigin.y = pOriginPoint_y;
	wykobi::point2d<float> tEnd;
	tEnd.x = pEndPoint_x;
	tEnd.y = pEndPoint_y;

	mDecoratorManager.GetCam2World().InterpolatedMap(tOrigin);
			mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);
			mDecoratorManager.GetCam2World().InterpolatedMap(tEnd);
			mDecoratorManager.GetWorld2Proj().InterpolatedMap(tEnd);
			mDecoratorManager.GetDisplay().RenderLine(tOrigin.x, tOrigin.y, tEnd.x, tEnd.y, 0.0f, 0.0f, 0.0f);

	switch(pType){
					case 0:
					{

						long tElapsedTime = Time::MillisTimestamp() - tInitialTime;
						//if(mRegrouper->IsPresent() && tElapsedTime > cChangeTime) mLastShotUnits2Tens = Time::MillisTimestamp();
						//if(mRegrouper->IsPresent() && tElapsedTime > cTranslationTime) mLastShotUnits2Tens = Time::MillisTimestamp();

						if(mRegrouper->IsPresent()){
							float tPartialLineX2 = tOrigin.x + (tEnd.x - tOrigin.x)*(tElapsedTime/(float)(cTranslationTime));
							float tPartialLineY2 = tOrigin.y + (tEnd.y - tOrigin.y)*(tElapsedTime/(float)(cTranslationTime));
							tIsTraslationDone = false;

							if(tPartialLineX2 >= tEnd.x && tPartialLineY2 >= tEnd.y){
								tPartialLineX2 = tEnd.x;
								tPartialLineY2 = tEnd.y;
								tIsTraslationDone = true;
							}

							if(!tIsTraslationDone){
								char * tDigit = new char[3];
								sprintf(tDigit,"%d",pDigit);
								mDecoratorManager.GetDisplay().RenderText(tDigit, tPartialLineX2,tPartialLineY2,1.0f,0.0f,0.0f,0.0f,1.0f);
							}
						}

						break;
					}
					case 1:


						//if(mRegrouper->IsPresent() && tElapsedTime > cTranslationTime) mLastShotTens2Cents = Time::MillisTimestamp();
						break;

	}

	return true;
	-----




	long tElapsedTime = Time::MillisTimestamp() - mLastShotUnits2Tens;

	if (mFlipper->IsFlipped() && tElapsedTime > cShotPreparationTime && mFlipper->IsSecondSideUp()) {
		mLastShotUnits2Tens = Time::MillisTimestamp();
		mIsAnimationStarted = false;
		//tFull = false;
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

								//mDecoratorManager.GetDisplay().RenderText("  *",tPartialX2,tPartialY2,2.5f,125.0f,125.0f,150.0f,1.0f);
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

				mDecoratorManager.GetDisplay().RenderText("  *",tPartialX2,tPartialY2,2.5f,125.0f,125.0f,150.0f,1.0f);
				mDecoratorManager.GetDisplay().RenderText(tFull?"Prêt!":"En charge...", tFull?0.5f:-1.0f, 6.0f, 0.04f);
				mDecoratorManager.GetDisplay().PopTransformation();

				mIsAnimationStarted = true;

			return tFull;
		}
	}
*/
}
