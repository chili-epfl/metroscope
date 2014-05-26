/*******************************************************************************
*   Copyright 2013 EPFL                                                        *
*                                                                              *
*   This file is part of metroscope.                                           *
*                                                                              *
*   Metroscope is free software: you can redistribute it and/or modify         *
*   it under the terms of the GNU General Public License as                    *
*   published by the Free Software Foundation, either version 3 of the         *
*   License, or (at your option) any later version.                            *
*                                                                              *
*   Metroscope is distributed in the hope that it will be useful,              *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of             *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
*   GNU General Public License for more details.                               *
*                                                                              *
*   You should have received a copy of the GNU General Public License          *
*   along with Metroscope.  If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************/

#ifndef TOKENMODEL_HPP_
#define TOKENMODEL_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "FractionToken.hpp"
#include <list>
#include <queue>
#include <deque>

namespace decorators {

	class TokenModel : public FiducialDecorator
	{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		TokenModel (DecoratorManager &pDecoratormanager, FractionToken **pTokens, int pTokenNumbers);
		~TokenModel();

		int getTotalTokens() {return mTotalActiveTokens;};
		int getActiveTokens(int pType){
			return (pType == 0 ? mActiveFirstToken : mActiveSecondToken);
		}
		float GetProportion(){return (float)(mActiveFirstToken/(float)mTotalActiveTokens);}
		bool isPresent();
		bool AreTokensSpread();
		wykobi::point2d<float> GetPosition();


	protected:
		void update();
		int mTotalActiveTokens;
		int mActiveFirstToken;
		int mActiveSecondToken;


	private:

		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		const int mTokenNumbers;
		FractionToken **mTokens;
		std::vector<FractionToken *>mFirstTokens;
		std::vector<FractionToken *>mSecondTokens;
		std::vector<FractionToken *>mActiveTokens;
		int mTokenFirstCuadrant, mTokenSecondCuadrant, mTokenThirdCuadrant, mTokenFourthCuadrant;
	};
}




#endif /* TOKENMODEL_HPP_ */
