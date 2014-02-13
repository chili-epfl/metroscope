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

#include "FractionModel.hpp"
#include <iostream>

const std::string decorators::FractionModel::scDecoratorName("FractionModel");
const  DecoratorManager::Registerer decorators::FractionModel::mRegisterer(decorators::FractionModel::scDecoratorName, &decorators::FractionModel::create);

decorators::FiducialDecorator *decorators::FractionModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tCardStrings = pSetting["cards"];
		int tNumCards = pSetting["num_cards"];
		FractionCard **tFractionCards = new FractionCard *[tNumCards];
		for (int i = 0; i < tNumCards; ++i) {
			tFractionCards[i] = (FractionCard *) pDecoratorManager.loadDecorator(tCardStrings[i]);
		}

		return new decorators::FractionModel(pDecoratorManager, tFractionCards, tNumCards, (VariableFractionCard *)pDecoratorManager.loadDecorator(pSetting["variable_fraction_card"]));

	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::FractionModel::FractionModel(DecoratorManager &pDecoratorManager, FractionCard **pCards, const int pNumCards, VariableFractionCard *pVariableFractionCard):
FiducialDecorator(pDecoratorManager, 0),
mNumCards(pNumCards),
mCards(pCards),
mVariableFractionCard(pVariableFractionCard),
mPresentActivities(),
mActiveCards(),
mHintState(new HintState())
{
}
decorators::FractionModel::~FractionModel(){
	delete [] mCards;
	delete mHintState;
}


void decorators::FractionModel::update(){

}

//Returns the index of the added hint type
int decorators::FractionModel::AddHintType(const HintType pHintType){
	std::vector<HintType>::iterator it = std::find(mCurrentHintTypes.begin(), mCurrentHintTypes.end(), pHintType);
	if(it != mCurrentHintTypes.end()) return std::distance(mCurrentHintTypes.begin(), it);
	mCurrentHintTypes.push_back(pHintType);
	return mCurrentHintTypes.size()- 1;
}

void decorators::FractionModel::RemoveHintType(const HintType pHintType){
	std::vector<HintType>::iterator it = std::find(mCurrentHintTypes.begin(), mCurrentHintTypes.end(), pHintType);
	if(it != mCurrentHintTypes.end()) mCurrentHintTypes.erase(it);
}

bool decorators::FractionModel::HintTypeIsPresent(const HintType pHintType){
	std::vector<HintType>::iterator it = std::find(mCurrentHintTypes.begin(), mCurrentHintTypes.end(), pHintType);
	return it != mCurrentHintTypes.end();
}

void decorators::FractionModel::ResetHintState(){
	mHintState->level = 0;
	mHintState->errorType = Unknown;
	mHintState->message = "";
}
void decorators::FractionModel::IncreaseHintLevel(){
	if(mHintState->level != mHintState->numLevels) mHintState->level++;
}
int	decorators::FractionModel::GetHintLevel(){
	return mHintState->level;
}

void decorators::FractionModel::SetHintErrorType(ErrorType pType){
	mHintState->errorType = pType;
}
ErrorType decorators::FractionModel::GetHintErrorType(){
	return mHintState->errorType;
}

const char *decorators::FractionModel::GetHintMessage(){
	return mHintState->message;
}
void decorators::FractionModel::SetHintMessage(const char *pMessage){
	mHintState->message = pMessage;
}
void decorators::FractionModel::SetHintMessage(char *pMessage){
	strcpy(mHintState->strBuffer, pMessage);
	mHintState->message = mHintState->strBuffer;
}

void decorators::FractionModel::SetHintMessageGraphicsType(HintType pType){
	mHintState->messageGraphicsType = pType;
}

HintType decorators::FractionModel::GetHintMessageGraphicsType(){
	return mHintState->messageGraphicsType;
}

void decorators::FractionModel::SetHintMessageGraphicsFractions(int pNum1, int pDenom1, int pNum2, int pDenom2){
	mHintState->graphicsNum1 = pNum1;
	mHintState->graphicsDenom1 = pDenom1;
	mHintState->graphicsNum2 = pNum2;
	mHintState->graphicsDenom2 = pDenom2;
}
void decorators::FractionModel::GetHintMessageGraphicsFractions(int &pNum1, int &pDenom1, int &pNum2, int &pDenom2){
	pNum1 =mHintState->graphicsNum1;
	pDenom1 = mHintState->graphicsDenom1;
	pNum2 = mHintState->graphicsNum2;
	pDenom2	= mHintState->graphicsDenom2;
}


void decorators::FractionModel::SetCurrentActivity (const char *pActivityName){
	std::vector<const char *>::iterator it = std::find(mPresentActivities.begin(), mPresentActivities.end(), pActivityName);
	if(it == mPresentActivities.end()) mPresentActivities.push_back(pActivityName);
}

const char *decorators::FractionModel::GetCurrentActivity () const {
	if(!mPresentActivities.empty())return mPresentActivities.front();
	return "";
}

void decorators::FractionModel::RemoveActivity(const char *pActivityName){
	std::vector<const char *>::iterator it = std::find(mPresentActivities.begin(), mPresentActivities.end(), pActivityName);
	if(it != mPresentActivities.end()) mPresentActivities.erase(it);
}

std::vector<const char*> & decorators::FractionModel::GetPresentActivities() {
	return mPresentActivities;
}


std::vector<decorators::FractionCard *> & decorators::FractionModel::GetActiveCards() {
	return mActiveCards;
}

void decorators::FractionModel::ClearActiveCards(){
	mActiveCards.clear();
}


/*Accepts two points in real world coordinates and calculates the angle between the line formed by the two points and the x-axis.*/
float decorators::FractionModel::CalculateTransformAngle(wykobi::point2d<float> &pOrigin, wykobi::point2d<float> &pPointOnPosXAxis)
{
	return std::atan2(pPointOnPosXAxis.y - pOrigin.y, pPointOnPosXAxis.x - pOrigin.x);
}

void decorators::FractionModel::transformPoint(wykobi::point2d<float> &pPoint, wykobi::point2d<float> pOrigin, float pTransformAngle)
{
	float tCosine = wykobi::cos(pTransformAngle);
	float tSine = wykobi::sin(pTransformAngle);
	float tX = pPoint.x-pOrigin.x;
	float tY = pPoint.y-pOrigin.y;
	pPoint.x = tX*tCosine - tY*tSine;
	pPoint.y = tX*tSine + tY*tCosine;
}


int decorators::FractionModel::FractionLCD(std::vector<FractionCard *> &pFractions){
	int tLCD = pFractions.front()->GetDenominator();
		for(std::vector<FractionCard *>::iterator it = pFractions.begin()+1; it != pFractions.end(); ++it){
			tLCD = LCM(tLCD, (*it)->GetDenominator());
		}
	return tLCD;
}

//Finds the least-common multiple of two ints
int decorators::FractionModel::LCM(int x, int y){
	if(x != 0 && y!=0) return (x*y)/GCD(x, y);
	return 0;
}

//Finds the greatest common divisor of two positive integers. Returns -1 upon error
int decorators::FractionModel::GCD(int x, int y) {
	if ( y == 0 ) return x;
    else if (y > 0) return GCD(y, x%y);
    else return -1;

}

bool decorators::FractionModel::IsReduced(int pNumerator, int pDenominator){
	return GCD(pNumerator, pDenominator) == 1;
}

void decorators::FractionModel::getCardsInsideRectangle(std::vector<FractionCard *> & pCardArray, wykobi::point2d<float> pOrigin, wykobi::vector2d<float> pTransformXVector, wykobi::vector2d<float> pTransformYVector, float pXMin, float pXMax, float pYMin, float pYMax)
{
	wykobi::line<float,2> tTransformXAxis = wykobi::make_line(make_ray(pOrigin, pTransformXVector));
	wykobi::line<float,2> tTransformYAxis = wykobi::make_line(make_ray(pOrigin, pTransformYVector));

	for(int i=0; i < mNumCards; i++){
		wykobi::point2d<float> tPoint = mCards[i]->GetLocation();
		if(!(tPoint.x ==0.0f && tPoint.y == 0.0f)){
			mDecoratorManager.GetCam2World().InterpolatedMap(tPoint);
			wykobi::point2d<float> tTransformedPoint;

			float tXOrientation = (wykobi::dot_product(pTransformXVector, wykobi::make_vector(pOrigin) - wykobi::make_vector(tPoint)) > 0) ? -1.0f : 1.0f;
			float tYOrientation = (wykobi::dot_product(pTransformYVector, wykobi::make_vector(pOrigin) - wykobi::make_vector(tPoint)) > 0) ? -1.0f : 1.0f;

			tTransformedPoint.x = tXOrientation*wykobi::distance(wykobi::closest_point_on_line_from_point(tTransformXAxis, tPoint), pOrigin);
			tTransformedPoint.y = tYOrientation*wykobi::distance(wykobi::closest_point_on_line_from_point(tTransformYAxis, tPoint), pOrigin);

			if(valueIsInRange(tTransformedPoint.x, pXMin, pXMax)&& valueIsInRange(tTransformedPoint.y, pYMin, pYMax)) {
				pCardArray.push_back(mCards[i]);
			}
		}
	}
}

bool decorators::FractionModel::valueIsInRange(float &pValue, float pRangeStart, float pRangeEnd){
	return pValue > pRangeStart && pValue < pRangeEnd;
}



void decorators::FractionModel::getClosestFactors(int pNumber, int &pSmallFactor, int &pLargeFactor){
	if(pNumber == 1){
		pSmallFactor = 1;
		pLargeFactor = 1;
		return;
	}
	for(int i = 1; i < pNumber; i++){
		if(pNumber%i == 0){
			pLargeFactor = i;
			pSmallFactor = pNumber/i;
			if(pSmallFactor <= pLargeFactor) break;
		}
	}
	if(pSmallFactor > pLargeFactor) {
		int temp = pSmallFactor;
		pSmallFactor = pLargeFactor;
		pLargeFactor = temp;
	}
}


bool decorators::FractionModel::FractionCardSortByValue(const FractionCard *card1, const FractionCard *card2){
	return (card1->GetValue() < card2->GetValue());
}
