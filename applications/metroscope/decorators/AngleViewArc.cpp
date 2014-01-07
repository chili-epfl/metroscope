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

#include "AngleViewArc.hpp"
#include <iostream>

const std::string decorators::AngleViewArc::scDecoratorName("AngleViewArc");
const DecoratorManager::Registerer decorators::AngleViewArc::mRegisterer(decorators::AngleViewArc::scDecoratorName, &decorators::AngleViewArc::create);

decorators::FiducialDecorator *decorators::AngleViewArc::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & pColor = pSetting["color"];
		return new decorators::AngleViewArc(pDecoratorManager, (AngleModel *) pDecoratorManager.loadDecorator(pSetting["angle"]), pSetting["length"], pColor[0], pColor[1], pColor[2]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::AngleViewArc::AngleViewArc(DecoratorManager &pDecoratorManager, AngleModel *pAngle, float pLength, float pRed, float pGreen, float pBlue):
FiducialDecorator(pDecoratorManager, 0),
mAngle(pAngle),
mLength(pLength),
mRed(pRed), mGreen(pGreen), mBlue(pBlue)
{
}

void decorators::AngleViewArc::update() {

	if (mAngle->isPresent())
	{

		wykobi::point2d<float> tOrigin = mAngle->OriginPoint();
		mDecoratorManager.GetCam2World().InterpolatedMap(tOrigin);
		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);

		wykobi::point2d<float> tStart = mAngle->StartPoint();
		mDecoratorManager.GetCam2World().InterpolatedMap(tStart);
		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tStart);

		wykobi::point2d<float> tEnd = mAngle->EndPoint();
		mDecoratorManager.GetCam2World().InterpolatedMap(tEnd);
		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tEnd);

		float tAngle;
		float tArcStart;
		float tArcStop;
		float tArrowX;
		if (mAngle->Clockwise())
		{
			tAngle = -wykobi::oriented_vertex_angle(tStart, tOrigin, tEnd, wykobi::Clockwise);
			tArcStart = tAngle+180.0f;
			tArcStop = 180.0f;
			tArrowX = -mLength;
		}
		else
		{
			tAngle = wykobi::oriented_vertex_angle(tStart, tOrigin, tEnd, wykobi::CounterClockwise);
			tArcStart = 0.0f;
			tArcStop = tAngle;
			tArrowX = mLength;
		}
		static const float scArrowSize = 5.0f;

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().Translate(tOrigin.x, tOrigin.y);
		mDecoratorManager.GetDisplay().Rotate(tAngle-1.0f); //no idea why, but it fills the gap
		mDecoratorManager.GetDisplay().RenderArc(0.0f, 0.0f, mLength, mLength, tArcStart, tArcStop, mRed, mGreen, mBlue);
		mDecoratorManager.GetDisplay().RenderLine(tArrowX-scArrowSize, scArrowSize, tArrowX, 0.0f, mRed, mGreen, mBlue);
		mDecoratorManager.GetDisplay().RenderLine(tArrowX+scArrowSize, scArrowSize, tArrowX, 0.0f, mRed, mGreen, mBlue);
		mDecoratorManager.GetDisplay().PopTransformation();
	}
}
