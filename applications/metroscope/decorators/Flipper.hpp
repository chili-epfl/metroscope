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

#ifndef Flipper_HPP
#define Flipper_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <qa/components/vision/Craftag.hpp>

namespace decorators {

class Flipper : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		/*If there is only one tag on each side, pass in NULL for pSide1Tag2 and pSide2Tag2*/
		Flipper(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pMarker2,
				Craftag *pSide1Tag1,
				Craftag *pSide1Tag2,
				Craftag *pSide2Tag1,
				Craftag *pSide2Tag2);

		~Flipper();

		bool IsPresent() const {return mMarker->isPresent() || mMarker2->isPresent();}
		bool IsFlipped() const {return mIsFlipped;}
		const FiducialMarker *GetCurrentSide() const {return mCurrentSide;}
		long GetTimeOfLastFlip() const {return mLastFlipTimestamp;}
		void SetTimeOfLastFlip(long pTimestamp) {mLastFlipTimestamp = pTimestamp;}

	protected:
		void update();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		const FiducialMarker *mMarker2;

		const FiducialMarker *mCurrentSide;
		long mLastFlipTimestamp;
		bool mIsFlipped;

		Craftag *mSide1Tag1;
		Craftag *mSide1Tag2; //If the marker is not aggregated, set this to NULL

		Craftag *mSide2Tag1;
		Craftag *mSide2Tag2; //If the marker is not aggregated, set this to NULL


};

}
#endif
