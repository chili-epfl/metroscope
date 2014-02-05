/*******************************************************************************
*   Copyright 2014 EPFL                                                        *
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

#include "AnalysisChecker.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>
#include <wykobi/wykobi_utilities.hpp>

const std::string decorators::AnalysisChecker::scDecoratorName("AnalysisChecker");
const DecoratorManager::Registerer decorators::AnalysisChecker::mRegisterer(decorators::AnalysisChecker::scDecoratorName, &decorators::AnalysisChecker::create);

decorators::FiducialDecorator *decorators::AnalysisChecker::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::AnalysisChecker(
				pDecoratorManager,
				(Flipper *) pDecoratorManager.loadDecorator(pSetting["flipper"]),
				(SentenceAnalysisModel *) pDecoratorManager.loadDecorator(pSetting["analysismodel"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::AnalysisChecker::AnalysisChecker(DecoratorManager &pDecoratorManager, Flipper *pFlipper, SentenceAnalysisModel *pModel):
FiducialDecorator(pDecoratorManager, 0),
mFlipper(pFlipper),
mModel(pModel),
mLastShot(Time::MillisTimestamp())
{
}

void decorators::AnalysisChecker::update()
{
	static const float scX1 = -1.0f;
	static const float scX2 = 4.5f;
	static const float scY1 = 6.5f;
	static const float scY2 = 7.0f;

	static const long cShotPreparationTime = 20l*1000l;
	long tElapsedTime = Time::MillisTimestamp() - mLastShot;
	if (mFlipper->IsFlipped() && tElapsedTime > cShotPreparationTime) {
		mModel->Correct();
		mLastShot = Time::MillisTimestamp();
	}

	if (mFlipper->IsPresent())
	{
		float tPartialX2 = scX1+(scX2-scX1)*(tElapsedTime/(float)cShotPreparationTime);
		bool tFull = false;
		if (tPartialX2 >= scX2)
		{
			tPartialX2 = scX2;
			tFull = true;
		}

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(*mFlipper->GetCurrentSide(), mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
		mDecoratorManager.GetDisplay().RenderQuadFilled(
				scX1, scY1,
				tPartialX2, scY1,
				tPartialX2, scY2,
				scX1, scY2,
				tFull?0.0f:1.0f, 1.0f, 0.0f);
		mDecoratorManager.GetDisplay().RenderText(tFull?"Prêt!":"En charge...", tFull?0.5f:-1.0f, 6.0f, 0.04f);
		mDecoratorManager.GetDisplay().PopTransformation();
	}
}

