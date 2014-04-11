/*
 * RegroupDigirts.hpp
 *
 *  Created on: Apr 1, 2014
 *      Author: daniela
 */

#ifndef REGROUPDIGITS_HPP_
#define REGROUPDIGITS_HPP_

#include "Regrouper.hpp"
#include <qa/pipeables/misc/DecoratorManager.hpp>

namespace decorators{

class RegroupDigits : public FiducialDecorator
{
	public:

		static FiducialDecorator *create (libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		RegroupDigits (DecoratorManager &pDecoratorManager, Regrouper ** pRegroupers);
		virtual ~RegroupDigits();

		bool Regroup(int pType, int pDigit,
				float pOriginPoint_x, float pOriginPoint_y,
				float pEndPoint_x, float pEndPoint_y,
				bool pIsNecessary); //0 if the pDigit are units to be regrouped as tens and 1 if its tens to cents

		bool IsGrouperPresent();
		bool IsGrouperPresent(int pType);

	protected:

		void update();
		Regrouper ** mRegroupers;
		long mLastShotUnits2Tens;
		long mLastShotTens2Cents;
		//int mRegrouperNumber;



	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;


};
}

#endif /* REGROUPDIGITS_HPP_ */
