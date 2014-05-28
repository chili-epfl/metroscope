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

#include "RhombusPuzzle.hpp"
#include <iostream>

const std::string decorators::RhombusPuzzle::scDecoratorName("RhombusPuzzle");
const DecoratorManager::Registerer decorators::RhombusPuzzle::mRegisterer(decorators::RhombusPuzzle::scDecoratorName, &decorators::RhombusPuzzle::create);

namespace{

	/*inline bool pointIsInBox(wykobi::point2d<float> pPos, wykobi::point2d<float> pCorner1, wykobi::point2d<float> pCorner2,
			wykobi::point2d<float> pCorner3, wykobi::point2d<float> pCorner4){
		return false;
	}*/
}

decorators::FiducialDecorator *decorators::RhombusPuzzle::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try{
		libconfig::Setting & tPoint = pSetting["firstRhombus"];
		decorators::RhombusPuzzle *tRhombusPuzzle = new decorators::RhombusPuzzle(pDecoratorManager,
				pDecoratorManager.loadMarker(pSetting["marker"]),
				tPoint[0], tPoint[1]);

		libconfig::Setting & tPuzzleSolution = pSetting["puzzle"];
		libconfig::Setting & tPuzzleVisibility = pSetting["showed"];


		for(int i=0; i<tPuzzleSolution.getLength(); i++){
			tRhombusPuzzle->mExpectedContent.push_back(tPuzzleSolution[i]);
			//tRhombusPuzzle->mExpectedContent.push_back(pDecoratorManager.loadMarker(tPuzzleSolution[i]));
			//tRhombusPuzzle->mExpectedContent.insert(tPuzzleSolution[i]);
		}

		for(int i=0; i<tPuzzleVisibility.getLength(); i++){
			tRhombusPuzzle->mContentIsShown.push_back(tPuzzleVisibility[i]);
			//tRhombusPuzzle->mContentIsShown.insert((int)tPuzzleVisibility[i]);
		}
		return tRhombusPuzzle;
	} catch(libconfig::SettingNotFoundException &e) {
			std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
			std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}

	return 0;
}

decorators::RhombusPuzzle::RhombusPuzzle(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, float pTopBoxX, float pTopBoxY):
	FiducialDecorator(pDecoratorManager,pMarker)
{
	mRelativeBox0.x = pTopBoxX;
	mRelativeBox0.y = pTopBoxY;

	mRelativeBox1.x = pTopBoxX - 3.5f;
	mRelativeBox1.y = pTopBoxY - 3.5f;

	mRelativeBox2.x = pTopBoxX + 3.5f;
	mRelativeBox2.y = pTopBoxY - 3.5f;

	mRelativeBox3.x = pTopBoxX - 7.0f;
	mRelativeBox3.y = pTopBoxY - 7.0f;

	mRelativeBox4.x = pTopBoxX;
	mRelativeBox4.y = pTopBoxY - 7.0f;

	mRelativeBox5.x = pTopBoxX + 7.0f;
	mRelativeBox5.y = pTopBoxY - 7.0f;

	mRelativeBox6.x = pTopBoxX - 10.5f;
	mRelativeBox6.y = pTopBoxY - 10.5f;

	mRelativeBox7.x = pTopBoxX - 3.5f;
	mRelativeBox7.y = pTopBoxY - 10.5f;

	mRelativeBox8.x = pTopBoxX + 3.5f;
	mRelativeBox8.y = pTopBoxY - 10.5f;

	mRelativeBox9.x = pTopBoxX + 10.5f;
	mRelativeBox9.y = pTopBoxY - 10.5f;

	//Special points to draw:
	mRelativeBox6_2.x = mRelativeBox6.x - 3.5f;
	mRelativeBox6_2.y = mRelativeBox6.y - 3.5f;

	mRelativeBox6_3.x = mRelativeBox6.x;
	mRelativeBox6_3.y = mRelativeBox6.y - 7.0f;

	mRelativeBox7_2.x = mRelativeBox7.x;
	mRelativeBox7_2.y = mRelativeBox7.y - 7.0f;

	mRelativeBox8_2.x = mRelativeBox8.x;
	mRelativeBox8_2.y = mRelativeBox8.y - 7.0f;

	mRelativeBox9_2.x = mRelativeBox9.x;
	mRelativeBox9_2.y = mRelativeBox9.y - 7.0f;

	mRelativeBox9_3.x = mRelativeBox9.x + 3.5f;
	mRelativeBox9_3.y = mRelativeBox9.y - 3.5f;
}

void decorators::RhombusPuzzle::update()
{
	if(mMarker->isPresent())
	{
		wykobi::quadix<float, 2> tMarkerCorners = mMarker->getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
		wykobi::point2d<float> tOrigin;
		wykobi::point2d<float> tXUnit;
		wykobi::point2d<float> tYUnit;
		FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

		float am = 2.0f;

		mBox0 = am*(tOrigin + mRelativeBox0.x*tXUnit + mRelativeBox0.y*tYUnit);
		mBox1 = am*(tOrigin + mRelativeBox1.x*tXUnit + mRelativeBox1.y*tYUnit);
		mBox2 = am*(tOrigin + mRelativeBox2.x*tXUnit + mRelativeBox2.y*tYUnit);
		mBox3 = am*(tOrigin + mRelativeBox3.x*tXUnit + mRelativeBox3.y*tYUnit);
		mBox4 = am*(tOrigin + mRelativeBox4.x*tXUnit + mRelativeBox4.y*tYUnit);
		mBox5 = am*(tOrigin + mRelativeBox5.x*tXUnit + mRelativeBox5.y*tYUnit);
		mBox6 = am*(tOrigin + mRelativeBox6.x*tXUnit + mRelativeBox6.y*tYUnit);
		mBox7 = am*(tOrigin + mRelativeBox7.x*tXUnit + mRelativeBox7.y*tYUnit);
		mBox8 = am*(tOrigin + mRelativeBox8.x*tXUnit + mRelativeBox8.y*tYUnit);
		mBox9 = am*(tOrigin + mRelativeBox9.x*tXUnit + mRelativeBox9.y*tYUnit);
		mBox6_2 = am*(tOrigin + mRelativeBox6_2.x*tXUnit + mRelativeBox6_2.y*tYUnit);
		mBox6_3 = am*(tOrigin + mRelativeBox6_3.x*tXUnit + mRelativeBox6_3.y*tYUnit);
		mBox7_2 = am*(tOrigin + mRelativeBox7_2.x*tXUnit + mRelativeBox7_2.y*tYUnit);
		mBox8_2 = am*(tOrigin + mRelativeBox8_2.x*tXUnit + mRelativeBox8_2.y*tYUnit);
		mBox9_2 = am*(tOrigin + mRelativeBox9_2.x*tXUnit + mRelativeBox9_2.y*tYUnit);
		mBox9_3 = am*(tOrigin + mRelativeBox9_3.x*tXUnit + mRelativeBox9_3.y*tYUnit);

		mBox[0] = mBox0;
		mBox[1] = mBox1;
		mBox[2] = mBox2;
		mBox[3] = mBox3;
		mBox[4] = mBox4;
		mBox[5] = mBox5;
		mBox[6] = mBox6;
		mBox[7] = mBox7;
		mBox[8] = mBox8;
		mBox[9] = mBox9;

		drawPuzzle();
		checkAnswer();
	}
}

void decorators::RhombusPuzzle::drawPuzzle()
{
	mDecoratorManager.GetDisplay().PopTransformation();
	mDecoratorManager.GetDisplay().RenderLine(mBox0.x,mBox0.y,mBox6_2.x,mBox6_2.y,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(mBox0.x,mBox0.y,mBox9_3.x,mBox9_3.y,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(mBox1.x,mBox1.y,mBox9_2.x,mBox9_2.y,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(mBox3.x,mBox3.y,mBox8_2.x,mBox8_2.y,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(mBox6.x,mBox6.y,mBox7_2.x,mBox7_2.y,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(mBox2.x,mBox2.y,mBox6_3.x,mBox6_3.y,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(mBox5.x,mBox5.y,mBox7_2.x,mBox7_2.y,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(mBox9.x,mBox9.y,mBox8_2.x,mBox8_2.y,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(mBox9_3.x,mBox9_3.y,mBox9_2.x,mBox9_2.y,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(mBox6_3.x,mBox6_3.y,mBox6_2.x,mBox6_2.y,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().PushTransformation();

	for(unsigned int i = 0; i < mExpectedContent.size(); i++){
		if(mContentIsShown[i] == 1)
		{
			mDecoratorManager.GetDisplay().PushTransformation();
			char* mNumber= new char[2];
			sprintf (mNumber, "%d", mExpectedContent[i]);
			mDecoratorManager.GetDisplay().RenderText(mNumber,mBox[i].x,mBox[i].y,1.0f,0.0f,0.0f,0.0f,1.0f);
			mDecoratorManager.GetDisplay().PopTransformation();
		}
	}
}

void decorators::RhombusPuzzle::checkAnswer()
{
	//std::vector<NumberCard *> tActiveCards = mNumberModel->GetActiveCards();

}

