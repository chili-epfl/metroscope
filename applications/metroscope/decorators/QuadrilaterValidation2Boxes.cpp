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

#include "QuadrilaterValidation2Boxes.hpp"
#include <iostream>

const std::string decorators::QuadrilaterValidation2Boxes::scDecoratorName("QuadrilaterValidation2Boxes");
const  DecoratorManager::Registerer decorators::QuadrilaterValidation2Boxes::mRegisterer(decorators::QuadrilaterValidation2Boxes::scDecoratorName, &decorators::QuadrilaterValidation2Boxes::create);

namespace {
	enum {INCOMPLETE = -2, WRONG = -1, NONE = 0};

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

decorators::FiducialDecorator *decorators::QuadrilaterValidation2Boxes::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tBoxes = pSetting["boxes"];
		decorators::QuadrilaterValidation2Boxes *tQuadrilaterValidation2Boxes = new decorators::QuadrilaterValidation2Boxes(
				pDecoratorManager,
				pDecoratorManager.loadMarker(pSetting["marker"]),
				tBoxes[0], tBoxes[1], tBoxes[2], tBoxes[3],
				tBoxes[4], tBoxes[5], tBoxes[6], tBoxes[7]);
		libconfig::Setting & tContent1 = pSetting["content1"];
		for (int i=0; i<tContent1.getLength(); ++i)
		{
			tQuadrilaterValidation2Boxes->mExpectedContent1.insert(pDecoratorManager.loadMarker(tContent1[i]));
		}
		libconfig::Setting & tContent2 = pSetting["content2"];
		for (int i=0; i<tContent2.getLength(); ++i)
		{
			tQuadrilaterValidation2Boxes->mExpectedContent2.insert(pDecoratorManager.loadMarker(tContent2[i]));
		}
		libconfig::Setting & tMessage = pSetting["message"];
		for (int i=0; i<tMessage.getLength(); ++i)
		{
			tQuadrilaterValidation2Boxes->mMessage.push_back(tMessage[i]);
		}
		return tQuadrilaterValidation2Boxes;
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::QuadrilaterValidation2Boxes::QuadrilaterValidation2Boxes(
		DecoratorManager &pDecoratorManager,
		FiducialMarker *pMarker,
		float pBox1x1, float pBox1y1, float pBox1x2, float pBox1y2,
		float pBox2x1,float pBox2y1,float pBox2x2,float pBox2y2):
FiducialDecorator(pDecoratorManager, pMarker),
mRelativeBox1x1(pBox1x1), mRelativeBox1y1(pBox1y1), mRelativeBox1x2(pBox1x2), mRelativeBox1y2(pBox1y2),
mRelativeBox2x1(pBox2x1), mRelativeBox2y1(pBox2y1), mRelativeBox2x2(pBox2x2), mRelativeBox2y2(pBox2y2)
{
}

void decorators::QuadrilaterValidation2Boxes::update()
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
		char tBoxNumberOfContent1 = findBoxAssociatedToContent(mExpectedContent1);
		char tBoxNumberOfContent2 = findBoxAssociatedToContent(mExpectedContent2);

		wykobi::point2d<float> tTextPosition = tOrigin + scTextXPos*tXUnit + scTextYPos*tYUnit;
		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tTextPosition);
		if (tBoxNumberOfContent1 == INCOMPLETE || tBoxNumberOfContent2 == INCOMPLETE)
		{
			char tText[40];
			sprintf(tText, "Place les %d formes de l'exercice.", (int) (mExpectedContent1.size()+mExpectedContent2.size()));
			mDecoratorManager.GetDisplay().RenderText(tText, tTextPosition.x, tTextPosition.y);
		} else if (tBoxNumberOfContent1 == WRONG || tBoxNumberOfContent2 == WRONG)
		{
			mDecoratorManager.GetDisplay().RenderText("Tu peux faire autre chose.", tTextPosition.x, tTextPosition.y);
			mDecoratorManager.GetDisplay().RenderText("Utilise les cartes d'aide!", tTextPosition.x, tTextPosition.y + 20.0f);
		} else if (tBoxNumberOfContent1 != tBoxNumberOfContent2)
		{
			for (std::list<std::string>::iterator it = mMessage.begin(); it != mMessage.end(); ++it)
			{
				mDecoratorManager.GetDisplay().RenderText((*it).c_str(), tTextPosition.x, tTextPosition.y);
				tTextPosition.y += 20.0f;
			}
		}
	}
}

char decorators::QuadrilaterValidation2Boxes::findBoxAssociatedToContent(std::set<FiducialMarker *> &pExpectedContent)
{
	char tBoxNumberOfContent = NONE;
	for (std::set<FiducialMarker *>::iterator it = pExpectedContent.begin(); it != pExpectedContent.end(); ++it)
	{
		if (true || (*it)->isPresent())
		{
			wykobi::point2d<float> tCenter = (*it)->getCenter();
			mDecoratorManager.GetCam2World().InterpolatedMap(tCenter);
			char tBoxNumber = findBox(tCenter);
			if (tBoxNumber == NONE) return INCOMPLETE;
			if (!tBoxNumberOfContent) tBoxNumberOfContent = tBoxNumber;
			else if (tBoxNumberOfContent != tBoxNumber) return WRONG;
		}
		else return INCOMPLETE;
	}
	return tBoxNumberOfContent;
}

char decorators::QuadrilaterValidation2Boxes::findBox(wykobi::point2d<float> pPos)
{
	if (pointIsInBox(pPos, mBox1P1, mBox1P2)) return 1;
	if (pointIsInBox(pPos, mBox2P1, mBox2P2)) return 2;
	return NONE;
}
