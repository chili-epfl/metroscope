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

#include "QuadViewPerimeter.hpp"
#include <iostream>

const std::string decorators::QuadViewPerimeter::scDecoratorName("QuadViewPerimeter");
const  DecoratorManager::Registerer decorators::QuadViewPerimeter::mRegisterer(decorators::QuadViewPerimeter::scDecoratorName, &decorators::QuadViewPerimeter::create);

decorators::FiducialDecorator *decorators::QuadViewPerimeter::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tColor = pSetting["color"];
		return new decorators::QuadViewPerimeter(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), *(QuadModel *)pDecoratorManager.loadDecorator(pSetting["model"]), tColor[0], tColor[1], tColor[2]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::QuadViewPerimeter::QuadViewPerimeter(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const QuadModel &pQuadModel,
	float pRed, float pGreen, float pBlue):
FiducialDecorator(pDecoratorManager, pMarker),
mQuadModel(pQuadModel),
mRed(pRed),
mGreen(pGreen),
mBlue(pBlue)
{
}

void decorators::QuadViewPerimeter::update() {
	if (mMarker->isPresent())
	{
		wykobi::quadix<float ,2> tMarkerCorners = mMarker->getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
		wykobi::point2d<float> tOrigin;
		wykobi::point2d<float> tXUnit;
		wykobi::point2d<float> tYUnit;
		FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

		wykobi::quadix<float ,2> tQuadCorners;
		for (int i = 0; i < 4; ++i) {
			tQuadCorners[i] = tOrigin + mQuadModel.getQuad()[i].x*tXUnit + mQuadModel.getQuad()[i].y*tYUnit;
		}

		mDecoratorManager.GetWorld2Proj().InterpolatedMapOnQuad(tQuadCorners);
		for (int i = 0; i < 4; ++i) {
			mDecoratorManager.GetDisplay().RenderLine(tQuadCorners[i].x, tQuadCorners[i].y, tQuadCorners[(i+1)%4].x, tQuadCorners[(i+1)%4].y, mRed, mGreen, mBlue);
		}
	}
}
