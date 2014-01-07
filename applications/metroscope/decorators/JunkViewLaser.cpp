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

#include "JunkViewLaser.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>
#include <wykobi/wykobi_utilities.hpp>

const std::string decorators::JunkViewLaser::scDecoratorName("JunkViewLaser");
const DecoratorManager::Registerer decorators::JunkViewLaser::mRegisterer(decorators::JunkViewLaser::scDecoratorName, &decorators::JunkViewLaser::create);

decorators::FiducialDecorator *decorators::JunkViewLaser::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::JunkViewLaser(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), (const JunkModel *) pDecoratorManager.loadDecorator(pSetting["junkmodel"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::JunkViewLaser::JunkViewLaser(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const JunkModel *pModel):
FiducialDecorator(pDecoratorManager, pMarker),
mModel(pModel)
{
}

void decorators::JunkViewLaser::update() {
	if (mMarker->isPresent())
	{
		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(*mMarker, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());

		static const float scGrayLevel = 0.5f;
		static const float scTubeRadius = 0.8f;
		static const float scBaseRadius = 2.0f*scTubeRadius;
		static const float scPseudoPerspective = 0.375f;
		static const float scTubeLength = 3.5f;
		static const float scLaserEnd = 30.0f;

		long tLastLaserShotTimestamp = mModel->GetLastLaserShotTimestamp();
		static const long scLaserShotDuration = 3000l;
		long tElapsedTime = Time::MillisTimestamp() - tLastLaserShotTimestamp;
		if ( tElapsedTime < scLaserShotDuration)
		{
			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().Rotate(mModel->GetLastLaserShotAngle());
			mDecoratorManager.GetDisplay().RenderQuadFilled(
					0.0f, scTubeRadius,
					scLaserEnd, scTubeRadius,
					scLaserEnd, -scTubeRadius,
					0.0f, -scTubeRadius,
					0.0f, 0.0f, 1.0f, (1.0f-tElapsedTime/(float)scLaserShotDuration));
			mDecoratorManager.GetDisplay().RenderFilledEllipse(scLaserEnd, 0.0f, scPseudoPerspective*scTubeRadius, scTubeRadius, 0.0f, 0.0f, 1.0f, (1.0f-tElapsedTime/(float)scLaserShotDuration));
			mDecoratorManager.GetDisplay().PopTransformation();
		}

		float tCurrentLaserAngle = mModel->GetCurrentLaserAngle();
		if (0.0f <= tCurrentLaserAngle && tCurrentLaserAngle <= 180.0f)
		{
			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().Rotate(tCurrentLaserAngle);
			mDecoratorManager.GetDisplay().RenderQuadFilled(0.0f, -scTubeRadius, 0.0f, scTubeRadius, scTubeLength, scTubeRadius, scTubeLength, -scTubeRadius, scGrayLevel, scGrayLevel, scGrayLevel);
			mDecoratorManager.GetDisplay().RenderFilledEllipse(0.0f, 0.0f, scBaseRadius, scBaseRadius, scGrayLevel, scGrayLevel, scGrayLevel);
			mDecoratorManager.GetDisplay().PopTransformation();
		}

		mDecoratorManager.GetDisplay().RenderEllipse(0.0f, -5.0f-scTubeRadius, scTubeRadius, scTubeRadius, 0.0f, 0.0f, 0.0f);
		char tText[64];
		sprintf(tText, "%d", 3-mModel->GetCurrentLaserNumber()); //FIXME
		mDecoratorManager.GetDisplay().RenderText(tText, -0.4f, -5.3f, 0.05f);
		mDecoratorManager.GetDisplay().RenderLine(0.0f, -1.0f, 0.0f, -0.0f, 0.0f, 0.0f, 0.0f);
		mDecoratorManager.GetDisplay().RenderLine(0.0f, -3.0f, 0.0f, -2.0f, 0.0f, 0.0f, 0.0f);
		mDecoratorManager.GetDisplay().RenderLine(0.0f, -5.0f, 0.0f, -4.0f, 0.0f, 0.0f, 0.0f);
		mDecoratorManager.GetDisplay().RenderLine(-10.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		mDecoratorManager.GetDisplay().PopTransformation();
	}
}
