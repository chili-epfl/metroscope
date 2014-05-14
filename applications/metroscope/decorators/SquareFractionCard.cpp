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

#include "SquareFractionCard.hpp"
#include <iostream>
#include <cmath>

const std::string decorators::SquareFractionCard::scDecoratorName("SquareFractionCard");
const DecoratorManager::Registerer decorators::SquareFractionCard::mRegisterer(decorators::SquareFractionCard::scDecoratorName, &decorators::SquareFractionCard::create);

decorators::FiducialDecorator *decorators::SquareFractionCard::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager){
	try{
		return new decorators::SquareFractionCard(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), (int)pSetting["denominator"], (int)pSetting["color"]);
	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::SquareFractionCard::SquareFractionCard(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, int pDenom, int pType):
		FiducialDecorator(pDecoratorManager, pMarker),
		mDenom(pDenom),
		mType(pType){
			switch(mDenom){
			case 2:
				wykobi::point2d<float> tExpectedPoint1;
				wykobi::point2d<float> tExpectedPoint2;
				wykobi::point2d<float> tExpectedPoint3;
				wykobi::point2d<float> tExpectedPoint4;

				tExpectedPoint1.x = 7.0;
				tExpectedPoint1.y = 7.0;
				tExpectedPoint2.x = 13.0;
				tExpectedPoint2.y = 13.0;
				tExpectedPoint3.x = 13.0;
				tExpectedPoint3.y = 7.0;
				tExpectedPoint4.x = 7.0;
				tExpectedPoint4.y = 13.0;

				mExpectedLocation.push_back(tExpectedPoint1);
				mExpectedLocation.push_back(tExpectedPoint2);
				mExpectedLocation.push_back(tExpectedPoint3);
				mExpectedLocation.push_back(tExpectedPoint4);
				mPosibleAngle.push_back(0);
				mPosibleAngle.push_back(90);
				mPosibleAngle.push_back(180);
				mPosibleAngle.push_back(270);
				break;
			}
}

void decorators::SquareFractionCard::update(){

}

bool decorators::SquareFractionCard::isInCorrectPlace(FiducialMarker *pMarker){
	wykobi::point2d<float> tActualPosition = GetLocation();

	mDecoratorManager.GetCam2World().InterpolatedMap(tActualPosition);

	return pointIsInRange(tActualPosition);
	/*for(int i = 0 ; i < mExpectedLocation.size() ; i++){
		wykobi::point2d<float> tPoint;
		tPoint.x = mExpectedLocation[i].x + 10.0f;
		tPoint.y = mExpectedLocation[i].y;

		wykobi::point2d<float> tReference = pMarker->getCenter();

		mDecoratorManager.GetCam2World().InterpolatedMap(tReference);
		mDecoratorManager.GetCam2World().InterpolatedMap(tPoint);

		if (pointIsInRange(tActualPosition)) return true;
	}
	return false;
	*/
}

/*bool decorators::SquareFractionCard::pointIsInRange(wykobi::point2d<float> tReferencePoint, wykobi::point2d<float> tPoint){
	return ((-1 < abs(tReferencePoint.x - tPoint.x) || abs(tReferencePoint.x - tPoint.x) < 5) && (-1 < abs(tReferencePoint.y - tPoint.y) || abs(tReferencePoint.y - tPoint.y) < 5));
}
*/
bool decorators::SquareFractionCard::pointIsInRange(wykobi::point2d<float> tPoint){

	return 0;
}
