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

#include "QuadrilaterValidation3Boxes.hpp"
#include <iostream>

const std::string decorators::QuadrilaterValidation3Boxes::scDecoratorName("QuadrilaterValidation3Boxes");
const  DecoratorManager::Registerer decorators::QuadrilaterValidation3Boxes::mRegisterer(decorators::QuadrilaterValidation3Boxes::scDecoratorName, &decorators::QuadrilaterValidation3Boxes::create);

namespace {

	inline bool pointIsInBox(wykobi::point2d<float> pPos, wykobi::point2d<float> pCorner1, wykobi::point2d<float> pCorner2)
	{
		return (pCorner1.x <= pPos.x && pPos.x <= pCorner2.x && pCorner1.y <= pPos.y && pPos.y <= pCorner2.y)
			|| (pCorner2.x <= pPos.x && pPos.x <= pCorner1.x && pCorner1.y <= pPos.y && pPos.y <= pCorner2.y)
			|| (pCorner1.x <= pPos.x && pPos.x <= pCorner2.x && pCorner2.y <= pPos.y && pPos.y <= pCorner1.y)
			|| (pCorner2.x <= pPos.x && pPos.x <= pCorner1.x && pCorner2.y <= pPos.y && pPos.y <= pCorner1.y);
	}

	static const float scTextXPos = 12.0f;
	static const float scTextYPos = 0.0f;
}

decorators::FiducialDecorator *decorators::QuadrilaterValidation3Boxes::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tBoxes = pSetting["boxes"];
		decorators::QuadrilaterValidation3Boxes *tQuadrilaterValidation3Boxes = new decorators::QuadrilaterValidation3Boxes(
				pDecoratorManager,
				pDecoratorManager.loadMarker(pSetting["marker"]),
				pDecoratorManager.loadMarker(pSetting["square"]),
				pDecoratorManager.loadMarker(pSetting["diamond"]),
				pDecoratorManager.loadMarker(pSetting["rectangle"]),
				tBoxes[0], tBoxes[1], tBoxes[2], tBoxes[3],
				tBoxes[4], tBoxes[5], tBoxes[6], tBoxes[7],
				tBoxes[8], tBoxes[9], tBoxes[10], tBoxes[11]);
		return tQuadrilaterValidation3Boxes;
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::QuadrilaterValidation3Boxes::QuadrilaterValidation3Boxes(
		DecoratorManager &pDecoratorManager,
		FiducialMarker *pMarker,
		FiducialMarker *pSquare,
		FiducialMarker *pDiamond,
		FiducialMarker *pRectangle,
		float pBox1x1, float pBox1y1, float pBox1x2, float pBox1y2,
		float pBox2x1,float pBox2y1,float pBox2x2,float pBox2y2,
		float pBox3x1,float pBox3y1,float pBox3x2,float pBox3y2):
FiducialDecorator(pDecoratorManager, pMarker),
mSquare(pSquare),
mDiamond(pDiamond),
mRectangle(pRectangle),
mRelativeBox1x1(pBox1x1), mRelativeBox1y1(pBox1y1), mRelativeBox1x2(pBox1x2), mRelativeBox1y2(pBox1y2),
mRelativeBox2x1(pBox2x1), mRelativeBox2y1(pBox2y1), mRelativeBox2x2(pBox2x2), mRelativeBox2y2(pBox2y2),
mRelativeBox3x1(pBox3x1), mRelativeBox3y1(pBox3y1), mRelativeBox3x2(pBox3x2), mRelativeBox3y2(pBox3y2)
{
}

void decorators::QuadrilaterValidation3Boxes::update()
{
	if (mMarker->isPresent())
	{
		wykobi::quadix<float ,2> tMarkerCorners = mMarker->getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
		wykobi::point2d<float> tOrigin;
		wykobi::point2d<float> tXUnit;
		wykobi::point2d<float> tYUnit;
		FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

		mBox1P1 = tOrigin+mRelativeBox1x1*tXUnit+mRelativeBox1y1*tYUnit;
		mBox1P2 = tOrigin+mRelativeBox1x2*tXUnit+mRelativeBox1y2*tYUnit;
		mBox2P1 = tOrigin+mRelativeBox2x1*tXUnit+mRelativeBox2y1*tYUnit;
		mBox2P2 = tOrigin+mRelativeBox2x2*tXUnit+mRelativeBox2y2*tYUnit;
		mBox3P1 = tOrigin+mRelativeBox3x1*tXUnit+mRelativeBox3y1*tYUnit;
		mBox3P2 = tOrigin+mRelativeBox3x2*tXUnit+mRelativeBox3y2*tYUnit;

		wykobi::point2d<float> tCenter = mSquare->getCenter();
		mDecoratorManager.GetCam2World().InterpolatedMap(tCenter);
		char tSquareBox = findBox(tCenter);
		tCenter = mDiamond->getCenter();
		mDecoratorManager.GetCam2World().InterpolatedMap(tCenter);
		char tDiamondBox = findBox(tCenter);
		tCenter = mRectangle->getCenter();
		mDecoratorManager.GetCam2World().InterpolatedMap(tCenter);
		char tRectangleBox = findBox(tCenter);

		wykobi::point2d<float> tTextPosition = tOrigin + scTextXPos*tXUnit + scTextYPos*tYUnit;
		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tTextPosition);
		if (!tSquareBox || !tRectangleBox || !tDiamondBox)
		{
			mDecoratorManager.GetDisplay().RenderText("Place les 3 formes de l'exercice.", tTextPosition.x, tTextPosition.y);
		} else if (tRectangleBox != 1)
		{
			mDecoratorManager.GetDisplay().RenderText("Quelle est la propriété du rectangle?", tTextPosition.x, tTextPosition.y);
		} else if (tDiamondBox != 3) {
			mDecoratorManager.GetDisplay().RenderText("Quelle est la propriété du losange?", tTextPosition.x, tTextPosition.y);
		} else if (tSquareBox == 1) {
			mDecoratorManager.GetDisplay().RenderText("Mais le carré a aussi", tTextPosition.x, tTextPosition.y);
			mDecoratorManager.GetDisplay().RenderText("4 côtés isométriques...", tTextPosition.x, tTextPosition.y+20.0f);
		}  else if (tSquareBox == 3) {
			mDecoratorManager.GetDisplay().RenderText("Mais le carré a aussi ", tTextPosition.x, tTextPosition.y);
			mDecoratorManager.GetDisplay().RenderText("4 angles droits...", tTextPosition.x, tTextPosition.y+20.0f);
		} else {
			mDecoratorManager.GetDisplay().RenderText("Bravo, les carrés ont à la fois", tTextPosition.x, tTextPosition.y);
			mDecoratorManager.GetDisplay().RenderText("4 côtes isométriques et", tTextPosition.x, tTextPosition.y+20.0f);
			mDecoratorManager.GetDisplay().RenderText("4 angles droits.", tTextPosition.x, tTextPosition.y+40.0f);
			mDecoratorManager.GetDisplay().RenderText("", tTextPosition.x, tTextPosition.y+60.0f);
			mDecoratorManager.GetDisplay().RenderText("Fais l'exercice de la page 7.", tTextPosition.x, tTextPosition.y+80.0f);
		}
	}
}

char decorators::QuadrilaterValidation3Boxes::findBox(wykobi::point2d<float> pPos)
{
	if (pointIsInBox(pPos, mBox1P1, mBox1P2)) return 1;
	if (pointIsInBox(pPos, mBox2P1, mBox2P2)) return 2;
	if (pointIsInBox(pPos, mBox3P1, mBox3P2)) return 3;
	return 0;
}
