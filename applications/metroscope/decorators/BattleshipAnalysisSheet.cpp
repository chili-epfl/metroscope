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
		BattleshipAnalysisSheet *tBattleshipAnalysisSheet = new decorators::BattleshipAnalysisSheet(
				pDecoratorManager,
				pDecoratorManager.loadMarker(pSetting["TLmarker"]),
				pDecoratorManager.loadMarker(pSetting["TRmarker"]),
				pDecoratorManager.loadMarker(pSetting["BRmarker"]),
				pDecoratorManager.loadMarker(pSetting["BLmarker"]),
				tWorldSize[0], tWorldSize[1]);

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
		float pWorldHeight):
FiducialDecorator(pDecoratorManager, pTLMarker),
mTRMarker(pTRMarker),
mBRMarker(pBRMarker),
mBLMarker(pBLMarker),
mAreaOfInterest(),
mBoxOfInterest(),
mNormalizedAreaOfInterest(),
mMirroredAreasOfInterest(),
mSelection(),
mLocalSelection()
{
	mAreaOfInterest.push_back(wykobi::make_point(-pWorldWidth/2.0f, -pWorldHeight/2.0f));
	mAreaOfInterest.push_back(wykobi::make_point( pWorldWidth/2.0f, -pWorldHeight/2.0f));
	mAreaOfInterest.push_back(wykobi::make_point( pWorldWidth/2.0f,  pWorldHeight/2.0f));
	mAreaOfInterest.push_back(wykobi::make_point(-pWorldWidth/2.0f,  pWorldHeight/2.0f));

	mBoxOfInterest[0] = mAreaOfInterest[0];
	mBoxOfInterest[1] = mAreaOfInterest[2];

	mLocalSelection[0] = mAreaOfInterest[0];
	mLocalSelection[1] = mAreaOfInterest[1];
	mLocalSelection[2] = mAreaOfInterest[2];
	mLocalSelection[3] = mAreaOfInterest[3];

	static const float scMM2Px = 32.0f;

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
		mSelection[0] = mMarker->getCenter();
		mSelection[1] = mTRMarker->getCenter();
		mSelection[2] = mBRMarker->getCenter();
		mSelection[3] = mBLMarker->getCenter();

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinatesFixed(*mMarker, scREAL_WORLD_MARKER_WIDTH_MM, scREAL_WORLD_MARKER_HEIGHT_MM, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
		mDecoratorManager.GetDisplay().RenderQuad(mMarker->getCenter().x, mMarker->getCenter().y,
				mTRMarker->getCenter().x, mTRMarker->getCenter().y, mBRMarker->getCenter().x, mBRMarker->getCenter().y,
				mBLMarker->getCenter().x, mBLMarker->getCenter().y, 0.0f, 0.0f, 0.0f, 1.0f);
		mDecoratorManager.GetDisplay().PopTransformation();
	}
}
