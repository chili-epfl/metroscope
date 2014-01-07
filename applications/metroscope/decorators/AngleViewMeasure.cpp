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

#include "AngleViewMeasure.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>

const std::string decorators::AngleViewMeasure::scDecoratorName("AngleViewMeasure");
const DecoratorManager::Registerer decorators::AngleViewMeasure::mRegisterer(decorators::AngleViewMeasure::scDecoratorName, &decorators::AngleViewMeasure::create);

decorators::FiducialDecorator *decorators::AngleViewMeasure::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		if (pSetting.exists("expected"))
		{
			return new decorators::AngleViewMeasure(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), (AngleModel *) pDecoratorManager.loadDecorator(pSetting["angle"]), pSetting["expected"], pSetting["correct"], pSetting["close"]);
		}
		return new decorators::AngleViewMeasure(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), (AngleModel *) pDecoratorManager.loadDecorator(pSetting["angle"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::AngleViewMeasure::AngleViewMeasure(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, AngleModel *pAngle, float pExpectedAngle, float pCorrectTolerance, float pCloseTolerance):
FiducialDecorator(pDecoratorManager, pMarker),
mAngleModel(pAngle),
mColors(true),
mExpectedAngle(pExpectedAngle),
mCorrectTolerance(pCorrectTolerance),
mCloseTolerance(pCloseTolerance)
{
	while (mExpectedAngle < 0.0f) mExpectedAngle += 360.0f;
	while (mExpectedAngle >= 360.0f) mExpectedAngle -= 360.0f;
}

decorators::AngleViewMeasure::AngleViewMeasure(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, AngleModel *pAngle):
FiducialDecorator(pDecoratorManager, pMarker),
mAngleModel(pAngle),
mColors(false)
{
}

void decorators::AngleViewMeasure::update() {

	if (mMarker->isPresent() && mAngleModel->isPresent())
	{
		float tAngle = mAngleModel->Angle();

		float tRed = 1.0f;
		float tGreen = 0.0f;
		float tBlue = 0.0f;

		if (mColors)
		{
			float tDifference = AngleModel::AbsoluteDifference(tAngle, mExpectedAngle);
			tRed = (tDifference <= mCorrectTolerance)?0.0f:1.0f;
			tGreen = (tDifference <= mCloseTolerance)?1.0f:0.0f;
		}

		wykobi::quadix<float ,2> tMarkerCorners = mMarker->getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
		wykobi::point2d<float> tOrigin;
		wykobi::point2d<float> tXUnit;
		wykobi::point2d<float> tYUnit;
		FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);

		char tText[20];
		sprintf(tText, "%.0f°", tAngle);
		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().Rotate(-wykobi::cartesian_angle(tXUnit), tOrigin.x, tOrigin.y);
		mDecoratorManager.GetDisplay().RenderText(tText, tOrigin.x, tOrigin.y, 1.0f, tRed, tGreen, tBlue);
		mDecoratorManager.GetDisplay().PopTransformation();
	}
}
