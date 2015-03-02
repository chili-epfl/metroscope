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

#include "PolyViewPerimeter.hpp"
#include <iostream>

const std::string decorators::PolyViewPerimeter::scDecoratorName("PolyViewPerimeter");
const  DecoratorManager::Registerer decorators::PolyViewPerimeter::mRegisterer(decorators::PolyViewPerimeter::scDecoratorName, &decorators::PolyViewPerimeter::create);

decorators::FiducialDecorator *decorators::PolyViewPerimeter::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tColor = pSetting["color"];
		return new decorators::PolyViewPerimeter(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), *(PolyModel *)pDecoratorManager.loadDecorator(pSetting["model"]), tColor[0], tColor[1], tColor[2]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::PolyViewPerimeter::PolyViewPerimeter(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const PolyModel &pPolyModel,
	float pRed, float pGreen, float pBlue):
FiducialDecorator(pDecoratorManager, pMarker),
mPolyModel(pPolyModel),
mRed(pRed),
mGreen(pGreen),
mBlue(pBlue)
{
}

void decorators::PolyViewPerimeter::update() {
	if (mMarker->isPresent())
	{
		wykobi::quadix<float ,2> tMarkerCorners = mMarker->getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
		wykobi::point2d<float> tOrigin;
		wykobi::point2d<float> tXUnit;
		wykobi::point2d<float> tYUnit;
		FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

		wykobi::polygon<float ,2> tPolyCorners;
		for (int i = 0; i < mPolyModel.getSize(); ++i) {
			//tPolyCorners[i] = tOrigin + mPolyModel.getPolygon()[i].x*tXUnit + mPolyModel.getPolygon()[i].y*tYUnit;
			tPolyCorners.push_back(tOrigin + mPolyModel.getPolygon()[i].x*tXUnit + mPolyModel.getPolygon()[i].y*tYUnit);
		}

		//We draw the polygon
		mDecoratorManager.GetWorld2Proj().InterpolatedMapOnPolygon(tPolyCorners, mPolyModel.getSize());
		for (int i = 0; i < mPolyModel.getSize(); ++i) {
			mDecoratorManager.GetDisplay().RenderLine(tPolyCorners[i].x, tPolyCorners[i].y, tPolyCorners[(i+1)%mPolyModel.getSize()].x, tPolyCorners[(i+1)%mPolyModel.getSize()].y, mRed, mGreen, mBlue);
		}

		//Add the marker for the origin point -- for now, we just take the first corner of the polygon
		mDecoratorManager.GetDisplay().RenderEllipse(tPolyCorners[0].x, tPolyCorners[0].y, 10, 10, mRed, mGreen, mBlue);

	}
}
