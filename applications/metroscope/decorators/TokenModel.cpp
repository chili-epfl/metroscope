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

#include "TokenModel.hpp"
#include <iostream>

const std::string decorators::TokenModel::scDecoratorName("TokenModel");
const DecoratorManager::Registerer decorators::TokenModel::mRegisterer(decorators::TokenModel::scDecoratorName, &decorators::TokenModel::create);


decorators::FiducialDecorator *decorators::TokenModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager){
	try{
		libconfig::Setting & tTokenString = pSetting["cards"];
		int tTokenNumber = pSetting["num_cards"];
		FractionToken **tFractionTokens = new FractionToken *[tTokenNumber];

		for(int i = 0 ; i < tTokenNumber ; i++){
			tFractionTokens[i] = (FractionToken *)pDecoratorManager.loadDecorator(tTokenString[i]);
		}

		return new decorators::TokenModel(pDecoratorManager, (FractionToken **) tFractionTokens, tTokenNumber);

	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::TokenModel::TokenModel (DecoratorManager &pDecoratorManager, FractionToken **pTokens, int pTokenNumbers):
		FiducialDecorator(pDecoratorManager,0),
		mTokens(pTokens),
		mTokenNumbers(pTokenNumbers),
		mTotalActiveTokens(0),
		mActiveFirstToken(0),
		mActiveSecondToken(0),
		mActiveTokens(0),
		mTokenFirstQuadrant(0),
		mTokenSecondQuadrant(0),
		mTokenThirdQuadrant(0),
		mTokenFourthQuadrant(0){

			for(int i = 0 ; i < mTokenNumbers ; i++){
				switch(mTokens[i]->mType){
				case 0: mFirstTokens.push_back(mTokens[i]); break;
				case 1: mSecondTokens.push_back(mTokens[i]); break;
				}
			}
}

decorators::TokenModel::~TokenModel(){
	delete [] mTokens;
}

/*
 * First it takes the number of active tokens of each type (color)
 * Then, the quadrant is checked
 */
void decorators::TokenModel::update(){
	mTotalActiveTokens = 0;
	mActiveFirstToken = 0;
	mActiveSecondToken = 0;
	mTokenFirstQuadrant = 0;
	mTokenSecondQuadrant = 0;
	mTokenThirdQuadrant = 0;
	mTokenFourthQuadrant = 0;
	mActiveTokens.clear();

	// For each token that is present, we see if is yellow or blue
	for(int i = 0; i < mTokenNumbers; i++){
		if(mTokens[i]->IsPresent()){
			mTotalActiveTokens++;
			mActiveTokens.push_back(mTokens[i]);
			if(mTokens[i]->GetType() == 0) mActiveFirstToken++;
			else mActiveSecondToken++;
		}
	}

	// The number of tokens that is in each quadrant is calculated.
	for(std::vector<FractionToken *>::iterator it = mActiveTokens.begin() ; it != mActiveTokens.end() ; ++it){
		switch((*it)->mQuadrant){
			case 1: mTokenFirstQuadrant++; break;
			case 2: mTokenSecondQuadrant++; break;
			case 3: mTokenThirdQuadrant++; break;
			case 4: mTokenFourthQuadrant++; break;
		}
	}
}

/*
 * Check if the model is present in any position.
 * @return true if at least one token is present
*/
bool decorators::TokenModel::IsPresent(){
	return (mTotalActiveTokens > 0);
}

/*
 * Check if the model is present in one quadrant
 * @param the quadrant's number (1, 2, 3 or 4)
 * @return true if the model has at least one token
 * in that quadrant
 */
bool decorators::TokenModel::IsPresent(int pQuadrant){
	switch(pQuadrant){
		case 1: return (mTokenFirstQuadrant > 0);
		case 2: return (mTokenSecondQuadrant > 0);
		case 3: return (mTokenThirdQuadrant > 0);
		case 4: return (mTokenFourthQuadrant > 0);
		default: return false;
	}
}

/*
 * Returns the position of the model
 * @return a point2d<float> which correspond of the mean
 * in each axis (X and Y) of all the tokens
 */
wykobi::point2d<float> decorators::TokenModel::GetPosition(){
	float tX = 0.0f;
	float tY = 0.0f;
	float tMeanX = 0.0f;
	float tMeanY = 0.0f;

	// Check all the tokens that are active, takes the X and Y values
	for(std::vector<FractionToken *>::iterator it = mActiveTokens.begin(); it != mActiveTokens.end() ; ++it){
		tX += (*it)->GetLocation().x;
		tY += (*it)->GetLocation().y;
	}

	// Take the mean of X and Y
	tMeanX = (mTotalActiveTokens > 0)? (float)(tX/mTotalActiveTokens) : -1.0;
	tMeanY = (mTotalActiveTokens > 0)? (float)(tY/mTotalActiveTokens) : -1.0;

	return wykobi::make_point(tMeanX,tMeanY);
}

/*
 * Check if the tokens are in more than one quadrant
 * @return true if there is more than one quadrant that
 * has tokens
 */
bool decorators::TokenModel::AreTokensSpread(){
	int tQuadrants = 0;
	if(mTokenFirstQuadrant != 0) tQuadrants++;
	if(mTokenSecondQuadrant != 0) tQuadrants++;
	if(mTokenThirdQuadrant != 0) tQuadrants++;
	if(mTokenFourthQuadrant != 0) tQuadrants++;

	return (tQuadrants > 1);
}

/*
 * Check the proportion that is related to one quadrant
 * @param number of quadrant to check (1, 2, 3 or 4)
 * @return a float with the proportion that corresponds to
 * that quadrant
 */
float decorators::TokenModel::GetProportion(int pQuadrant){
	int tFirstToken = 0;
	int tTotalToken = 0;

	// If the quadrant doesn't have tokens in the quadrant, then
	// return 0.0
	switch(pQuadrant){
		case 1: if(mTokenFirstQuadrant == 0) return (0.0f); break;
		case 2:	if(mTokenSecondQuadrant == 0) return (0.0f); break;
		case 3: if(mTokenThirdQuadrant == 0) return (0.0f); break;
		case 4: if(mTokenFourthQuadrant == 0) return (0.0f); break;
	}

	// The total token is calculated (denominator) and the tokens
	// that corresponds to the yellow token (numerator)
	for(std::vector<FractionToken *>::iterator it = mActiveTokens.begin(); it != mActiveTokens.end() ; ++it){
		if((*it)->mQuadrant == pQuadrant){
			tTotalToken++;
			if((*it)->mType == 0) tFirstToken++;
		}
	}

	// Calculate the proportion
	if(tTotalToken !=0) return (float)(tFirstToken/(float)tTotalToken);
	else return 0.0f;
}

/*
 * Get the numerator of a quadrant
 * @param number of the quadrant (1, 2, 3, or 4)
 * @return the numerator of the quadrant
 */
int decorators::TokenModel::GetNumerator(int pQuadrant){
	int tFirstToken = 0;
	int tTotalToken = 0;

	// If there's no token there, then return 0
	switch(pQuadrant){
		case 1: if(mTokenFirstQuadrant == 0) return (0); break;
		case 2:	if(mTokenSecondQuadrant == 0) return (0); break;
		case 3: if(mTokenThirdQuadrant == 0) return (0); break;
		case 4: if(mTokenFourthQuadrant == 0) return (0); break;
	}

	for(std::vector<FractionToken *>::iterator it = mActiveTokens.begin(); it != mActiveTokens.end() ; ++it){
		if((*it)->mQuadrant == pQuadrant){
			tTotalToken++;
			if((*it)->mType == 0) tFirstToken++;
		}
	}

	// Return the number of tokens that are yellow (first token)
	if(tTotalToken !=0) return tFirstToken;
	else return 0;
}

/*
 * Get the numerator of a quadrant
 * @param pQuadrant number of the quadrant (1, 2, 3, or 4)
 * @return the denominator of the quadrant
 */
int decorators::TokenModel::GetDenominator(int pQuadrant){
	int tFirstToken = 0;
	int tTotalToken = 0;

	// If there's no token there, then return 0
	switch(pQuadrant){
		case 1: if(mTokenFirstQuadrant == 0) return (0); break;
		case 2:	if(mTokenSecondQuadrant == 0) return (0); break;
		case 3: if(mTokenThirdQuadrant == 0) return (0); break;
		case 4: if(mTokenFourthQuadrant == 0) return (0); break;
	}

	for(std::vector<FractionToken *>::iterator it = mActiveTokens.begin(); it != mActiveTokens.end() ; ++it){
		if((*it)->mQuadrant == pQuadrant){
			tTotalToken++;
			if((*it)->mType == 0) tFirstToken++;
		}
	}

	// Return the number of all tokens
	if(tTotalToken !=0) return tTotalToken;
	else return 0;
}

