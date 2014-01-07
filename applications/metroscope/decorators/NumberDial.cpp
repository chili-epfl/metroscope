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

#include "NumberDial.hpp"
#include <iostream>
#include <cmath>
//#include "../FractionsConstants.hpp"


const std::string decorators::NumberDial::scDecoratorName("NumberDial");
const  DecoratorManager::Registerer decorators::NumberDial::mRegisterer(decorators::NumberDial::scDecoratorName, &decorators::NumberDial::create);

decorators::FiducialDecorator *decorators::NumberDial::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::NumberDial(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),
										pDecoratorManager.loadMarker(pSetting["stabilized_graphics_marker"]),
										-30, 60, 18, 0, &scBLACK);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::NumberDial::NumberDial(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker,
									FiducialMarker *pStabilizedGraphicsMarker,
									const int pMin,
									const int pRange,
									const int pMaxNumbersInCircle,
									const int pStartNumber,
									const color *pHighlightColor
									):
FiducialDecorator(pDecoratorManager, pMarker),
mStabilizedGraphicsMarker(pStabilizedGraphicsMarker),
mMin(pMin),
mRange(pRange),
mNumbersInCircle(mRange > pMaxNumbersInCircle? pMaxNumbersInCircle: mRange),
mNotchesFromTop(0),
mHighlightColor(pHighlightColor),
mCurrentNumber(pStartNumber)
{
}

decorators::NumberDial::~NumberDial(){/*Empty*/}


void decorators::NumberDial::update() {
	if (mMarker->isPresent())
	{
		float tAngle = mMarker->getAngle();
		updateValue(tAngle);
		displayDialNumbers(tAngle);
	}
}


void decorators::NumberDial::updateValue(float pAngle){
	float tDeltaAngle = pAngle - mNotchesFromTop*scDIAL_DEGREES_PER_NOTCH;
	if (tDeltaAngle > scDEGREES_IN_CIRCLE/2) tDeltaAngle -= scDEGREES_IN_CIRCLE;
	else if (tDeltaAngle < -scDEGREES_IN_CIRCLE/2) tDeltaAngle += scDEGREES_IN_CIRCLE;

	int tOrientation = tDeltaAngle < 0? -1: 1;

	int tUpdatedNotchesFromTop = (int)(fmodulo(pAngle +.5*scDIAL_DEGREES_PER_NOTCH, scDEGREES_IN_CIRCLE)/scDIAL_DEGREES_PER_NOTCH);
	int tDeltaNumber = tOrientation*modulo(tOrientation*(tUpdatedNotchesFromTop-mNotchesFromTop), mNumbersInCircle);
	mCurrentNumber = modulo(mCurrentNumber + tDeltaNumber - mMin, mRange) + mMin;
	mNotchesFromTop = tUpdatedNotchesFromTop;
}


void decorators::NumberDial::displayDialNumbers(float pAngle){

	wykobi::point2d<float> tCenter = mStabilizedGraphicsMarker->getCenter();
	mDecoratorManager.GetCam2World().InterpolatedMap(tCenter);
	mDecoratorManager.GetWorld2Proj().InterpolatedMap(tCenter);

	drawDialSelectorBox(tCenter, pAngle);

	int tNumbersLeftOfSelected = mNumbersInCircle/2;
	int tNumbersRightOfSelected = mNumbersInCircle%2 == 0 ? mNumbersInCircle/2: (mNumbersInCircle+1)/2;

	float tDeltaFromNotch = pAngle - mNotchesFromTop*scDIAL_DEGREES_PER_NOTCH;
	if(tDeltaFromNotch>scDIAL_DEGREES_PER_NOTCH/2) tDeltaFromNotch -= scDEGREES_IN_CIRCLE;

	for(int i = -tNumbersLeftOfSelected; i <= tNumbersRightOfSelected; i++){
		char tNumberText[scMAX_DIGITS_IN_NUMERATOR_OR_DENOMINATOR];
		sprintf(tNumberText, "%d", mMin + modulo(mCurrentNumber + i - mMin, mRange));
		float tPhi = -fmod((mNotchesFromTop + i)*scDIAL_DEGREES_PER_NOTCH, scDEGREES_IN_CIRCLE);
		mDecoratorManager.GetDisplay().PushTransformation();
				mDecoratorManager.GetDisplay().Rotate(tPhi, tCenter.x, tCenter.y);
				if (i == 0) {
					mDecoratorManager.GetDisplay().RenderCenteredText(tNumberText, tCenter.x, tCenter.y - scDIAL_RADIUS,
												true, scDIAL_SELECTED_NUMBER_SCALE,
												mHighlightColor->r, mHighlightColor->g, mHighlightColor->b, mHighlightColor->a);
				}

				else {
					float tDeltaFromSelector = i<0? std::abs(i)*scDIAL_DEGREES_PER_NOTCH + tDeltaFromNotch: std::abs(i)*scDIAL_DEGREES_PER_NOTCH - tDeltaFromNotch;
					float tScale = scDIAL_NUMBERS_MAX_SCALE*(scDEGREES_IN_CIRCLE)/scDEGREES_IN_CIRCLE;
					mDecoratorManager.GetDisplay().RenderCenteredText(tNumberText, tCenter.x, tCenter.y - scDIAL_RADIUS,
												true, tScale,
												scDIAL_LOWLIGHT_COLOR->r, scDIAL_LOWLIGHT_COLOR->g, scDIAL_LOWLIGHT_COLOR->b,
												scDIAL_LOWLIGHT_COLOR->a - (scsDIAL_TRANSPARENCY_GROWTH_RATE*tDeltaFromSelector)/scDEGREES_IN_CIRCLE);
				}
			mDecoratorManager.GetDisplay().PopTransformation();
	}
}

//Create the grey box under the selected number
void decorators::NumberDial::drawDialSelectorBox(wykobi::point2d<float> tCenter, float pAngle){
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().Translate(tCenter.x, tCenter.y);
	mDecoratorManager.GetDisplay().Rotate(-pAngle, 0.0, 0.0);

	mDecoratorManager.GetDisplay().RenderFilledSector(0, 0, scDIAL_RADIUS + scDIAL_HIGHTLIGHTED_NUMBER_BOX_HEIGHT/2, scDIAL_RADIUS + scDIAL_HIGHTLIGHTED_NUMBER_BOX_HEIGHT/2,
							scDIAL_DEGREES_PER_NOTCH, scDEGREES_IN_CIRCLE/4-scDIAL_DEGREES_PER_NOTCH/2,
							scLIGHT_GREY.r, scLIGHT_GREY.g, scLIGHT_GREY.b, 0.5f, 1);
	mDecoratorManager.GetDisplay().RenderFilledSector(0, 0, scDIAL_RADIUS - scDIAL_HIGHTLIGHTED_NUMBER_BOX_HEIGHT/2, scDIAL_RADIUS - scDIAL_HIGHTLIGHTED_NUMBER_BOX_HEIGHT/2,
									scDIAL_DEGREES_PER_NOTCH, scDEGREES_IN_CIRCLE/4-scDIAL_DEGREES_PER_NOTCH/2,
									scWHITE.r, scWHITE.g, scWHITE.b, scWHITE.a, 1);
	mDecoratorManager.GetDisplay().PopTransformation();


}

int decorators::NumberDial::modulo(int x, int y){
	return (x%y + y)%y;
}

float decorators::NumberDial::fmodulo(float x, float y){
	float fmoduloTemp = std::fmod(x, y) + y;
	return std::fmod(fmoduloTemp,y);
}
