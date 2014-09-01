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

#include "FractionLearningModel.hpp"
#include <iostream>

const std::string decorators::FractionLearningModel::scDecoratorName("FractionLearningModel");
const DecoratorManager::Registerer decorators::FractionLearningModel::mRegisterer(decorators::FractionLearningModel::scDecoratorName , &decorators::FractionLearningModel::create);

decorators::FiducialDecorator *decorators::FractionLearningModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try{

		int tConstructionNb = pSetting ["nb_construction_activities"];
		libconfig::Setting &tConstructionString = pSetting ["fraction_construction"];

		FractionViewMeasure ** tConstructionActivities = new FractionViewMeasure *[tConstructionNb];


		for( int i = 0 ; i < tConstructionNb ; i++){
			tConstructionActivities[i] = (FractionViewMeasure *) pDecoratorManager.loadDecorator(tConstructionString[i]);
		}

		return new decorators::FractionLearningModel(pDecoratorManager ,
				(KillBugModel *) pDecoratorManager.loadDecorator(pSetting["kill_bug_game"]),
				(FractionComparisionView *) pDecoratorManager.loadDecorator(pSetting["fraction_comparison_greater"]),
				(FractionComparisionView *) pDecoratorManager.loadDecorator(pSetting["fraction_comparison_smaller"]),
				tConstructionActivities,
				(EquivalentFractionView *) pDecoratorManager.loadDecorator(pSetting["fraction_equivalent"]),
				(int) tConstructionNb);

	}catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::FractionLearningModel::FractionLearningModel(DecoratorManager &pDecoratorManager , KillBugModel *pKillBugModel , FractionComparisionView *pComparisonGreater,
		FractionComparisionView *pComparisonSmaller, FractionViewMeasure **pFractionView, EquivalentFractionView *pEquivalentFraction, int pConstructionActivity):
				FiducialDecorator(pDecoratorManager,0),
				mFractionConstruction(pFractionView),
				mFractionEquivalent(pEquivalentFraction),
				mFractionComparisonGreater(pComparisonGreater),
				mFractionComparisonSmaller(pComparisonSmaller),
				mKillBugGame(pKillBugModel),
				mConstructionActivity(pConstructionActivity)

	{

	}

decorators::FractionLearningModel::~FractionLearningModel(){}

void decorators::FractionLearningModel::update(){
	mKillBugGame->SetCurrentActivity(false);
	SetFractionComparison(false);
	SetFractionConstruction(false);
	mFractionEquivalent->SetCurrentActivity(false);

	if(mKillBugGame->IsPresent()){
		mKillBugGame->SetCurrentActivity(true);
		SetFractionComparison(false);
		SetFractionConstruction(false);
		mFractionEquivalent->SetCurrentActivity(false);
	}

	if(mFractionEquivalent->IsPresent()){
		mFractionEquivalent->SetCurrentActivity(true);
		mKillBugGame->SetCurrentActivity(false);
		SetFractionComparison(false);
		SetFractionConstruction(false);
	}

	if(IsPresentFractionComparison()){
		mKillBugGame->SetCurrentActivity(false);
		SetFractionConstruction(false);
		mFractionEquivalent->SetCurrentActivity(false);
	}

	if(IsPresentFractionConstruction()){
		mKillBugGame->SetCurrentActivity(false);
		SetFractionComparison(false);
		mFractionEquivalent->SetCurrentActivity(false);
	}
}

bool decorators::FractionLearningModel::IsPresentFractionComparison(){
	if(mFractionComparisonGreater->IsPresent()){
		mFractionComparisonGreater->SetCurrentActivity(true);
		mFractionComparisonSmaller->SetCurrentActivity(false);
	}else if(mFractionComparisonSmaller->IsPresent()){
		mFractionComparisonGreater->SetCurrentActivity(false);
		mFractionComparisonSmaller->SetCurrentActivity(true);
	}
	return (mFractionComparisonGreater->IsPresent() || mFractionComparisonSmaller->IsPresent());
}

bool decorators::FractionLearningModel::IsPresentFractionConstruction(){
	for(int i = 0 ; i < mConstructionActivity ; i++){
		if(mFractionConstruction[i]->IsPresent()){
			mFractionConstruction[i]->SetCurrentActivity(true);
			return true;
		}
	}
	return false;
}

void decorators::FractionLearningModel::SetFractionComparison(bool pIsCurrentActivity){
	mFractionComparisonGreater->SetCurrentActivity(pIsCurrentActivity);
	mFractionComparisonSmaller->SetCurrentActivity(pIsCurrentActivity);
}

void decorators::FractionLearningModel::SetFractionConstruction(bool pIsCurrentActivity){
	for(int i = 0 ; i < mConstructionActivity ; i++){
		mFractionConstruction[i]->SetCurrentActivity(pIsCurrentActivity);
	}
}
