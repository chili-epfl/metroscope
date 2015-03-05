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

#include "BattleshipAnalysisSheet.hpp"
#include <iostream>


#include <qa/utils/Geometry.hpp>
#include <qa/utils/CvWykobiBridge.hpp>


const std::string decorators::BattleshipAnalysisSheet::scDecoratorName("BattleshipAnalysisSheet");
const DecoratorManager::Registerer decorators::BattleshipAnalysisSheet::mRegisterer(decorators::BattleshipAnalysisSheet::scDecoratorName, &decorators::BattleshipAnalysisSheet::create);



decorators::FiducialDecorator *decorators::BattleshipAnalysisSheet::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tWorldSize = pSetting["worldSize"];
		libconfig::Setting & tOriginCoords = pSetting["initialOriginCoords"];
		libconfig::Setting & tOriginMM = pSetting["initialOriginMM"];
		BattleshipAnalysisSheet *tBattleshipAnalysisSheet = new decorators::BattleshipAnalysisSheet(
				pDecoratorManager,
				pDecoratorManager.loadMarker(pSetting["TLmarker"]),
				pDecoratorManager.loadMarker(pSetting["TRmarker"]),
				pDecoratorManager.loadMarker(pSetting["BRmarker"]),
				pDecoratorManager.loadMarker(pSetting["BLmarker"]),
				tWorldSize[0], tWorldSize[1],
				pSetting["team"], tOriginCoords[0], tOriginCoords[1],
								tOriginMM[0], tOriginMM[1]);

		return tBattleshipAnalysisSheet;
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::BattleshipAnalysisSheet::BattleshipAnalysisSheet(
		DecoratorManager &pDecoratorManager,
		const FiducialMarker *pTLMarker,
		const FiducialMarker *pTRMarker,
		const FiducialMarker *pBRMarker,
		const FiducialMarker *pBLMarker,
		float pWorldWidth,
		float pWorldHeight,
		int pTeam, float pOriginCoordsX, float pOriginCoordsY,
		float pOriginMMX, float pOriginMMY):
FiducialDecorator(pDecoratorManager, pTLMarker),
mTRMarker(pTRMarker),
mBRMarker(pBRMarker),
mBLMarker(pBLMarker),
mTeam(pTeam),
mAreaOfInterest()
{
	mAreaOfInterest.push_back(wykobi::make_point(0.0f, 0.0f));
	mAreaOfInterest.push_back(wykobi::make_point(pWorldWidth*10.0f*scPX2MM, 0.0f));
	mAreaOfInterest.push_back(wykobi::make_point(pWorldWidth*10.0f*scPX2MM, pWorldHeight*10.0f*scPX2MM));
	mAreaOfInterest.push_back(wykobi::make_point(0.0f, pWorldHeight*10.0f*scPX2MM));

	mInitialOriginCoords = wykobi::make_point(pOriginCoordsX,pOriginCoordsY);
	mInitialOriginMM = wykobi::make_point(pOriginMMX,pOriginMMY);
}


decorators::BattleshipAnalysisSheet::~BattleshipAnalysisSheet()
{
}

void decorators::BattleshipAnalysisSheet::update() {
	if (mMarker->isPresent()
			&& mTRMarker->isPresent()
			&& mBRMarker->isPresent()
			&& mBLMarker->isPresent())
	{

		DisplayGrid();

//		mDecoratorManager.GetDisplay().PushTransformation();
//		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarker, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
//		mDecoratorManager.GetDisplay().RenderQuad(mAreaOfInterest[0].x, mAreaOfInterest[0].y,
//				mAreaOfInterest[1].x, mAreaOfInterest[1].y, mAreaOfInterest[2].x, mAreaOfInterest[2].y,
//				mAreaOfInterest[3].x, mAreaOfInterest[3].y, 0.0f, 0.0f, 0.0f, 1.0f);
//		mDecoratorManager.GetDisplay().PopTransformation();
	}
}

void decorators::BattleshipAnalysisSheet::DisplayGrid() {

	float tWidthPX = mAreaOfInterest[1].x - mAreaOfInterest[0].x;
	float tHeightPX = mAreaOfInterest[3].y - mAreaOfInterest[0].y;

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarker, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());

	//We draw the axis, counting from the mm initial Origin
	mDecoratorManager.GetDisplay().RenderLine(mInitialOriginMM.x * tWidthPX, mInitialOriginMM.y * tHeightPX,
			mInitialOriginMM.x * tWidthPX + (-1)*mInitialOriginCoords.x*tWidthPX*scAxesOnSheet, mInitialOriginMM.y * tHeightPX,
			0.0f, 0.0f, 0.0f, 1.0f); //Horizontal line from team's origin to x=zero
	mDecoratorManager.GetDisplay().RenderLine(mInitialOriginMM.x * tWidthPX, mInitialOriginMM.y * tHeightPX,
			mInitialOriginMM.x * tWidthPX, mInitialOriginMM.y * tHeightPX + mInitialOriginCoords.y*tHeightPX*scAxesOnSheet,
			0.0f, 0.0f, 0.0f, 1.0f); //Vertical line from team's origin to y=zero
	mDecoratorManager.GetDisplay().RenderLine(mInitialOriginMM.x * tWidthPX + (-1)*mInitialOriginCoords.x*tWidthPX*scAxesOnSheet, mInitialOriginMM.y * tHeightPX,
			mInitialOriginMM.x * tWidthPX + (-1)*mInitialOriginCoords.x*tWidthPX, mInitialOriginMM.y * tHeightPX,
			0.0f, 0.0f, 0.0f, 0.7f); //Horizontal line from x=zero to end of sheet (lighter)
	mDecoratorManager.GetDisplay().RenderLine(mInitialOriginMM.x * tWidthPX, mInitialOriginMM.y * tHeightPX + mInitialOriginCoords.y*tHeightPX*scAxesOnSheet,
			mInitialOriginMM.x * tWidthPX, mInitialOriginMM.y * tHeightPX + mInitialOriginCoords.y*tHeightPX,
			0.0f, 0.0f, 0.0f, 0.7f); //Vertical line from y=zero to end of sheet (lighter)


//	mDecoratorManager.GetDisplay().RenderQuad(mAreaOfInterest[0].x, mAreaOfInterest[0].y,
//			mAreaOfInterest[1].x, mAreaOfInterest[1].y, mAreaOfInterest[2].x, mAreaOfInterest[2].y,
//			mAreaOfInterest[3].x, mAreaOfInterest[3].y, 0.0f, 0.0f, 0.0f, 1.0f);
	mDecoratorManager.GetDisplay().PopTransformation();

}
