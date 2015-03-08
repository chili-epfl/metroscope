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

#include "RotationShoot.hpp"
#include <iostream>
#include <cmath>

const std::string decorators::RotationShoot::scDecoratorName("RotationShoot");
const DecoratorManager::Registerer decorators::RotationShoot::mRegisterer(decorators::RotationShoot::scDecoratorName, &decorators::RotationShoot::create);

decorators::FiducialDecorator *decorators::RotationShoot::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try{
		return new decorators::RotationShoot(pDecoratorManager,
				(AngleShoot *) pDecoratorManager.loadDecorator(pSetting["angle_model"]),
				pSetting["unit_fraction"]);
	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::RotationShoot::RotationShoot(DecoratorManager &pDecoratorManager, AngleShoot *pAngleModel, bool pUnit):
		FiducialDecorator(pDecoratorManager,0),
		mProportion(0.0f),
		mNumerator(0),
		mDenominator(1),
		mAngleModel(pAngleModel),
		mAngle(0.0f),
		mUnit(pUnit){

}

bool decorators::RotationShoot::isPresent(){
	return (mAngleModel->isPresent());
}

void decorators::RotationShoot::update(){
	if(isPresent()){
		wykobi::point2d<float> mOriginPoint = mAngleModel->OriginPoint();
		wykobi::point2d<float> mEndPoint = mAngleModel->EndPoint();

		wykobi::quadix<float ,2> tMarkerCorners = mAngleModel->getMarker().getCorners();
		wykobi::point2d<float> mStartPoint = wykobi::centroid(tMarkerCorners[0],tMarkerCorners[1]);

		wykobi::point2d<float> tWorldOriginPoint = mOriginPoint;
		wykobi::point2d<float> tWorldStartPoint = mStartPoint;
		wykobi::point2d<float> tWorldEndPoint = mEndPoint;

		mDecoratorManager.GetCam2World().InterpolatedMap(tWorldOriginPoint);
		mDecoratorManager.GetCam2World().InterpolatedMap(tWorldStartPoint);
		mDecoratorManager.GetCam2World().InterpolatedMap(tWorldEndPoint);

		mAngle = wykobi::oriented_vertex_angle(tWorldStartPoint, tWorldOriginPoint, tWorldEndPoint, wykobi::Clockwise);

		mProportion = mAngle/360.0f;

		//CalculateFractionFromDecimal();
		DisplayValue();

		//TODO: Update this move's state to propagate it to the server

	}
}

void decorators::RotationShoot::DisplayValue(){

	if(mUnit==scUNIT_FRACTION){//It's the display in fraction units
		std::ostringstream tMessage;
		tMessage.precision(3);
		tMessage << "rotation = " << mProportion;
		std::string tmp = tMessage.str();

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(mAngleModel->getMarker(), scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
			mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(tmp.c_str(), scTEXT_DELIMITERS,
					scOFFSETMSG_X, scOFFSETMSG_Y, 600,
					true, 0.65,
					0.0, 0.0, 0.0, 1.0);
			mDecoratorManager.GetDisplay().PopTransformation();

	}else{//It's the display in degrees

		std::ostringstream tMessage;
		tMessage.precision(3);
		tMessage << "rotation = " << mAngle;
		std::string tmp = tMessage.str();

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(mAngleModel->getMarker(), scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
			mDecoratorManager.GetDisplay().RenderCenteredTextFixedWidth(tmp.c_str(), scTEXT_DELIMITERS,
					scOFFSETMSG_X, scOFFSETMSG_Y, 600,
					true, 0.65,
					0.0, 0.0, 0.0, 1.0);
			mDecoratorManager.GetDisplay().PopTransformation();

	}
}


//void decorators::RotationShoot::CalculateFractionFromDecimal(){
//
//	//We'll use at max 8 iterations (less than checking each fraction that has denominator 2,3,4,5,6,7,8,9 or 10)
//	int tMaxIterations = 8;
//
//	std::vector<double> tZ(tMaxIterations+1);
//	std::vector<double> tD(tMaxIterations+1);
//
//	//We'll simplify the proportion so it'll have 2 numbers after the comma (0.25555454843213541 ~ 0.26)
//	double tX;
//	int tModule = (int)(mProportion*100)%10;
//	if(tModule >= 7 && tModule <= 9) tX = floor(mProportion*10 + 0.5)/10;
//	else if(tModule >= 3 && tModule <= 6) tX= (floor(mProportion*10)+0.5)/10;
//	else tX = floor(mProportion*10)/10;
//
//	//Initialize the variables
//	int tIterations = 1;
//	tZ[0] = 0.0;
//	tZ[1] = tX;
//	tD[0] = 0.0;
//	tD[1] = 1.0;
//
//	double tN;
//
//	//We put an epsilon of 0.001 to stop
//	while(tIterations < 8 && tZ[tIterations] - (int)tZ[tIterations] > 0.001){
//
//		tZ[tIterations + 1] = 1/(tZ[tIterations] - (int)tZ[tIterations]);
//		tD[tIterations + 1] = tD[tIterations]*(int)tZ[tIterations+1] + tD[tIterations-1];
//		tN = (tX*tD[tIterations + 1] + 0.5 >= (int)(tX*tD[tIterations + 1]) + 1) ? int(tX*tD[tIterations + 1])+1 : (int)(tX*tD[tIterations + 1]);
//
//		tIterations++;
//	}
//
//	mNumerator = tN;
//	mDenominator = tD[tIterations];
//}
//
