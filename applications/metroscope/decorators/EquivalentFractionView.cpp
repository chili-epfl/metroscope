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

#include "EquivalentFractionView.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>

const std::string decorators::EquivalentFractionView::scDecoratorName("EquivalentFractionView");
const DecoratorManager::Registerer decorators::EquivalentFractionView::mRegisterer(decorators::EquivalentFractionView::scDecoratorName, &decorators::EquivalentFractionView::create);

decorators::FiducialDecorator *decorators::EquivalentFractionView::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager){
	try{
		libconfig::Setting &tFractionCardsString = pSetting ["fraction_cards"];
		libconfig::Setting &tActivityCardsString = pSetting ["fractions"];

		FractionCard **tFractionCards = new FractionCard *[15];
		FractionCard **tActivityCards = new FractionCard *[9];

		for(int i = 0 ; i < 15 ; ++i){
			tFractionCards[i] = (FractionCard *) pDecoratorManager.loadDecorator(tFractionCardsString[i]);
		}

		for(int i = 0 ; i < 9 ; ++i){
			tActivityCards[i] = (FractionCard *) pDecoratorManager.loadDecorator(tActivityCardsString[i]);
		}

		return new decorators::EquivalentFractionView (pDecoratorManager, (FiducialMarker *) pDecoratorManager.loadMarker(pSetting["marker"]),
			(CircularFractionModel *) pDecoratorManager.loadDecorator(pSetting["circular_1"]),
			(CircularFractionModel *) pDecoratorManager.loadDecorator(pSetting["circular_2"]),
			(RectangleFractionModel *) pDecoratorManager.loadDecorator(pSetting["rectangle_1"]),
			(RectangleFractionModel *) pDecoratorManager.loadDecorator(pSetting["rectangle_2"]),
			(TokenModel *) pDecoratorManager.loadDecorator(pSetting["token_1"]),
			(FractionCard **) tActivityCards, (FractionCard **) tFractionCards);

		}catch(libconfig::SettingNotFoundException &e) {
			std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
		} catch(libconfig::SettingTypeException &e) {
			std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
		}
		return 0;
}

decorators::EquivalentFractionView::EquivalentFractionView(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker,
		CircularFractionModel *pAngleModel1, CircularFractionModel *pAngleModel2,
		RectangleFractionModel *pRectangleModel1, RectangleFractionModel *pRectangleModel2, TokenModel *pTokenModel1,
		FractionCard ** pActivityCards, FractionCard ** pFractionCards):
			FiducialDecorator(pDecoratorManager, pMarker),
			mAngleModel1(pAngleModel1), mAngleModel2(pAngleModel2),
			mRectangleModel1(pRectangleModel1), mRectangleModel2(pRectangleModel2),
			mTokenModel(pTokenModel1), mActivityCard(pActivityCards),mFractionCards(pFractionCards),
			mCurrentProportion(0.0f), mActiveManipulatives(0), mEquivalentManipulatives(0),mCurrentActivity(0),mIsCurrentActivity(false){
}


void decorators::EquivalentFractionView::update(){
	if(mIsCurrentActivity){
		if(mMarker->isPresent() && IsActivityPresent()){
				wykobi::quadix<float ,2> tMarkerCorners = mCurrentActivity->getMarker().getCorners();
				mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
				wykobi::point2d<float> tOrigin;
				wykobi::point2d<float> tXUnit;
				wykobi::point2d<float> tYUnit;
				FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

				mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);

				mDecoratorManager.GetDisplay().PushTransformation();
				mDecoratorManager.GetDisplay().Rotate(-wykobi::cartesian_angle(tXUnit), tOrigin.x, tOrigin.y);
				mDecoratorManager.GetDisplay().RenderEllipse(tOrigin.x, tOrigin.y+20.0f,70.0f,70.0f,0.0f,0.0f,0.0f,1.0f,1);

				mDecoratorManager.GetDisplay().RenderText("Mettez des fractions équivalentes", tOrigin.x - 150.0f, tOrigin.y - 70.0f, 0.7f, 0.0f, 0.0f, 0.0f);
				mDecoratorManager.GetDisplay().PopTransformation();

				CheckManipulativesPresent();
				if(mActiveManipulatives>0){

						wykobi::quadix<float ,2> tMarkerCorners = mMarker->getCorners();
						mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
						wykobi::point2d<float> tOrigin;
						wykobi::point2d<float> tXUnit;
						wykobi::point2d<float> tYUnit;
						FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

						mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);

						mDecoratorManager.GetDisplay().PushTransformation();
						mDecoratorManager.GetDisplay().Rotate(-wykobi::cartesian_angle(tXUnit), tOrigin.x, tOrigin.y);
						mDecoratorManager.GetDisplay().RenderText((mEquivalentManipulatives == mActiveManipulatives)? "Tres bien!" : "Essayez encore une fois",
								tOrigin.x, tOrigin.y, 0.7f, (mEquivalentManipulatives == mActiveManipulatives)? 0.0f: 1.0f,
										(mEquivalentManipulatives == mActiveManipulatives)? 1.0f : 0.0f, 0.0f);

						mDecoratorManager.GetDisplay().PopTransformation();
			}
		}
	}
}



bool decorators::EquivalentFractionView::IsActivityPresent(){
	for(int i = 0; i < 9 ; i++){
		if(mActivityCard[i]->IsPresent()){
			mCurrentActivity = mActivityCard[i];
			mCurrentProportion = mActivityCard[i]->GetValue();
			return true;
		}
	}
	return false;
}

void decorators::EquivalentFractionView::CheckManipulativesPresent(){
	mActiveManipulatives = 0;
	mEquivalentManipulatives = 0;

	if(mAngleModel1->isPresent()){
		mActiveManipulatives++;
		if((mAngleModel1->Numerator()/(float)mAngleModel1->Denominator()) == mCurrentProportion){
			mEquivalentManipulatives++;
			//TODO: do it in a nice way, methods
			wykobi::quadix<float ,2> tMarkerCorners = mAngleModel1->getMarker().getCorners();
			mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
			wykobi::point2d<float> tOrigin;
			wykobi::point2d<float> tXUnit;
			wykobi::point2d<float> tYUnit;
			FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

			mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);

			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().Rotate(-wykobi::cartesian_angle(tXUnit), tOrigin.x, tOrigin.y);
			mDecoratorManager.GetDisplay().RenderEllipse(tOrigin.x, tOrigin.y+20.0f,170.0f,170.0f,0.0f,1.0f,0.0f,1.0f,1);

			mDecoratorManager.GetDisplay().PopTransformation();
		}
	}
	if(mAngleModel2->isPresent()){
		mActiveManipulatives++;
		if((mAngleModel2->Numerator()/(float)mAngleModel2->Denominator()) == mCurrentProportion){
			mEquivalentManipulatives++;

			//TODO: do it in a nice way, methods
			wykobi::quadix<float ,2> tMarkerCorners = mAngleModel2->getMarker().getCorners();
						mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
						wykobi::point2d<float> tOrigin;
						wykobi::point2d<float> tXUnit;
						wykobi::point2d<float> tYUnit;
						FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

						mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);

						mDecoratorManager.GetDisplay().PushTransformation();
						mDecoratorManager.GetDisplay().Rotate(-wykobi::cartesian_angle(tXUnit), tOrigin.x, tOrigin.y);
						mDecoratorManager.GetDisplay().RenderEllipse(tOrigin.x, tOrigin.y+20.0f,170.0f,170.0f,0.0f,1.0f,0.0f,1.0f,1);

						mDecoratorManager.GetDisplay().PopTransformation();
		}
	}
	if(mRectangleModel1->isPresent()){
		mActiveManipulatives++;
		if((mRectangleModel1->Numerator()/(float)mRectangleModel1->Denominator()) == mCurrentProportion){
			mEquivalentManipulatives++;

			//TODO: do it in a nice way, methods
			wykobi::quadix<float ,2> tMarkerCorners = mRectangleModel1->getMarker().getCorners();
						mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
						wykobi::point2d<float> tOrigin;
						wykobi::point2d<float> tXUnit;
						wykobi::point2d<float> tYUnit;
						FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

						mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);

						mDecoratorManager.GetDisplay().PushTransformation();
						mDecoratorManager.GetDisplay().Rotate(-wykobi::cartesian_angle(tXUnit), tOrigin.x, tOrigin.y);
						mDecoratorManager.GetDisplay().RenderEllipse(tOrigin.x, tOrigin.y+20.0f,170.0f,170.0f,0.0f,1.0f,0.0f,1.0f,1);

						mDecoratorManager.GetDisplay().PopTransformation();
		}
	}
	if(mRectangleModel2->isPresent()){
		mActiveManipulatives++;
		if((mRectangleModel2->Numerator()/(float)mRectangleModel2->Denominator()) == mCurrentProportion){
			mEquivalentManipulatives++;
			//TODO: do it in a nice way, methods
						wykobi::quadix<float ,2> tMarkerCorners = mRectangleModel2->getMarker().getCorners();
									mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
									wykobi::point2d<float> tOrigin;
									wykobi::point2d<float> tXUnit;
									wykobi::point2d<float> tYUnit;
									FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

									mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);

									mDecoratorManager.GetDisplay().PushTransformation();
									mDecoratorManager.GetDisplay().Rotate(-wykobi::cartesian_angle(tXUnit), tOrigin.x, tOrigin.y);
									mDecoratorManager.GetDisplay().RenderEllipse(tOrigin.x, tOrigin.y+20.0f,170.0f,170.0f,0.0f,1.0f,0.0f,1.0f,1);

									mDecoratorManager.GetDisplay().PopTransformation();
		}
	}
	if(mTokenModel->isPresent()){
		mActiveManipulatives++;
		if(mTokenModel->GetProportion() == mCurrentProportion){
			mEquivalentManipulatives++;
			//TODO: do it in a nice way, methods

									wykobi::point2d<float> tOrigin = mTokenModel->GetPosition();

									mDecoratorManager.GetDisplay().PushTransformation();
									mDecoratorManager.GetDisplay().RenderEllipse(tOrigin.x, tOrigin.y+20.0f,100.0f,100.0f,0.0f,1.0f,0.0f,1.0f,1);

									mDecoratorManager.GetDisplay().PopTransformation();
		}
	}
	for(int i = 0 ; i < 15 ; i++){
		if(mFractionCards[i]->IsPresent()){
			mActiveManipulatives++;
			if(mFractionCards[i]->GetValue() == mCurrentProportion){
				mEquivalentManipulatives++;
				//TODO: do it in a nice way, methods
							wykobi::quadix<float ,2> tMarkerCorners = mFractionCards[i]->getMarker().getCorners();
										mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
										wykobi::point2d<float> tOrigin;
										wykobi::point2d<float> tXUnit;
										wykobi::point2d<float> tYUnit;
										FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

										mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);

										mDecoratorManager.GetDisplay().PushTransformation();
										mDecoratorManager.GetDisplay().Rotate(-wykobi::cartesian_angle(tXUnit), tOrigin.x, tOrigin.y);
										mDecoratorManager.GetDisplay().RenderEllipse(tOrigin.x, tOrigin.y+20.0f,70.0f,70.0f,0.0f,1.0f,0.0f,1.0f,1);

										mDecoratorManager.GetDisplay().PopTransformation();
			}
		}
	}

}
