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

#include "Flipper.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>


const std::string decorators::Flipper::scDecoratorName("Flipper");
const  DecoratorManager::Registerer decorators::Flipper::mRegisterer(decorators::Flipper::scDecoratorName, &decorators::Flipper::create);

decorators::FiducialDecorator *decorators::Flipper::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tSide1IDs = pSetting["side_1_marker_IDs"];
		libconfig::Setting & tSide2IDs = pSetting["side_2_marker_IDs"];
		int tNumIDsPerSide = pSetting["num_tags_per_side"];
		if(tNumIDsPerSide > 2) {std::cerr << "Flipper does not support more than two FiducialMarkers on a side" << std::endl; return 0;}
		Craftag *tSide1Tag1 = new Craftag(tSide1IDs[0]);
		Craftag *tSide2Tag1 =  new Craftag(tSide2IDs[0]);
		Craftag *tSide1Tag2 = NULL;
		Craftag *tSide2Tag2 = NULL;
		if(tNumIDsPerSide > 1){
			tSide1Tag2 =  new Craftag(tSide1IDs[1]);
			tSide2Tag2 = new Craftag(tSide2IDs[1]);
		}
		return new decorators::Flipper(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),
										pDecoratorManager.loadMarker(pSetting["marker_2"]),
										tSide1Tag1,
										tSide1Tag2,
										tSide2Tag1,
										tSide2Tag2);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::Flipper::Flipper(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pMarker2,
							Craftag *pSide1Tag1,
							Craftag *pSide1Tag2,
							Craftag *pSide2Tag1,
							Craftag *pSide2Tag2):
FiducialDecorator(pDecoratorManager, pMarker),
mMarker2(pMarker2),
mCurrentSide(NULL),
mLastFlipTimestamp(0),
mIsFlipped(false),
mSide1Tag1(pSide1Tag1),
mSide1Tag2(pSide1Tag2),
mSide2Tag1(pSide2Tag1),
mSide2Tag2(pSide2Tag2)
{

}

decorators::Flipper::~Flipper(){
	delete mSide1Tag1;
	delete mSide2Tag1;
	if (mSide1Tag2 != NULL) delete mSide1Tag2;
	if (mSide2Tag2 != NULL) delete mSide2Tag2;
}

void decorators::Flipper::update() {
	mIsFlipped = false;
	if (mMarker->isPresent() && mMarker2->isPresent()) //If both markers persist
	{
		int tNumFramesSinceSide1 = (mSide1Tag2 != NULL)? std::min(mSide1Tag1->GetFramesNumberSinceLastDetected(), mSide1Tag2->GetFramesNumberSinceLastDetected()) : mSide1Tag1->GetFramesNumberSinceLastDetected();
		int tNumFramesSinceSide2 = (mSide2Tag2 != NULL)? std::min(mSide2Tag1->GetFramesNumberSinceLastDetected(), mSide2Tag2->GetFramesNumberSinceLastDetected()) : mSide2Tag1->GetFramesNumberSinceLastDetected();
		if(tNumFramesSinceSide1 > tNumFramesSinceSide2){ //side2 is more recent
			if(mCurrentSide == mMarker){
				mIsFlipped = true;
				mLastFlipTimestamp = Time::MillisTimestamp();
			}
			mCurrentSide = mMarker2;
		} else { //side1 is more recent
			if(mCurrentSide == mMarker2){
				mIsFlipped = true;
				mLastFlipTimestamp = Time::MillisTimestamp();
			}
			mCurrentSide = mMarker;
		}

	} else if (mMarker->isPresent()){ //If only marker 1 persists
		if(mCurrentSide == mMarker2){
			mIsFlipped = true;
			mLastFlipTimestamp = Time::MillisTimestamp();
		}
		mCurrentSide = mMarker;
	} else if (mMarker2->isPresent()){ //If only marker 2 persists
		if(mCurrentSide == mMarker){
			mIsFlipped = true;
			mLastFlipTimestamp = Time::MillisTimestamp();
		}
		mCurrentSide = mMarker2;
	} else { //If the card isn't present at all
		mCurrentSide = NULL;
	}
}


