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

#ifndef DecoratorManager_HPP
#define DecoratorManager_HPP

#include <qa/pipeables/Pipeable.hpp>
#include <qa/components/vision/Craftag.hpp>
#include <qa/components/decorators/FiducialDecorator.hpp>
#include <qa/components/vision/FiducialMarker.hpp>
#include <qa/components/vision/CraftagRegistrar.hpp>
#include <qa/components/vision/PlaneCoordinateMapper.hpp>
#include <qa/pipeables/io/OpenGl2DDisplay.hpp>
#include <qa/components/misc/NetworkedStateManager.hpp>
#include <libconfig.h++>
#include <list>
#include <map>

class DecoratorManager : public Pipeable
{
public:
	class Registerer {
	public:
		Registerer(const std::string &pDecoratorName, decorators::FiducialDecorator * (*pFactory)(libconfig::Setting &, DecoratorManager &))
		{
			DecoratorManager::registerFactory(pDecoratorName, pFactory);
		}
	private:
		Registerer(){}
	};

	static void registerFactory(const std::string &pDecoratorName, decorators::FiducialDecorator * (*pFactory)(libconfig::Setting &, DecoratorManager &));

	DecoratorManager(
		const char *pActivitiesDirectory,
		OpenGl2DDisplay &pDisplay,
		const PlaneCoordinateMapper &pCam2World,
		const PlaneCoordinateMapper &pWorld2Proj,
		const IplImage *const *pInputImage,
		const char *pLogId = 0);
	virtual ~DecoratorManager();

	FiducialMarker *loadMarker(libconfig::Setting &tMarkerSettings);
	decorators::FiducialDecorator *loadDecorator(libconfig::Setting &pSetting);
	decorators::FiducialDecorator *getDecorator(const std::string &pFullName) {return mDecorators[pFullName];}

    OpenGl2DDisplay & GetDisplay() { return mDisplay;}
    const PlaneCoordinateMapper & GetCam2World() { return mCam2World;}
    const PlaneCoordinateMapper & GetWorld2Proj() { return mWorld2Proj;}
	const IplImage *const *GetInputImage(){return mInputImage;}
	const char *GetLogId() {return mLogId;}


protected:
	void run();

    std::string mCurrentConfigFile;
    libconfig::Config mConfig;
	std::map<std::string, FiducialMarker *> mMarkers;
	std::list<decorators::FiducialDecorator *> mOrderedDecorators;
	std::map<int, decorators::FiducialDecorator *> mMarkerMasks;
	std::map<std::string, decorators::FiducialDecorator *> mDecorators;
	static std::map<std::string, decorators::FiducialDecorator *(*)(libconfig::Setting &, DecoratorManager &)> *mDecoratorFactories;

	static int sNextAnonymousMarkerGUID;
	static int sNextAnonymousDecoratorGUID;

	FiducialMarker *loadCraftag(libconfig::Setting &pSetting);
	FiducialMarker *loadAggregated(libconfig::Setting &pSetting);
	FiducialMarker *loadCombined(libconfig::Setting &pSetting);
	FiducialMarker *loadStabilized(libconfig::Setting &pSetting);
	FiducialMarker *loadBistable(libconfig::Setting &pSetting);
	FiducialMarker *loadTranslated(libconfig::Setting &pSetting);
	FiducialMarker *loadRotated(libconfig::Setting &pSetting);
	FiducialMarker *loadProximity(libconfig::Setting &pSetting);
	FiducialMarker *loadLimitedTime(libconfig::Setting &pSetting);
	FiducialMarker *loadDummy(libconfig::Setting &pSetting);
	FiducialMarker *loadCached(libconfig::Setting &pSetting);
	FiducialMarker *loadFlipped(libconfig::Setting &pSetting);

    OpenGl2DDisplay &mDisplay;
    const PlaneCoordinateMapper &mCam2World;
    const PlaneCoordinateMapper &mWorld2Proj;
	const IplImage *const *mInputImage;
	const char *mLogId;

private:
	DecoratorManager();
	DecoratorManager(const DecoratorManager&);
	DecoratorManager& operator=(const DecoratorManager&);
};

#endif
