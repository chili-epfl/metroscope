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
			mTokenModel(pTokenModel1),
			mActivityCard(pActivityCards),
			mFractionCards(pFractionCards),
			mCurrentProportion(0.0f),
			mActiveManipulatives(0),
			mEquivalentManipulatives(0),
			mCurrentActivity(0),
			mIsCurrentActivity(false){
				mCircularModel.push_back(pAngleModel1);
				mCircularModel.push_back(pAngleModel2);
				mRectangularModel.push_back(pRectangleModel1);
				mRectangularModel.push_back(pRectangleModel2);
			}

/*
 * If the card is present and the fraction to be
 * simplified or amplified, it shows above it the
 * instruction.
 */
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

			ShowCardFeedback();
		}
	}
}

/*
 * Returns true if the activity card is present (fraction
 * to be simplified or amplified)
 */
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

/*
 * Calculate how many manipulatives are present and how many
 * are equivalent with the given fraction
 */
void decorators::EquivalentFractionView::CheckManipulativesPresent(){
	mActiveManipulatives = 0;
	mEquivalentManipulatives = 0;

	CheckCircularModel();
	CheckRectangularModel();
	CheckTokenModel();
	CheckFractionModel();

}

/*
 * Checks the two circular model. If is present, it will show feedback
 */
void decorators::EquivalentFractionView::CheckCircularModel(){

	for(std::vector<CircularFractionModel *>::iterator it = mCircularModel.begin(); it != mCircularModel.end() ; ++it){
		CircularFractionModel *tActualCircularModel = (*it);
		if(tActualCircularModel->IsPresent()){
			mActiveManipulatives++;

			if(tActualCircularModel->Numerator()/(float)tActualCircularModel->Denominator() == mCurrentProportion) mEquivalentManipulatives++;

			ShowFeedback(tActualCircularModel->GetMarker().getCorners(),0,
					(tActualCircularModel->Numerator()/(float)tActualCircularModel->Denominator() == mCurrentProportion));
		}
	}
}

/*
 * Checks the two rectangular model. If is present, it will show feedback
 */
void decorators::EquivalentFractionView::CheckRectangularModel(){
	for(std::vector<RectangleFractionModel *>::iterator it = mRectangularModel.begin(); it != mRectangularModel.end() ; ++it){
		RectangleFractionModel *tActualRectangularModel = (*it);

		if(tActualRectangularModel->IsPresent()){
			mActiveManipulatives++;

			if(tActualRectangularModel->Numerator()/(float)tActualRectangularModel->Denominator() == mCurrentProportion) mEquivalentManipulatives++;

			ShowFeedback(tActualRectangularModel->getMarker().getCorners(),1,
					(tActualRectangularModel->Numerator()/(float)tActualRectangularModel->Denominator() == mCurrentProportion));
		}
	}
}

/*
 * Checks token model. If is present, it will show feedback
 */
void decorators::EquivalentFractionView::CheckTokenModel(){
	if(mTokenModel->IsPresent()){
		mActiveManipulatives++;

		if(mTokenModel->GetProportion() == mCurrentProportion)	mEquivalentManipulatives++;

		wykobi::point2d<float> tOrigin = mTokenModel->GetPosition();

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().RenderEllipse(tOrigin.x, tOrigin.y,150.0f,150.0f,
				(mTokenModel->GetProportion() == mCurrentProportion)?0.0f:1.0f,
				(mTokenModel->GetProportion() == mCurrentProportion)?1.0f:0.0f,0.0f,1.0f,1);

		mDecoratorManager.GetDisplay().PopTransformation();

	}
}

/*
 * Checks every fraction card. If is present, it will show feedback
 */
void decorators::EquivalentFractionView::CheckFractionModel(){
	for(int i = 0 ; i < 15 ; i++){
		if(mFractionCards[i]->IsPresent()){
			mActiveManipulatives++;
			if(mFractionCards[i]->GetValue() == mCurrentProportion)	mEquivalentManipulatives++;

			ShowFeedback(mFractionCards[i]->getMarker().getCorners(), 3, (mFractionCards[i]->GetValue() == mCurrentProportion));
		}
	}
}

/*
 * It displays a circle around any manipulative present (red
 * if is wrong and green if is correct)
 */
void decorators::EquivalentFractionView::ShowFeedback(wykobi::quadix<float,2> pMarkerCorners, int pManipulativeType, bool pIsCorrect){
	mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(pMarkerCorners);
	wykobi::point2d<float> tOrigin;
	wykobi::point2d<float> tXUnit;
	wykobi::point2d<float> tYUnit;
	FiducialMarker::ComputeBasisFromSquare(pMarkerCorners, tOrigin, tXUnit, tYUnit);

	mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);

	float tXPosition = 0.0f;
	float tYPosition = 0.0f;
	float tRadius = 0.0f;

	switch(pManipulativeType){
	case 0:
		tXPosition = tOrigin.x;
		tYPosition = tOrigin.y+20.0f;
		tRadius = 170.0f;
		break;
	case 1:
		tXPosition = tOrigin.x;
		tYPosition = tOrigin.y+20.0f;
		tRadius = 170.0f;
		break;
	case 3:
		tXPosition = tOrigin.x;
		tYPosition = tOrigin.y + 20.0f;
		tRadius = 70.0f;
		break;
	}

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().Rotate(-wykobi::cartesian_angle(tXUnit), tOrigin.x, tOrigin.y);
	mDecoratorManager.GetDisplay().RenderEllipse(tXPosition, tYPosition,tRadius,tRadius,(pIsCorrect)?0.0f:1.0f,(pIsCorrect)?1.0f:0.0f,0.0f,1.0f,1);

	mDecoratorManager.GetDisplay().PopTransformation();
}

/*
 * Gives the feedback in the card: "Tres bien!"
 * or "Essayez encore une fois" if more practice is
 * needed
 */
void decorators::EquivalentFractionView::ShowCardFeedback(){

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
