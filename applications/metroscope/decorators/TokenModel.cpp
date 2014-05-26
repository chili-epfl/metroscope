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

		return new decorators::TokenModel(pDecoratorManager, tFractionTokens, tTokenNumber);

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
		mTokenFirstCuadrant(0),
		mTokenSecondCuadrant(0),
		mTokenThirdCuadrant(0),
		mTokenFourthCuadrant(0){

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

void decorators::TokenModel::update(){
	mTotalActiveTokens = 0;
	mActiveFirstToken = 0;
	mActiveSecondToken = 0;
	mTokenFirstCuadrant = 0;
	mTokenSecondCuadrant = 0;
	mTokenThirdCuadrant = 0;
	mTokenFourthCuadrant = 0;

	for(int i = 0; i < mTokenNumbers; i++){
		if(mTokens[i]->IsPresent()){
			mTotalActiveTokens++;
			mActiveTokens.push_back(mTokens[i]);
			if(mTokens[i]->getType() == 0) mActiveFirstToken++;
			else mActiveSecondToken++;
		}
	}

	for(std::vector<FractionToken *>::iterator it = mActiveTokens.begin() ; it != mActiveTokens.end() ; ++it){
		switch((*it)->mCuadrant){
			case 1: mTokenFirstCuadrant++; break;
			case 2: mTokenSecondCuadrant++; break;
			case 3: mTokenThirdCuadrant++; break;
			case 4: mTokenFourthCuadrant++; break;
		}
	}
}

bool decorators::TokenModel::isPresent(){
	return (mTotalActiveTokens > 0);
}

wykobi::point2d<float> decorators::TokenModel::GetPosition(){
	wykobi::point2d<float> tLocation;
	float tX = 0.0f;
	float tY = 0.0f;
	float tMeanX = 0.0f;
	float tMeanY = 0.0f;

	for(std::vector<FractionToken *>::iterator it = mActiveTokens.begin(); it != mActiveTokens.end() ; ++it){
		tX += (*it)->GetLocation().x;
		tY += (*it)->GetLocation().y;
	}

	tMeanX = (mTotalActiveTokens > 0)? (float)(tX/mTotalActiveTokens) : -1.0;
	tMeanY = (mTotalActiveTokens > 0)? (float)(tY/mTotalActiveTokens) : -1.0;

	tLocation.x = tMeanX;
	tLocation.y = tMeanY;

	return tLocation;
}

bool decorators::TokenModel::AreTokensSpread(){
	int tCuadrants = 0;
	if(mTokenFirstCuadrant != 0) tCuadrants++;
	if(mTokenSecondCuadrant != 0) tCuadrants++;
	if(mTokenThirdCuadrant != 0) tCuadrants++;
	if(mTokenFourthCuadrant != 0) tCuadrants++;

	return (tCuadrants > 1);
}

