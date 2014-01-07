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

#include "Grid.hpp"
#include <iostream>

const float decorators::Grid::scInfiniteGridSize = 1000.0f;

const std::string decorators::Grid::scDecoratorName("Grid");
const  DecoratorManager::Registerer decorators::Grid::mRegisterer(decorators::Grid::scDecoratorName, &decorators::Grid::create);

decorators::FiducialDecorator *decorators::Grid::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tSize = pSetting["cell_size"];
		libconfig::Setting & tColor = pSetting["color"];
		if (pSetting.exists("grid_size"))
		{
			float tGridSize = pSetting["grid_size"];
			return new decorators::Grid(pDecoratorManager,
					pDecoratorManager.loadMarker(pSetting["marker"]),
					tColor[0], tColor[1], tColor[2],
					tSize[0], tSize[1],
					tGridSize);
		}
		return new decorators::Grid(
				pDecoratorManager,
				pDecoratorManager.loadMarker(pSetting["marker"]),
				tColor[0], tColor[1], tColor[2],
				tSize[0], tSize[1]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::Grid::Grid(DecoratorManager &pDecoratorManager, 
		FiducialMarker *pMarker,
		float pRed, float pGreen, float pBlue,
		float pCellWidth, float pCellHeight,
		float pGridSize):
FiducialDecorator(pDecoratorManager, pMarker),
mRed(pRed),
mGreen(pGreen),
mBlue(pBlue),
mCellWidth(pCellWidth),
mCellHeight(pCellHeight),
mHalfGridSize(pGridSize/2.0f)
{
}

void decorators::Grid::update()
{
	if (mMarker->isPresent())
	{

		wykobi::quadix<float, 2> tCorners = mMarker->getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMap(tCorners[0]);
		mDecoratorManager.GetCam2World().InterpolatedMap(tCorners[1]);
		mDecoratorManager.GetCam2World().InterpolatedMap(tCorners[2]);
		mDecoratorManager.GetCam2World().InterpolatedMap(tCorners[3]);
		wykobi::point2d<float> tCenter = 0.25f*(tCorners[0]+tCorners[1]+tCorners[2]+tCorners[3]);
		wykobi::point2d<float> tXUnit = 0.25f*(tCorners[1]-tCorners[3]+tCorners[2]-tCorners[0]);
		wykobi::point2d<float> tYUnit = 0.25f*(tCorners[1]-tCorners[3]+tCorners[0]-tCorners[2]);

		for (float tXOffset=0; tXOffset <= mHalfGridSize; tXOffset += mCellWidth) {
			for (float tYOffset=0; tYOffset <= mHalfGridSize; tYOffset += mCellHeight) {
				wykobi::point2d<float> tUR = tCenter + mHalfGridSize*tXUnit +      tYOffset*tYUnit;
				wykobi::point2d<float> tUL = tCenter - mHalfGridSize*tXUnit +      tYOffset*tYUnit;
				wykobi::point2d<float> tRU = tCenter +      tXOffset*tXUnit + mHalfGridSize*tYUnit;
				wykobi::point2d<float> tRD = tCenter +      tXOffset*tXUnit - mHalfGridSize*tYUnit;
				wykobi::point2d<float> tDR = tCenter + mHalfGridSize*tXUnit -      tYOffset*tYUnit;
				wykobi::point2d<float> tDL = tCenter - mHalfGridSize*tXUnit -      tYOffset*tYUnit;
				wykobi::point2d<float> tLU = tCenter -      tXOffset*tXUnit + mHalfGridSize*tYUnit;
				wykobi::point2d<float> tLD = tCenter -      tXOffset*tXUnit - mHalfGridSize*tYUnit;
				mDecoratorManager.GetWorld2Proj().InterpolatedMap(tUR);
				mDecoratorManager.GetWorld2Proj().InterpolatedMap(tUL);
				mDecoratorManager.GetWorld2Proj().InterpolatedMap(tRU);
				mDecoratorManager.GetWorld2Proj().InterpolatedMap(tRD);
				mDecoratorManager.GetWorld2Proj().InterpolatedMap(tDR);
				mDecoratorManager.GetWorld2Proj().InterpolatedMap(tDL);
				mDecoratorManager.GetWorld2Proj().InterpolatedMap(tLU);
				mDecoratorManager.GetWorld2Proj().InterpolatedMap(tLD);
				mDecoratorManager.GetDisplay().RenderLine(tUR.x, tUR.y, tUL.x, tUL.y, mRed, mGreen, mBlue);
				mDecoratorManager.GetDisplay().RenderLine(tRU.x, tRU.y, tRD.x, tRD.y, mRed, mGreen, mBlue);
				mDecoratorManager.GetDisplay().RenderLine(tDR.x, tDR.y, tDL.x, tDL.y, mRed, mGreen, mBlue);
				mDecoratorManager.GetDisplay().RenderLine(tLU.x, tLU.y, tLD.x, tLD.y, mRed, mGreen, mBlue);
			}
		}
	}
}
