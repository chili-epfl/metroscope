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

#include "FractionComparisonView.hpp"
#include <iostream>

const std::string decorators::FractionComparisonView::scDecoratorName("FractionComparisonView");
const DecoratorManager::Registerer decorators::FractionComparisonView::mRegisterer(decorators::FractionComparisonView::scDecoratorName, &decorators::FractionComparisonView::create);

decorators::FiducialDecorator *decorators::FractionComparisonView::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager){
	try{
		libconfig::Setting &tFractionCardsString = pSetting ["fraction_cards"];
		FractionCard **tFractionCards = new FractionCard *[24];

		libconfig::Setting &tHintCardsString = pSetting ["hint_cards"];
		FractionBugHint **tHintCards = new FractionBugHint *[5];

		for(int i = 0 ; i < scHintCards ; ++i){
			tHintCards[i] = (FractionBugHint*) pDecoratorManager.loadDecorator(tHintCardsString[i]);
		}

		for(int i = 0 ; i < 24 ; ++i)	tFractionCards[i] = (FractionCard *) pDecoratorManager.loadDecorator(tFractionCardsString[i]);

		return new decorators::FractionComparisonView (pDecoratorManager, (FiducialMarker *) pDecoratorManager.loadMarker(pSetting["marker"]),
				(FiducialMarker *) pDecoratorManager.loadMarker(pSetting["activity_marker"]),
				(CircularFractionModel *) pDecoratorManager.loadDecorator(pSetting["circular_1"]),
				(CircularFractionModel *) pDecoratorManager.loadDecorator(pSetting["circular_2"]),
				(RectangleFractionModel *) pDecoratorManager.loadDecorator(pSetting["rectangle_1"]),
				(RectangleFractionModel *) pDecoratorManager.loadDecorator(pSetting["rectangle_2"]),
				(TokenModel *) pDecoratorManager.loadDecorator(pSetting["token_1"]),
				(FractionCard **) tFractionCards, (FractionBugHint**) tHintCards ,(int)pSetting["order"]);

		}catch(libconfig::SettingNotFoundException &e) {
			std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
		} catch(libconfig::SettingTypeException &e) {
			std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::FractionComparisonView::FractionComparisonView(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pActivityMarker,
		CircularFractionModel *pAngleModel1, CircularFractionModel *pAngleModel2,
		RectangleFractionModel *pRectangleModel1, RectangleFractionModel *pRectangleModel2, TokenModel *pTokenModel1,
		FractionCard ** pFractionCards, FractionBugHint ** pFractionHints, int pOrder):
			FiducialDecorator(pDecoratorManager, pMarker),
			mActivityMarker(pActivityMarker),
			mAngleModel1(pAngleModel1),
			mAngleModel2(pAngleModel2),
			mRectangleModel1(pRectangleModel1),
			mRectangleModel2(pRectangleModel2),
			mTokenModel(pTokenModel1),
			mFractionCards(pFractionCards),
			mFractionHints(pFractionHints),
			mActiveManipulatives(0),
			mOrder(pOrder),
			mCorrectOrder(false),
			mActivityPresent(0),
			mRectangleWidth(0.0f),
			mIsCurrentActivity(false),
			mActualHint(){
}

/*
 * If the activity is present, check manipulatives,
 * display visual feedback both in card and in the
 * display
 */
void decorators::FractionComparisonView::update(){
	if(IsActivityPresent()){
		CheckManipulativesPresent();
		ShowGrid();

		if(mActiveManipulatives>1){
			if(mMarker->isPresent()){
				CheckAnswer();
				ShowCardFeedback();

				if(mCorrectOrder){
					mDecoratorManager.GetDisplay().PushTransformation();
					for(int i = 1; i < mActiveManipulatives ; i++){
						mDecoratorManager.GetDisplay().RenderText((mOrder == 0)? ">" : "<", i*mRectangleWidth-15.0,mDecoratorManager.GetDisplay().GetHeight()/2+15.0, 2.0f, 0.0f,0.0f,0.0f,1.0f);
					}
					mDecoratorManager.GetDisplay().PopTransformation();
				}
			}
			if(IsHintPresent()){
				ShowHintFeedback();
			}
		}
	}

	if(!mIsCurrentActivity)		mActivityPresent = 0;

}

/*
 * Counts and sets the manipulatives that are present
 * in the map (so are ordered from left to right)
 */
void decorators::FractionComparisonView::CheckManipulativesPresent(){
	mActiveManipulatives = 0;
	mFractions.clear();

	if(mAngleModel1->IsPresent()){
		mActiveManipulatives++;
		mFractions[mAngleModel1->GetCenter().x] = mAngleModel1->GetProportion();
		//mNumAndDenom[mAngleModel1->GetCenter().x] = wykobi::make_point(mAngleModel1->Numerator(),mAngleModel1->Denominator());
	}
	if(mAngleModel2->IsPresent()){
		mActiveManipulatives++;
		mFractions[mAngleModel2->GetCenter().x] = mAngleModel2->GetProportion();
		//mNumAndDenom[mAngleModel2->GetCenter().x] = wykobi::make_point(mAngleModel2->Numerator(),mAngleModel2->Denominator());
	}
	if(mRectangleModel1->IsPresent()){
		mActiveManipulatives++;
		mFractions[mRectangleModel1->GetCenter().x] = mRectangleModel1->GetProportion();
		//mNumAndDenom[mAngleModel1->GetCenter().x] = wykobi::make_point(mRectangleModel1->Numerator(),mRectangleModel1->Denominator());
	}
	if(mRectangleModel2->IsPresent()){
		mActiveManipulatives++;
		mFractions[mRectangleModel2->GetCenter().x] = mRectangleModel2->GetProportion();
		//mNumAndDenom[mRectangleModel2->GetCenter().x] = wykobi::make_point(mRectangleModel2->Numerator(),mRectangleModel2->Denominator());
	}
	if(mTokenModel->IsPresent()){
		mActiveManipulatives++;
		mFractions[mTokenModel->GetPosition().x] = mTokenModel->GetProportion();
		//mNumAndDenom[mTokenModel->GetPosition().x] = wykobi::make_point(mTokenModel->GetActiveTokens(0),mTokenModel->GetTotalTokens());
	}
	for(int i = 0 ; i < 24 ; i++){
		if(mFractionCards[i]->IsPresent()){
			mActiveManipulatives++;
			mFractions[mFractionCards[i]->GetLocation().x] = mFractionCards[i]->GetValue();
			//mNumAndDenom[mFractionCards[i]->GetLocation().x] = wykobi::make_point(mFractionCards[i]->GetNumerator(),mFractionCards[i]->GetDenominator());
		}
	}

}

/*
 * Returns true if the side of the card that gives the
 * instruction is present
 */
bool decorators::FractionComparisonView::IsActivityPresent(){
	if(mActivityMarker->isPresent()) mActivityPresent = 1;
	return (mActivityPresent == 1);
}

/*
 * Checks the overall answer by comparing pairs of fractions
 * It starts with the left fraction and goes to the right
 */
void decorators::FractionComparisonView::CheckAnswer(){
	int tComparisionCorrect = 0;

	typedef std::map<float,float>::iterator it_type;
	it_type it = mFractions.begin();
	int tIteration = 1;

	while(it != mFractions.end() && tIteration < mActiveManipulatives){
		float tLeft = (*it).second;
		it++;
		float tRight = (*it).second;
		switch(mOrder){
			case 0: // the order is greater than
				if(tLeft > tRight)	tComparisionCorrect++;
				break;
			case 1: // the order is less than
				if(tLeft < tRight)	tComparisionCorrect++;
				break;
		}

		if(it == mFractions.end()) break;
		tIteration++;
	}

	mCorrectOrder = (tComparisionCorrect == (mActiveManipulatives - 1));
}

/*
 * Shows the working area: it separates the display depending
 * on the number of manipulatives that are active
 */
void decorators::FractionComparisonView::ShowGrid(){
	if(mActiveManipulatives>1){ // Need to have at least 2 fractions

		mRectangleWidth = mDecoratorManager.GetDisplay().GetWidth()/(float)mActiveManipulatives;

		mDecoratorManager.GetDisplay().PushTransformation();

		for(int i = 1; i < mActiveManipulatives ; i++){
			mDecoratorManager.GetDisplay().RenderLine(i*mRectangleWidth,0.0f,i*mRectangleWidth,mDecoratorManager.GetDisplay().GetHeight()/2 - 50.0f,0.0f,0.0f,0.0f,1.0f);
			mDecoratorManager.GetDisplay().RenderLine(i*mRectangleWidth,mDecoratorManager.GetDisplay().GetHeight()/2 + 50.0f,i*mRectangleWidth,mDecoratorManager.GetDisplay().GetHeight(),0.0f,0.0f,0.0f,1.0f);
			mDecoratorManager.GetDisplay().RenderEllipse(i*mRectangleWidth,mDecoratorManager.GetDisplay().GetHeight()/2,50.0f,50.0f,0.0f,0.0f,0.0f,1.0f,1);
		}

		mDecoratorManager.GetDisplay().PopTransformation();

	}
}

/*
 * Shows in the card if the solution is correct (Tres bien!)
 * or not (Essaye encore une fois)
 */
void decorators::FractionComparisonView::ShowCardFeedback(){
	wykobi::quadix<float ,2> tMarkerCorners = mMarker->getCorners();

	mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
	wykobi::point2d<float> tOrigin;
	wykobi::point2d<float> tXUnit;
	wykobi::point2d<float> tYUnit;
	FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

	mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().Rotate(-wykobi::cartesian_angle(tXUnit), tOrigin.x, tOrigin.y);
	mDecoratorManager.GetDisplay().RenderText(mCorrectOrder? "Tres bien!" : "Essaye encore ",
					tOrigin.x - 60.0f, tOrigin.y - 100.0f, 0.7f, (mCorrectOrder) ? 0.0f: 1.0f,
					(mCorrectOrder)? 1.0f : 0.0f, 0.0f);
	if(!mCorrectOrder)	mDecoratorManager.GetDisplay().RenderText("une fois ", tOrigin.x - 60.0f, tOrigin.y - 80.0f, 0.7f, 1.0f, 0.0f, 0.0f);
	mDecoratorManager.GetDisplay().PopTransformation();
}

/*
 * Returns true if one of the hints has been showed, it doesn't need to persist
 * (just to have less cards in the table)
 */
bool decorators::FractionComparisonView::IsHintPresent(){
	for(int i = 0 ; i < 5 ; i++){
		if(mFractionHints[i]->IsPresent()){
			mActualHint = mFractionHints[i];
		}
	}
	return (mActualHint!=NULL);
}

/*
 * Shows the corresponding feedback depending of
 * the fraction that has been shown
 */
void decorators::FractionComparisonView::ShowHintFeedback(){
	int tType = mActualHint->GetHintType();

	switch(tType){
	case 0:
		//DisplayDiscreteHint();
		break;
	case 1:
		//DisplayFractionHint();
		break;
	case 2:
		//DisplayDecimalHint();
		break;
	case 3:
		//DisplayRectangularHint();
		break;
	case 4:
		//DisplayCircularHint();
		break;
	}
}

void decorators::FractionComparisonView::DisplayDiscreteHint(){
	typedef std::map<float,float>::iterator it_type;
	/*it_type it = mNumAndDenom.begin();
	int tIteration = 0;

	while(it != mNumAndDenom.end() && tIteration < mActiveManipulatives){
		float tXPosition = (*it).first;
		wykobi::point2d<int> tFraction = (*it).second;
		int tNumerator = tFraction.x;
		int tDenominator = tFraction.y;
		DisplayIndividualDiscreteHint(tNumerator, tDenominator, wykobi::make_point(tXPosition, 650.0f));
	}*/
}


/*
 * Display the discrete hint of one proportion
 */
void decorators::FractionComparisonView::DisplayIndividualDiscreteHint(int pNumerator, int pDenominator, wykobi::point2d<float> pPosition){

	//Define some parameters
	float tBoxWidth = mDecoratorManager.GetDisplay().GetWidth()/(float)(mActiveManipulatives + 2.0f); // Width of the whole representation
	float tBoxHeight = tBoxWidth/1.5; // Height of the whole representation

	float tCellWidth = tBoxWidth/9; // Width of a small square
	float tCellHeight = tBoxHeight/7; // Width of a small square

	int tDenom = pDenominator;
	int tNum = pNumerator;

	float tPartialX = pPosition.x - tBoxWidth/2;
	float tPartialY = pPosition.y - tBoxHeight/2;


	// Display the cells
	mDecoratorManager.GetDisplay().PushTransformation();
	for(int i = 0 ; i < 4 ; i++){
		for(int j = 0 ; j < 5 ; j++){
			if(tDenom > 0) {
				mDecoratorManager.GetDisplay().RenderQuadFilled(tPartialX,tPartialY,tPartialX+tCellWidth,tPartialY,
						tPartialX+tCellWidth,tPartialY+tCellHeight,tPartialX,tPartialY+tCellHeight,
						tNum > 0 ? 1.0f : 0.0f,tNum > 0 ? 1.0f : 0.0f,tNum > 0 ? 0.0f : 1.0f,1.0f);
				mDecoratorManager.GetDisplay().RenderQuad(tPartialX,tPartialY,tPartialX+tCellWidth,tPartialY,
										tPartialX+tCellWidth,tPartialY+tCellHeight,tPartialX,tPartialY+tCellHeight,
										0.0f, 0.0f, 0.0f,1.0f);
				tPartialX += 2*tCellWidth;
				tDenom--;
				tNum--;
			}
		}
		tPartialY += 2*tCellHeight;
	}

	mDecoratorManager.GetDisplay().PopTransformation();

}

/*
 * Display the fraction hint of one proportion

void decorators::FractionComparisonView::DisplayIndividualFractionHint(int pNumerator, int pDenominator, int pProportionNumber){
	//	Some variables
	int tPosX, tPosY;
	char tProportionNumerator[3];
	char tProportionDenominator[3];

	//	Depending of the proportion number (1,2,3 or 4) we have different X and Y
	switch(pProportionNumber){
		case 1:		tPosX = mProportion1Point.x; 	tPosY = mProportion1Point.y;
			break;
		case 2:		tPosX = mProportion2Point.x; tPosY = mProportion2Point.y;
			break;
		case 3:		tPosX = mProportion3Point.x; tPosY = mProportion3Point.y;
			break;
		case 4:		tPosX = mProportion4Point.x; tPosY = mProportion4Point.y;
			break;
	}

	//	We save in the char* the numerator and denominator
	sprintf(tProportionNumerator, "%d", pNumerator);
	sprintf(tProportionDenominator, "%d",pDenominator);

	//	Display the representation
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportionNumerator,tPosX,tPosY - 15.0f,true,1.0f,0.0f,0.0f,0.0f,1.0f); // Numerator
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportionDenominator,tPosX,tPosY + 15.0f,true,1.0f,0.0f,0.0f,0.0f,1.0f); // Denominator

	mDecoratorManager.GetDisplay().RenderLine(tPosX-15,tPosY,tPosX+15,tPosY,0.0f,0.0f,0.0f,1.0f);	//	Fraction line
	mDecoratorManager.GetDisplay().PopTransformation();
}

/*
 * Display the decimal hint of one proportion

void decorators::FractionComparisonView::DisplayIndividualDecimalHint(float pProportion, int pProportionNumber){
	// Some variables
	int tPosX, tPosY;
	char tProportionText[3];

	switch(pProportionNumber){
		case 1:		tPosX = mProportion1Point.x; 	tPosY = mProportion1Point.y;
			break;
		case 2:		tPosX = mProportion2Point.x; tPosY = mProportion2Point.y;
			break;
		case 3:		tPosX = mProportion3Point.x; tPosY = mProportion3Point.y;
			break;
		case 4:		tPosX = mProportion4Point.x; tPosY = mProportion4Point.y;
			break;
	}

	//	Save the proportion, with 2 decimals
	sprintf(tProportionText, "%3.2f", pProportion);

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderCenteredText(tProportionText, tPosX,tPosY,true,1.0f,0.0f,0.0f,0.0f,1.0f);	//	Render the value
	mDecoratorManager.GetDisplay().PopTransformation();
}

/*
 * Display the rectangular hint of one proportion

void decorators::FractionComparisonView::DisplayIndividualRectangularHint(float pProportion, int pProportionNumber){
	float tRecWidth = mRectangleWidth = mDecoratorManager.GetDisplay().GetWidth()/(float)mActiveManipulatives;;
	float tRecHeight = tRecWidth/2;

	int tPosX, tPosY;
	float tR, tG, tB;

	switch(pProportionNumber){
	case 1:
		tPosX = mProportion1Point.x; 	tPosY = mProportion1Point.y;
		tR = scProp1R; 	tG = scProp1G; 	tB = scProp1B;
		break;
	case 2:
		tPosX = mProportion2Point.x; tPosY = mProportion2Point.y;
		tR = scProp2R; tG = scProp2G; tB = scProp2B;
		break;
	case 3:
		tPosX = mProportion3Point.x; tPosY = mProportion3Point.y;
		tR = scProp3R; tG = scProp3G; tB = scProp3B;
		break;
	case 4:
		tPosX = mProportion4Point.x; tPosY = mProportion4Point.y;
		tR = scProp4R; tG = scProp4G; tB = scProp4B;
		break;
	}

	mDecoratorManager.GetDisplay().PushTransformation();

	mDecoratorManager.GetDisplay().RenderQuadFilled(tPosX-tRecWidth/2, tPosY - tRecHeight/2,
					pProportion*tRecWidth +  tPosX-tRecWidth/2, tPosY - tRecHeight/2,
					pProportion*tRecWidth +  tPosX-tRecWidth/2, tPosY + tRecHeight/2,
					tPosX - tRecWidth/2 , tPosY + tRecHeight/2, tR,tG,tB,1.0f);

	mDecoratorManager.GetDisplay().RenderQuad(tPosX-tRecWidth/2, tPosY - tRecHeight/2,
			tPosX + tRecWidth/2 , tPosY - tRecHeight/2,
			tPosX + tRecWidth/2 , tPosY + tRecHeight/2,
			tPosX - tRecWidth/2 , tPosY + tRecHeight/2, 0.0f,0.0f,0.0f,1.0f);

		mDecoratorManager.GetDisplay().PopTransformation();
}

/*
 * Display the circular hint of one proportion

void decorators::FractionComparisonView::DisplayIndividualCircularHint(float pProportion, int pProportionNumber){
	// The parameter
	float tRadius = mWorkingTriangle/18;
	int tPosX, tPosY;
	float tR, tG, tB;

	switch(pProportionNumber){
	case 1:
		tPosX = mProportion1Point.x; 	tPosY = mProportion1Point.y;
		tR = scProp1R; 	tG = scProp1G; 	tB = scProp1B;
		break;
	case 2:
		tPosX = mProportion2Point.x; tPosY = mProportion2Point.y;
		tR = scProp2R; tG = scProp2G; tB = scProp2B;
		break;
	case 3:
		tPosX = mProportion3Point.x; tPosY = mProportion3Point.y;
		tR = scProp3R; tG = scProp3G; tB = scProp3B;
		break;
	case 4:
		tPosX = mProportion4Point.x; tPosY = mProportion4Point.y;
		tR = scProp4R; tG = scProp4G; tB = scProp4B;
		break;
	}

	mDecoratorManager.GetDisplay().PushTransformation();

	mDecoratorManager.GetDisplay().RenderFilledSector(tPosX,tPosY,tRadius,tRadius,pProportion*360,90.0f - pProportion*360,tR,tG,tB,1.0f,1);
	mDecoratorManager.GetDisplay().RenderEllipse(tPosX,tPosY,tRadius,tRadius,0.0f,0.0f,0.0f,1.0f);

	mDecoratorManager.GetDisplay().PopTransformation();
}
 */
