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

#ifndef FRACTIONBUGHINT_HPP_
#define FRACTIONBUGHINT_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>


namespace decorators {

class FractionBugHint : public FiducialDecorator{
public:
	static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
	FractionBugHint(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, int pHintType);
	~FractionBugHint();


	int GetHintType(){return mHintType;}
	bool IsPresent(){return mMarker->isPresent();}
	int GetVaryingHintType(){return mVaryingHintType;}
	void VaryHintType();

	static const int scDISCRETE_TYPE = 0;
	static const int scFRACTION_TYPE = 1;
	static const int scDECIMAL_TYPE = 2;
	static const int scRECTANGULAR_TYPE = 3;
	static const int scCIRCULAR_TYPE = 4;
	static const int scVARYING_TYPE = 5;

protected:
	void update();

private:
	static const std::string scDecoratorName;
	static const DecoratorManager::Registerer mRegisterer;
	FiducialMarker *mMarker;
	int mHintType;
	int mVaryingHintType = scDECIMAL_TYPE;
};
}

#endif /* FRACTIONBUGHINT_HPP_ */

