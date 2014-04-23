/*
 * RegroupDigirts.hpp
 *
 *  Created on: Apr 1, 2014
 *      Author: daniela
 */

#ifndef REGROUPDIGITS_HPP_
#define REGROUPDIGITS_HPP_

//#include "Regrouper.hpp"
#include "Flipper.hpp"
#include <qa/pipeables/misc/DecoratorManager.hpp>

namespace decorators{

class RegroupDigits : public FiducialDecorator
{
	public:

		static FiducialDecorator *create (libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		RegroupDigits (DecoratorManager &pDecoratorManager, Flipper ** pFlippers, int pFlipperNumber);
		//RegroupDigits (DecoratorManager &pDecoratorManager, Regrouper ** pRegroupers);
		virtual ~RegroupDigits();

		bool RegroupAnimation(int pType, bool pIsNecessary, bool pAreCardsFromStacked, bool pAreCardsToStacked);
		void UpdateLastShot(int pType);
		//void SetTraslationInitialTime() {mTraslationTime = Time::MillisTimestamp();};
		bool IsGrouperPresent();
		bool IsGrouperPresent(int pType);
		//Regrouper * GetRegrouper(int pType){return mRegroupers[pType];};
		long mLastShotUnits2Tens;
		long mLastShotTens2Cents;

	protected:

		void update();
		Flipper ** mFlippers;
		int mFlipperNumber;
		bool mIsAnimationStarted;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		static const long cShotPreparationTime = 10l*650l;




};
}

#endif /* REGROUPDIGITS_HPP_ */
