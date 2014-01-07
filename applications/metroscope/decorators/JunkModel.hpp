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

#ifndef JunkModel_HPP
#define JunkModel_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "AngleModel.hpp"
#include "JunkLaserSelector.hpp"
#include <fstream>

namespace decorators {

class JunkModel : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		JunkModel(DecoratorManager &pDecoratorManager, int pNJunks, const AngleModel *pAngle, JunkLaserSelector *pCurrentLaser);
		void SetJunk(int index, float pRadius, float pX, float pY);
		void SetLaser(int index, const AngleModel *pAngle, float pOffset);
		virtual ~JunkModel();
		
		enum JunkState {WAITING, TARGET, DESTROYED};
		
		int GetNJunks() const {return mNJunks;}
		float GetJunkRadius(int pIndex) const {return mJunkRadiuses[pIndex];}
		float GetJunkX(int pIndex) const {return mJunkXs[pIndex];}
		float GetJunkY(int pIndex) const {return mJunkYs[pIndex];}
		JunkState GetJunkState(int pIndex) const {return mJunkStates[pIndex];}

		static const float scEarthRadius;

		int GetCurrentLaserNumber() const {return mCurrentLaser->Selection();}
		float GetLaserOffset() const {return scAngleOffsets[mCurrentLaser->Selection()];}
		long GetLastLaserShotTimestamp() const {return mLastLaserShotTimestamp;}
		float GetLastLaserShotAngle() const {return mLastLaserShotAngle;}
		float GetCurrentLaserAngle() const {return mAngle->isPresent()?mAngle->Angle():90.0f;}

		void Shoot();

	protected:
		void update(){}

		static const int scNLasers = 3;
		static const float scAngleOffsets[scNLasers];

		const int mNJunks;
		float *mJunkRadiuses;
		float *mJunkXs;
		float *mJunkYs;
		JunkState *mJunkStates;
		int mCurrentJunk;
		const AngleModel *mAngle;
		const JunkLaserSelector *mCurrentLaser;
		long mLastLaserShotTimestamp;
		float mLastLaserShotAngle;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		JunkModel(const JunkModel&);
		JunkModel& operator=(const JunkModel&);
};

}
#endif
