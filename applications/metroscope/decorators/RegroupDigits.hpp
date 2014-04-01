/*
 * RegroupDigirts.hpp
 *
 *  Created on: Apr 1, 2014
 *      Author: daniela
 */

#ifndef REGROUPDIGITS_HPP_
#define REGROUPDIGITS_HPP_

#include "Flipper.hpp"
#include <qa/pipeables/misc/DecoratorManager.hpp>

namespace decorators{

class RegroupDigits : public FiducialDecorator
{
	public:

		static FiducialDecorator *create (libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		RegroupDigits (DecoratorManager &pDecoratorManager, Flipper ** pFlippers, int pFlipperNumber);
		virtual ~RegroupDigits();

		void Regroup(int pType, int pDigit, float pOriginPoint_x, float pOriginPoint_y, float pEndPoint_x, float pEndPoint_y); //0 if the pDigit are units to be regrouped as tens and 1 if its tens to cents

	protected:

		void update();
		Flipper ** mFlippers;
		long mLastShotUnits2Tens;
		long mLastShotTens2Cents;
		int mFlipperNumber;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
};
}

#endif /* REGROUPDIGITS_HPP_ */
