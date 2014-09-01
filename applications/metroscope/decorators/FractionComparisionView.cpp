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

#include "FractionComparisionView.hpp"
#include <iostream>

const std::string decorators::FractionComparisionView::scDecoratorName("FractionComparisionView");
const DecoratorManager::Registerer decorators::FractionComparisionView::mRegisterer(decorators::FractionComparisionView::scDecoratorName, &decorators::FractionComparisionView::create);

decorators::FiducialDecorator *decorators::FractionComparisionView::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager){
	try{
		libconfig::Setting &tFractionCardsString = pSetting ["fraction_cards"];
		FractionCard **tFractionCards = new FractionCard *[24];

		for(int i = 0 ; i < 24 ; ++i)	tFractionCards[i] = (FractionCard *) pDecoratorManager.loadDecorator(tFractionCardsString[i]);

		return new decorators::FractionComparisionView (pDecoratorManager, (FiducialMarker *) pDecoratorManager.loadMarker(pSetting["marker"]),
				(FiducialMarker *) pDecoratorManager.loadMarker(pSetting["activity_marker"]),
				(CircularFractionModel *) pDecoratorManager.loadDecorator(pSetting["circular_1"]),
				(CircularFractionModel *) pDecoratorManager.loadDecorator(pSetting["circular_2"]),
				(RectangleFractionModel *) pDecoratorManager.loadDecorator(pSetting["rectangle_1"]),
				(RectangleFractionModel *) pDecoratorManager.loadDecorator(pSetting["rectangle_2"]),
				(TokenModel *) pDecoratorManager.loadDecorator(pSetting["token_1"]),
				(FractionCard **) tFractionCards, (int)pSetting["order"]);

		}catch(libconfig::SettingNotFoundException &e) {
			std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
		} catch(libconfig::SettingTypeException &e) {
			std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::FractionComparisionView::FractionComparisionView(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pActivityMarker,
		CircularFractionModel *pAngleModel1, CircularFractionModel *pAngleModel2,
		RectangleFractionModel *pRectangleModel1, RectangleFractionModel *pRectangleModel2, TokenModel *pTokenModel1,
		FractionCard ** pFractionCards, int pOrder):
			FiducialDecorator(pDecoratorManager, pMarker),mActivityMarker(pActivityMarker),
			mAngleModel1(pAngleModel1), mAngleModel2(pAngleModel2), mRectangleModel1(pRectangleModel1), mRectangleModel2(pRectangleModel2),
			mTokenModel(pTokenModel1), mFractionCards(pFractionCards),mActiveManipulatives(0), mOrder(pOrder), mCorrectOrder(false),
			mActivityPresent(0),mRectangleWidth(0.0f),mIsCurrentActivity(false){
}
void decorators::FractionComparisionView::update(){
		if(IsActivityPresent()){
				CheckManipulativesPresent();
				if(mActiveManipulatives>1){
					mRectangleWidth = mDecoratorManager.GetDisplay().GetWidth()/(float)mActiveManipulatives;

					mDecoratorManager.GetDisplay().PushTransformation();
					for(int i = 1; i < mActiveManipulatives ; i++){
						mDecoratorManager.GetDisplay().RenderLine(i*mRectangleWidth,0.0f,i*mRectangleWidth,mDecoratorManager.GetDisplay().GetHeight()/2 - 50.0f,0.0f,0.0f,0.0f,1.0f);
						mDecoratorManager.GetDisplay().RenderLine(i*mRectangleWidth,mDecoratorManager.GetDisplay().GetHeight()/2 + 50.0f,i*mRectangleWidth,mDecoratorManager.GetDisplay().GetHeight(),0.0f,0.0f,0.0f,1.0f);
						mDecoratorManager.GetDisplay().RenderEllipse(i*mRectangleWidth,mDecoratorManager.GetDisplay().GetHeight()/2,50.0f,50.0f,0.0f,0.0f,0.0f,1.0f,1);
					}

					mDecoratorManager.GetDisplay().PopTransformation();


					if(mMarker->isPresent()){
						CheckAnswer();

						wykobi::quadix<float ,2> tMarkerCorners = mMarker->getCorners();
						mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
						wykobi::point2d<float> tOrigin;
						wykobi::point2d<float> tXUnit;
						wykobi::point2d<float> tYUnit;
						FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

						mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);

						mDecoratorManager.GetDisplay().PushTransformation();
						mDecoratorManager.GetDisplay().Rotate(-wykobi::cartesian_angle(tXUnit), tOrigin.x, tOrigin.y);
						mDecoratorManager.GetDisplay().RenderText(mCorrectOrder? "Tres bien!" : "Essaye encore une fois",
								tOrigin.x, tOrigin.y, 0.7f, (mCorrectOrder) ? 0.0f: 1.0f,
								(mCorrectOrder)? 1.0f : 0.0f, 0.0f);
						mDecoratorManager.GetDisplay().PopTransformation();

						if(mCorrectOrder){
							mDecoratorManager.GetDisplay().PushTransformation();
							for(int i = 1; i < mActiveManipulatives ; i++){
								mDecoratorManager.GetDisplay().RenderText((mOrder == 0)? ">" : "<", i*mRectangleWidth-15.0,mDecoratorManager.GetDisplay().GetHeight()/2+15.0, 2.0f, 0.0f,0.0f,0.0f,1.0f);
							}
							mDecoratorManager.GetDisplay().PopTransformation();
						}
					}
				}
			}
		if(!mIsCurrentActivity)		mActivityPresent = 0;

}


void decorators::FractionComparisionView::CheckManipulativesPresent(){
	mActiveManipulatives = 0;
	mFractions.clear();

	if(mAngleModel1->isPresent()){
		mActiveManipulatives++;
		mFractions[mAngleModel1->GetCenter().x] = mAngleModel1->GetProportion();
	}
	if(mAngleModel2->isPresent()){
		mActiveManipulatives++;
		mFractions[mAngleModel2->GetCenter().x] = mAngleModel2->GetProportion();
	}
	if(mRectangleModel1->isPresent()){
		mActiveManipulatives++;
		mFractions[mRectangleModel1->GetCenter().x] = mRectangleModel1->proportion();
	}
	if(mRectangleModel2->isPresent()){
		mActiveManipulatives++;
		mFractions[mRectangleModel2->GetCenter().x] = mRectangleModel2->proportion();
	}
	if(mTokenModel->isPresent()){
		mActiveManipulatives++;
		mFractions[mTokenModel->GetPosition().x] = mTokenModel->GetProportion();
	}
	for(int i = 0 ; i < 24 ; i++){
		if(mFractionCards[i]->IsPresent()){
			mActiveManipulatives++;
			mFractions[mFractionCards[i]->GetLocation().x] = mFractionCards[i]->GetValue();
		}
	}

}

bool decorators::FractionComparisionView::IsActivityPresent(){
	if(mActivityMarker->isPresent()) mActivityPresent = 1;
	return (mActivityPresent == 1);
}

void decorators::FractionComparisionView::IdentifyFractions(){

}

void decorators::FractionComparisionView::CheckAnswer(){
	int tComparisionCorrect = 0;

	typedef std::map<float,float>::iterator it_type;
	it_type it = mFractions.begin();
	int tIteration = 1;

	while(it != mFractions.end() && tIteration < mActiveManipulatives){
		float tLeft = (*it).second;
		it++;
		float tRight = (*it).second;
		switch(mOrder){
			case 0: //greater than
				if(tLeft > tRight)	tComparisionCorrect++;
				break;
			case 1:
				if(tLeft < tRight)	tComparisionCorrect++;
				break;
		}

		if(it == mFractions.end()) break;
		tIteration++;
	}

	mCorrectOrder = (tComparisionCorrect == (mActiveManipulatives - 1));
}




