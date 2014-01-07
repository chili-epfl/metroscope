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

#include "JunkModel.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>

const std::string decorators::JunkModel::scDecoratorName("JunkModel");
const  DecoratorManager::Registerer decorators::JunkModel::mRegisterer(decorators::JunkModel::scDecoratorName, &decorators::JunkModel::create);

const float decorators::JunkModel::scEarthRadius = 1.5f;
const float decorators::JunkModel::scAngleOffsets[scNLasers] = {210.0f, 330.0f, 90.0f};

decorators::FiducialDecorator *decorators::JunkModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tJunks = pSetting["junk"];
		int tNJunk = tJunks.getLength();
		JunkModel *tJunkModel = new JunkModel(pDecoratorManager, tNJunk, (AngleModel *) pDecoratorManager.loadDecorator(pSetting["angle"]), (JunkLaserSelector *) pDecoratorManager.loadDecorator(pSetting["laserselector"]));
		for (int i = 0; i < tNJunk; ++i) {
			libconfig::Setting & tJunk = tJunks[i];
			tJunkModel->SetJunk(i, tJunk[0], tJunk[1], tJunk[2]);
		}
		return tJunkModel;
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::JunkModel::JunkModel(DecoratorManager &pDecoratorManager, int pNJunks, const AngleModel *pAngle, JunkLaserSelector *pCurrentLaser):
FiducialDecorator(pDecoratorManager, 0),
mNJunks(pNJunks),
mJunkRadiuses(new float[pNJunks]),
mJunkXs(new float[pNJunks]),
mJunkYs(new float[pNJunks]),
mJunkStates(new JunkState[pNJunks]),
mCurrentJunk(0),
mAngle(pAngle),
mCurrentLaser(pCurrentLaser),
mLastLaserShotTimestamp(0l),
mLastLaserShotAngle(0.0f)
{
	mJunkStates[0]=TARGET;
	for (int i=1; i<mNJunks; ++i) mJunkStates[i]=WAITING;
}

decorators::JunkModel::~JunkModel()
{
	delete []mJunkStates;
	delete []mJunkYs;
	delete []mJunkXs;
	delete []mJunkRadiuses;
}

void decorators::JunkModel::SetJunk(int index, float pRadius, float pX, float pY)
{
	mJunkRadiuses[index] = pRadius;
	mJunkXs[index] = pX;
	mJunkYs[index] = pY;
}


void decorators::JunkModel::Shoot()
{
	int tSelectedLaser = mCurrentLaser->Selection();
	if (mAngle->Angle() < 0.0f || mAngle->Angle() > 180.0f) return;
	mLastLaserShotTimestamp = Time::MillisTimestamp();
	mLastLaserShotAngle = mAngle->Angle();
	AngleModel::NormalizeAngle(mLastLaserShotAngle);
	if (mCurrentJunk < mNJunks)
	{
		float tLaserX = scEarthRadius*cos(scAngleOffsets[tSelectedLaser]*wykobi::PIDiv180);
		float tLaserY = scEarthRadius*sin(scAngleOffsets[tSelectedLaser]*wykobi::PIDiv180);
		wykobi::vector2d<float> tJunkVector = wykobi::make_vector(mJunkXs[mCurrentJunk]-tLaserX, mJunkYs[mCurrentJunk]-tLaserY);
		float tErrorMargin = atan2(mJunkRadiuses[mCurrentJunk], wykobi::vector_norm(tJunkVector))*wykobi::_180DivPI;
		float tJunkAngle = wykobi::cartesian_angle(tJunkVector) - scAngleOffsets[tSelectedLaser] + 90.0f;
		float tDifference = AngleModel::AbsoluteDifference(mLastLaserShotAngle, tJunkAngle);

		if (tDifference <= tErrorMargin)
		{
			mJunkStates[mCurrentJunk++] = DESTROYED;
			if (mCurrentJunk < mNJunks) mJunkStates[mCurrentJunk] = TARGET;
		}
	}
}
