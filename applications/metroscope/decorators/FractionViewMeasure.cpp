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

#include "FractionViewMeasure.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>

const std::string decorators::FractionViewMeasure::scDecoratorName("FractionViewMeasure");
const DecoratorManager::Registerer decorators::FractionViewMeasure::mRegisterer(decorators::FractionViewMeasure::scDecoratorName, &decorators::FractionViewMeasure::create);

decorators::FiducialDecorator *decorators::FractionViewMeasure::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager){
	try{



			// FractionViewMeasure (DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, CircularFractionModel *pAngleModel1, CircularFractionModel *pAngleModel2,
		//	RectangleFractionModel *pRectangleModel1, RectangleFractionModel *pRectangleModel2, TokenModel *pTokenModel1, FractionCard ** pFractionCards);
			return new decorators::FractionViewMeasure (pDecoratorManager, (FiducialMarker *) pDecoratorManager.loadMarker(pSetting["marker"]),
					(CircularFractionModel *) pDecoratorManager.loadDecorator(pSetting["circular_1"]),
					(CircularFractionModel *) pDecoratorManager.loadDecorator(pSetting["circular_2"]),
					(RectangleFractionModel *) pDecoratorManager.loadDecorator(pSetting["rectangle_1"]),
					(RectangleFractionModel *) pDecoratorManager.loadDecorator(pSetting["rectangle_2"]),
					(TokenModel *) pDecoratorManager.loadDecorator(pSetting["token_1"]),
					(int) pSetting["exp_num"], (int) pSetting["exp_denom"]);

		}catch(libconfig::SettingNotFoundException &e) {
			std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
		} catch(libconfig::SettingTypeException &e) {
			std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
		}
		return 0;
}

decorators::FractionViewMeasure::FractionViewMeasure(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, CircularFractionModel *pAngleModel1,
		CircularFractionModel *pAngleModel2, RectangleFractionModel *pRectangleModel1, RectangleFractionModel *pRectangleModel2,
		TokenModel *pTokenModel1, int pExpectedNumerator, int pExpectedDenominator):
			FiducialDecorator(pDecoratorManager, pMarker),
			mAngleModel1(pAngleModel1), mAngleModel2(pAngleModel2),
			mRectangleModel1(pRectangleModel1), mRectangleModel2(pRectangleModel2),
			mTokenModel(pTokenModel1), mExpectedNumerator(pExpectedNumerator), mExpectedDenominator(pExpectedDenominator){
}


void decorators::FractionViewMeasure::update(){
	if(mMarker->isPresent()){
		int tNumerator = -1;
		int tDenominator = -1;
		float tRed;
		float tGreen;

		if(mAngleModel1->isPresent()){
			tNumerator = mAngleModel1->Numerator();
			tDenominator = mAngleModel1->Denominator();
		}else if (mAngleModel2->isPresent()){
			tNumerator = mAngleModel2->Numerator();
			tDenominator = mAngleModel2->Denominator();
		}else if (mRectangleModel1->isPresent()){
			tNumerator = mRectangleModel1->Numerator();
			tDenominator = mRectangleModel1->Denominator();
		}else if (mRectangleModel2->isPresent()){
			tNumerator = mRectangleModel2->Numerator();
			tDenominator = mRectangleModel2->Denominator();
		}else if (mTokenModel->isPresent()){
			tNumerator = mTokenModel->getActiveTokens(0);
			tDenominator = mTokenModel->getTotalTokens();
		}
		if(tNumerator!= -1 && tDenominator!= -1){
			if(tNumerator == mExpectedNumerator && tDenominator == mExpectedDenominator){
				tRed = 0.0f;
				tGreen = 1.0f;
			}else{
				tRed = 1.0f;
				tGreen = 0.0f;
			}
			char tNumString[3];
			char tDenomString[3];

			wykobi::quadix<float ,2> tMarkerCorners = mMarker->getCorners();
			mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
			wykobi::point2d<float> tOrigin;
			wykobi::point2d<float> tXUnit;
			wykobi::point2d<float> tYUnit;
			FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

			mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);

			sprintf(tNumString,"%d",tNumerator);
			sprintf(tDenomString,"%d",tDenominator);

			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().Rotate(-wykobi::cartesian_angle(tXUnit), tOrigin.x, tOrigin.y);
			mDecoratorManager.GetDisplay().RenderText(tNumString, tOrigin.x, tOrigin.y, 1.25f, tRed, tGreen, 0.0f);
			mDecoratorManager.GetDisplay().RenderText(tDenomString, tOrigin.x, tOrigin.y + 35.0f, 1.25f, tRed, tGreen, 0.0f);

			mDecoratorManager.GetDisplay().RenderLine(tOrigin.x-5,tOrigin.y+5,tOrigin.x+25,tOrigin.y+5,tRed,tGreen,0.0f,1.0f);
			mDecoratorManager.GetDisplay().PopTransformation();
		}




	}
}

