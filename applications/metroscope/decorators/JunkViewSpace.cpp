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

#include "JunkViewSpace.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>
#include <wykobi/wykobi_utilities.hpp>

const std::string decorators::JunkViewSpace::scDecoratorName("JunkViewSpace");
const DecoratorManager::Registerer decorators::JunkViewSpace::mRegisterer(decorators::JunkViewSpace::scDecoratorName, &decorators::JunkViewSpace::create);

decorators::FiducialDecorator *decorators::JunkViewSpace::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::JunkViewSpace(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), (const JunkModel *) pDecoratorManager.loadDecorator(pSetting["junkmodel"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::JunkViewSpace::JunkViewSpace(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const JunkModel *pModel):
FiducialDecorator(pDecoratorManager, pMarker),
mModel(pModel)
{
}

decorators::JunkViewSpace::~JunkViewSpace()
{
}

void decorators::JunkViewSpace::update() {
	if (mMarker->isPresent())
	{
		for(int i=0; i < mModel->GetNJunks(); ++i)
		{
			JunkModel::JunkState tState = mModel->GetJunkState(i);
			if (tState == JunkModel::TARGET || tState == JunkModel::DESTROYED)
			{
				wykobi::quadix<float ,2> tMarkerCorners = mMarker->getCorners();
				mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
				wykobi::point2d<float> tOrigin;
				wykobi::point2d<float> tXUnit;
				wykobi::point2d<float> tYUnit;
				FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

				float tJunkX = mModel->GetJunkX(i);
				float tJunkY = mModel->GetJunkY(i);
				wykobi::point2d<float> tJunkCenter = tOrigin + tJunkX*tXUnit + tJunkY*tYUnit;
				wykobi::point2d<float> tJunkBorderPoint = tJunkCenter + mModel->GetJunkRadius(i)*tXUnit;
				mDecoratorManager.GetWorld2Proj().InterpolatedMap(tJunkCenter);
				mDecoratorManager.GetWorld2Proj().InterpolatedMap(tJunkBorderPoint);
				float tRadius = wykobi::distance(tJunkCenter, tJunkBorderPoint);
				if (tState == JunkModel::TARGET)
				{
					mDecoratorManager.GetDisplay().RenderFilledEllipse(tJunkCenter.x, tJunkCenter.y, tRadius, tRadius, 1.0f, 0.0f, 0.0f);
				}
				else
				{
					mDecoratorManager.GetDisplay().RenderFilledEllipse(tJunkCenter.x, tJunkCenter.y, tRadius, tRadius, 0.0f, 1.0f, 0.0f);
				}
			}
		}
		
		static const long scLaserShotDuration = 3000l;
		long tElapsedTime = Time::MillisTimestamp() - mModel->GetLastLaserShotTimestamp();
		if ( tElapsedTime < scLaserShotDuration)
		{
			mDecoratorManager.GetDisplay().PushTransformation();
			mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(*mMarker, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
			mDecoratorManager.GetDisplay().Rotate(mModel->GetLaserOffset());
			mDecoratorManager.GetDisplay().Translate(JunkModel::scEarthRadius, 0.0f);
			mDecoratorManager.GetDisplay().Rotate(270.0f);
			mDecoratorManager.GetDisplay().Rotate(mModel->GetLastLaserShotAngle());
			mDecoratorManager.GetDisplay().RenderLine(0.0f, 0.0f, 130.0f, 0.0f, 0.0f, 0.0f, 1.0f, (1.0f-tElapsedTime/(float)scLaserShotDuration));
			mDecoratorManager.GetDisplay().PopTransformation();
		}
	}
}
