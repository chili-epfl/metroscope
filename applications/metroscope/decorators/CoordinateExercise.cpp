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

#include "CoordinateExercise.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>
#include <cmath>


const std::string decorators::CoordinateExercise::scDecoratorName("CoordinateExercise");
const DecoratorManager::Registerer decorators::CoordinateExercise::mRegisterer(decorators::CoordinateExercise::scDecoratorName, &decorators::CoordinateExercise::create);

decorators::FiducialDecorator *decorators::CoordinateExercise::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::CoordinateExercise(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), (LinearShoot *) pDecoratorManager.loadDecorator(pSetting["linear_x"]), (LinearShoot *) pDecoratorManager.loadDecorator(pSetting["linear_y"]), pSetting["coord_x"], pSetting["coord_y"]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::CoordinateExercise::CoordinateExercise(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, LinearShoot *pLinearX, LinearShoot *pLinearY, int pExpectedX, int pExpectedY):
FiducialDecorator(pDecoratorManager, pMarker),
mLinearX(pLinearX),
mLinearY(pLinearY),
mColors(true),
mExpectedCoordX(pExpectedX),
mExpectedCoordY(pExpectedY)
{
}

void decorators::CoordinateExercise::update() {

	if (mMarker->isPresent() && mLinearX->isPresent() && mLinearY->isPresent())
	{
		int tX = (int) mLinearX->proportion();
		int tY = (int) mLinearY->proportion();

		float tRed = 1.0f;
		float tGreen = 0.0f;
		float tBlue = 0.0f;

		if (mColors)
		{
			if(tX-mExpectedCoordX==0 && tY-mExpectedCoordY==0){
				tRed = 0.0f;
				tGreen = 1.0f;
			} else if(abs(tX-mExpectedCoordX)<2 && abs(tY-mExpectedCoordY)<2){
				tRed = 0.8f;
				tGreen = 0.8f;
			}

		}

		wykobi::quadix<float ,2> tMarkerCorners = mMarker->getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
		wykobi::point2d<float> tOrigin;
		wykobi::point2d<float> tXUnit;
		wykobi::point2d<float> tYUnit;
		FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);

		char tText[20];
		sprintf(tText, "[ %d , %d ]", tX, tY);
		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().Rotate(-wykobi::cartesian_angle(tXUnit), tOrigin.x, tOrigin.y);
		mDecoratorManager.GetDisplay().RenderText(tText, tOrigin.x, tOrigin.y, 1.0f, tRed, tGreen, tBlue);
		mDecoratorManager.GetDisplay().PopTransformation();
	}
}
