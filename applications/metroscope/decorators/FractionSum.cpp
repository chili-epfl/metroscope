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

#include "FractionSum.hpp"
#include <iostream>

const std::string decorators::FractionSum::scDecoratorName ("FractionSum");
const DecoratorManager::Registerer decorators::FractionSum::mRegisterer(decorators::FractionSum::scDecoratorName, &decorators::FractionSum::create);

decorators::FiducialDecorator *decorators::FractionSum::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager){
	try{
		int tManipulativeNumber = pSetting ["circular_number"];
		CircularFractionModel ** tCircularManipulatives = new CircularFractionModel *[tManipulativeNumber];
		libconfig::Setting &tManipulativeString = pSetting ["circular_manipulatives"];

		for(int i = 0 ; i < tManipulativeNumber ; i++){
			tCircularManipulatives[i] = (CircularFractionModel *) pDecoratorManager.loadDecorator(tManipulativeString[i]);
		}
		return new decorators::FractionSum (pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),tCircularManipulatives, (int)tManipulativeNumber);
	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::FractionSum::FractionSum (DecoratorManager &pDecoratorManager,FiducialMarker *pMarker, CircularFractionModel** pCircularModel, int pCircularManipulatives):
		FiducialDecorator(pDecoratorManager, pMarker),
		mCircularModel(pCircularModel), mCircularNumber(pCircularManipulatives),
		mSum(0.0), mNumerator(0), mDenominator(1), mActivityPresent(false), mActiveCircularModel(0){

}

void decorators::FractionSum::update(){
	 if(mMarker->isPresent()){
		CheckManipulativesPresent();
		float tAngle = 0.0f;
		int tCircleNumber = (int)mSum;

		mDecoratorManager.GetDisplay().PushTransformation();
		for(int i = 0 ; i < tCircleNumber + 1; i++){
			mDecoratorManager.GetDisplay().RenderEllipse(300.0f + 300.0*i,300.0f,90.0f,90.0f,0.0f,0.0f,0.0f,1.0f,1);
		}
		mDecoratorManager.GetDisplay().PopTransformation();

		/*
		mDecoratorManager.GetDisplay().PushTransformation();
		for(std::vector<CircularFractionModel *>::iterator it = mActiveCircularModel.begin() ; it != mActiveCircularModel.end() ; ++it){
			mDecoratorManager.GetDisplay().RenderFilledSector(300.0f,300.0f,90.0f,90.0f,(360.0 -(*it)->GetAngle()),90.0f + tAngle,tAngle+0.1, 0.0f,0.0f,1.0f,2);
			tAngle += (360 - (*it)->GetAngle());
		}
		mDecoratorManager.GetDisplay().PopTransformation();*/

	}
}

void decorators::FractionSum::CheckManipulativesPresent(){
	mActiveCircularModel.clear();
	mSum = 0.0;
	for(int i = 0 ; i < mCircularNumber ; i++){
		if(mCircularModel[i]->IsPresent()){
			mActiveCircularModel.push_back(mCircularModel[i]);
			mSum += mCircularModel[i]->GetProportion();
		}
	}
}
