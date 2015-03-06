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
	mAreaOfInterest.push_back(wykobi::make_point(pWorldWidth*10.0f*scPX2MM, 0.0f));//In MM, from the top left of the area of interest
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
			0.0f, 0.0f, 0.0f, 0.5f); //Horizontal line from x=zero to end of sheet (lighter)
	mDecoratorManager.GetDisplay().RenderLine(mInitialOriginMM.x * tWidthPX, mInitialOriginMM.y * tHeightPX + mInitialOriginCoords.y*tHeightPX*scAxesOnSheet,
			mInitialOriginMM.x * tWidthPX, mInitialOriginMM.y * tHeightPX + mInitialOriginCoords.y*tHeightPX,
			0.0f, 0.0f, 0.0f, 0.5f); //Vertical line from y=zero to end of sheet (lighter)

	mDecoratorManager.GetDisplay().RenderQuadFilled((mInitialOriginMM.x * tWidthPX)-1  + (-1)*mInitialOriginCoords.x*tWidthPX*scAxesOnSheet, mInitialOriginMM.y * tHeightPX,
			(mInitialOriginMM.x * tWidthPX)+1  + (-1)*mInitialOriginCoords.x*tWidthPX*scAxesOnSheet, mInitialOriginMM.y * tHeightPX,
			(mInitialOriginMM.x * tWidthPX)+1  + (-1)*mInitialOriginCoords.x*tWidthPX*scAxesOnSheet, mInitialOriginMM.y * tHeightPX + mInitialOriginCoords.y*tHeightPX,
			(mInitialOriginMM.x * tWidthPX)-1  + (-1)*mInitialOriginCoords.x*tWidthPX*scAxesOnSheet, mInitialOriginMM.y * tHeightPX + mInitialOriginCoords.y*tHeightPX,
			0.0f, 0.0f, 0.0f, 1.0f); //Vertical line x=zero (thicker)
	mDecoratorManager.GetDisplay().RenderQuadFilled(mInitialOriginMM.x * tWidthPX, mInitialOriginMM.y * tHeightPX + mInitialOriginCoords.y*tHeightPX*scAxesOnSheet -1,
			mInitialOriginMM.x * tWidthPX + (-1)*mInitialOriginCoords.x*tWidthPX, mInitialOriginMM.y * tHeightPX + mInitialOriginCoords.y*tHeightPX*scAxesOnSheet -1,
			mInitialOriginMM.x * tWidthPX + (-1)*mInitialOriginCoords.x*tWidthPX, mInitialOriginMM.y * tHeightPX + mInitialOriginCoords.y*tHeightPX*scAxesOnSheet +1,
			mInitialOriginMM.x * tWidthPX, mInitialOriginMM.y * tHeightPX + mInitialOriginCoords.y*tHeightPX*scAxesOnSheet +1,
			0.0f, 0.0f, 0.0f, 1.0f); //Horizontal line y=zero (thicker)

	for (int i=1; i< 10*(1+(1-scAxesOnSheet)/scAxesOnSheet); ++i){

		if(i==10) continue; //This is actually the origin
		else if(i<10){//the first 9 lines are darker
			mDecoratorManager.GetDisplay().RenderLine(mInitialOriginMM.x * tWidthPX, mInitialOriginMM.y * tHeightPX + 0.1*i*tHeightPX*scAxesOnSheet*mInitialOriginCoords.y,
					mInitialOriginMM.x * tWidthPX + (-1)*mInitialOriginCoords.x*tWidthPX, mInitialOriginMM.y * tHeightPX + 0.1*i*tHeightPX*scAxesOnSheet*mInitialOriginCoords.y,
					0.0f, 0.0f, 0.0f, 0.2f); //Horizontal 0.1 lines from team's origin to x=zero
			mDecoratorManager.GetDisplay().RenderLine(mInitialOriginMM.x * tWidthPX + (-1)*0.1*i*tWidthPX*scAxesOnSheet*mInitialOriginCoords.x, mInitialOriginMM.y * tHeightPX,
					mInitialOriginMM.x * tWidthPX + (-1)*0.1*i*tWidthPX*scAxesOnSheet*mInitialOriginCoords.x, mInitialOriginMM.y * tHeightPX + mInitialOriginCoords.y*tHeightPX,
					0.0f, 0.0f, 0.0f, 0.2f); //Vertical line from team's origin to y=zero
		}
		else{
			mDecoratorManager.GetDisplay().RenderLine(mInitialOriginMM.x * tWidthPX, mInitialOriginMM.y * tHeightPX + 0.1*i*tHeightPX*scAxesOnSheet*mInitialOriginCoords.y,
					mInitialOriginMM.x * tWidthPX + (-1)*mInitialOriginCoords.x*tWidthPX, mInitialOriginMM.y * tHeightPX + 0.1*i*tHeightPX*scAxesOnSheet*mInitialOriginCoords.y,
					0.0f, 0.0f, 0.0f, 0.1f); //Horizontal 0.1 lines from team's origin to x=zero
			mDecoratorManager.GetDisplay().RenderLine(mInitialOriginMM.x * tWidthPX + (-1)*0.1*i*tWidthPX*scAxesOnSheet*mInitialOriginCoords.x, mInitialOriginMM.y * tHeightPX,
					mInitialOriginMM.x * tWidthPX + (-1)*0.1*i*tWidthPX*scAxesOnSheet*mInitialOriginCoords.x, mInitialOriginMM.y * tHeightPX + mInitialOriginCoords.y*tHeightPX,
					0.0f, 0.0f, 0.0f, 0.1f); //Vertical line from team's origin to y=zero
		}

	}


	//We mark the origin
	//TODO: this should be updated with the last move of the team from the network!
	//mDecoratorManager.GetDisplay().RenderEllipse(mInitialOriginMM.x * tWidthPX, mInitialOriginMM.y * tHeightPX,
	//		15.0f, 15.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	//To test, we plot the circle at -0.1, 0.1
	wykobi::point2d<float> tOriginMarkCoords = wykobi::make_point(-0.1f, 0.1f);
	wykobi::point2d<float> tOriginMarkMM = ConvertCoords2MM(tOriginMarkCoords);
	mDecoratorManager.GetDisplay().RenderEllipse(tOriginMarkMM.x, tOriginMarkMM.y,
				15.0f, 15.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	mDecoratorManager.GetDisplay().PopTransformation();

}

//Converts a set of coordinates in the -1,1 range, to MM in the sheet (0,width/height of AOI), for EVERY team taking into account they start in different places the display
wykobi::point2d<float> decorators::BattleshipAnalysisSheet::ConvertCoords2MM(wykobi::point2d<float> pCoords){

	float MMx = 0;
	float MMy = 0;

	float tWidthMM = mAreaOfInterest[1].x - mAreaOfInterest[0].x; //This is the length in MM of the area of interest (partial view of the coordinate plane)
	float tHeightMM = mAreaOfInterest[3].y - mAreaOfInterest[0].y;

	float tWidthPlaneMM = tWidthMM*2.0f*scAxesOnSheet;//Length of the whole coordinate plane (beyond our area of interest)
	float tHeightPlaneMM = tHeightMM*2.0f*scAxesOnSheet;

	//Transformation coefficients for the two coordinate systems as per https://msdn.microsoft.com/en-us/library/ie/jj635757(v=vs.85).aspx
	//This transformation gets game coordinates (-1,1) and transforms it into another that goes (0,widthplane|heightplane)
	// a = Wp+Hp / 4 ; b = -Wp+Hp / 4 ; c = Wp/2 ; d = Hp/2
	//x' = a*x+b*y+c ; y' = b*x-a*y+d
	MMx = ((tWidthPlaneMM+tHeightPlaneMM)/4.0f)*pCoords.x + ((-tWidthPlaneMM+tHeightPlaneMM)/4.0f)*pCoords.y + tWidthPlaneMM/2.0f;
	MMy =  ((-tWidthPlaneMM+tHeightPlaneMM)/4.0f)*pCoords.x - ((tWidthPlaneMM+tHeightPlaneMM)/4.0f)*pCoords.y + tHeightPlaneMM/2.0f;

	std::cout << "Partial conversion to pixels of " << pCoords.x << "," << pCoords.y << ": " << MMx << "," << MMy << std::endl;

	//We take into account the offset introduced by the fact that the different sheets have different displacement/view of the whole plane
	MMx = MMx - mInitialOriginMM.x*(tWidthPlaneMM-tWidthMM);
	MMy = MMy - mInitialOriginMM.y*(tHeightPlaneMM-tHeightMM);

	std::cout << "Complete conversion to pixels of " << pCoords.x << "," << pCoords.y << ": " << MMx << "," << MMy << std::endl;

	wykobi::point2d<float> newMM = wykobi::make_point(MMx,MMy);
	return newMM;

}

