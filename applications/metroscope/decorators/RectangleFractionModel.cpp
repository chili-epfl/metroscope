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

#include "RectangleFractionModel.hpp"
#include <iostream>
#include <cmath>

const std::string decorators::RectangleFractionModel::scDecoratorName("RectangleFractionModel");
const DecoratorManager::Registerer decorators::RectangleFractionModel::mRegisterer(decorators::RectangleFractionModel::scDecoratorName, &decorators::RectangleFractionModel::create);

decorators::FiducialDecorator *decorators::RectangleFractionModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try{
		return new decorators::RectangleFractionModel(pDecoratorManager,pDecoratorManager.loadMarker(pSetting["start"]),pDecoratorManager.loadMarker(pSetting["end"]));
	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::RectangleFractionModel::RectangleFractionModel(DecoratorManager &pDecoratorManager, FiducialMarker *pStartRectangle, FiducialMarker *pEndRectangle):
		FiducialDecorator(pDecoratorManager,pStartRectangle),
		mStart(pStartRectangle),
		mEnd(pEndRectangle),
		mProportion(0),
		mDenominator(1),
		mNumerator(0)
		{
		}

bool decorators::RectangleFractionModel::isPresent(){
	return (mMarker->isPresent() && mEnd->isPresent());
}

void decorators::RectangleFractionModel::update(){
	if(isPresent()){
		wykobi::quadix<float ,2> tMarkerCorners = mMarker->getCorners();
		wykobi::point2d<float> tWorldEndRectangle = centroid(tMarkerCorners[1],tMarkerCorners[2]);
		wykobi::point2d<float> tWorldStartPoint =tMarkerCorners[0];
		wykobi::quadix<float ,2> tMarkerCorners2 = mEnd->getCorners();
		wykobi::point2d<float> tWorldEndPoint = tMarkerCorners2[0];

		mDecoratorManager.GetCam2World().InterpolatedMap(tWorldStartPoint);
		mDecoratorManager.GetCam2World().InterpolatedMap(tWorldEndRectangle);
		mDecoratorManager.GetCam2World().InterpolatedMap(tWorldEndPoint);


		float tTotalDistance = wykobi::distance(tWorldStartPoint,tWorldEndRectangle);
		float tDistance = wykobi::distance(tWorldStartPoint,tWorldEndPoint);

		//int tTotalDistance = (int)abs(tWorldStartPoint.x - tWorldEndRectangle.x);
		//int tDistance = (int)abs(tWorldStartPoint.x - tWorldEndPoint.x);

		mProportion = (float)tDistance/tTotalDistance;
		if(mProportion > 1) mProportion = 1;

		mDenominator = 10;
		mNumerator = ((int)(mProportion*100)%10 >= 5) ? ceil((mProportion*100)/10) : floor((mProportion*100)/10);
	}
}

