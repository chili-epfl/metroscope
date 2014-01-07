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

#include "MarkerMask.hpp"
#include <iostream>

const std::string decorators::MarkerMask::scDecoratorName("MarkerMask");
const DecoratorManager::Registerer decorators::MarkerMask::mRegisterer(decorators::MarkerMask::scDecoratorName, &decorators::MarkerMask::create);

decorators::FiducialDecorator *decorators::MarkerMask::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::MarkerMask(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

void decorators::MarkerMask::update() {
	if (mMarker->isPresent())
	{
		wykobi::quadix<float ,2> tMarkerCorners = mMarker->getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
		wykobi::point2d<float> tMarkerOrigin;
		wykobi::vector2d<float> tMarkerXUnit;
		wykobi::vector2d<float> tMarkerYUnit;
		FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tMarkerOrigin, tMarkerXUnit, tMarkerYUnit);

		static const float scWhiteBorderRatio = 7.0f/5.0f;
		tMarkerCorners[0] = tMarkerOrigin - scWhiteBorderRatio*tMarkerXUnit + scWhiteBorderRatio*tMarkerYUnit;
		tMarkerCorners[1] = tMarkerOrigin + scWhiteBorderRatio*tMarkerXUnit + scWhiteBorderRatio*tMarkerYUnit;
		tMarkerCorners[2] = tMarkerOrigin + scWhiteBorderRatio*tMarkerXUnit - scWhiteBorderRatio*tMarkerYUnit;
		tMarkerCorners[3] = tMarkerOrigin - scWhiteBorderRatio*tMarkerXUnit - scWhiteBorderRatio*tMarkerYUnit;

		mDecoratorManager.GetWorld2Proj().InterpolatedMapOnQuad(tMarkerCorners);
		mDecoratorManager.GetDisplay().RenderQuadFilled(
				tMarkerCorners[0].x, tMarkerCorners[0].y,
				tMarkerCorners[1].x, tMarkerCorners[1].y,
				tMarkerCorners[2].x, tMarkerCorners[2].y,
				tMarkerCorners[3].x, tMarkerCorners[3].y,
				1.0f, 1.0f, 1.0f, 0.8f);
	}
}
