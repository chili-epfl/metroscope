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

#include "SetModel.hpp"
#include <iostream>

const std::string decorators::SetModel::scDecoratorName("SetModel");
const  DecoratorManager::Registerer decorators::SetModel::mRegisterer(decorators::SetModel::scDecoratorName, &decorators::SetModel::create);

decorators::FiducialDecorator *decorators::SetModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tColor = pSetting["color"];
		libconfig::Setting & tRadius = pSetting["radius"];
		return new decorators::SetModel(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), tColor[0], tColor[1], tColor[2], tRadius);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::SetModel::SetModel(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, float pRed, float pGreen, float pBlue, float pRadius):
FiducialDecorator(pDecoratorManager, pMarker),
mRed(pRed),
mGreen(pGreen),
mBlue(pBlue),
mRadius(pRadius)
{
}

void decorators::SetModel::update() {

	if (mMarker->isPresent())
	{
		wykobi::quadix<float ,2> tMarkerCorners = mMarker->getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
		mDecoratorManager.GetWorld2Proj().InterpolatedMapOnQuad(tMarkerCorners);
		wykobi::point2d<float> tOrigin;
		wykobi::vector2d<float> tXUnit;
		wykobi::vector2d<float> tYUnit;
		//FIXME: not correct to use in projection coordinates, but well...
		FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

		mDecoratorManager.GetDisplay().RenderEllipse(tOrigin.x, tOrigin.y, mRadius*wykobi::vector_norm(tXUnit), mRadius*wykobi::vector_norm(tYUnit), mRed, mGreen, mBlue);
	}
}
