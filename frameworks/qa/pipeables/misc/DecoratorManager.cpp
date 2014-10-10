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

#include <sstream>
#include <cstring>
#include <iostream>
#include "DecoratorManager.hpp"
#include <qa/components/decorators/MarkerMask.hpp>
#include <qa/components/vision/Craftag.hpp>
#include <qa/components/vision/StabilizedFiducialMarker.hpp>
#include <qa/components/vision/BistableFiducialMarker.hpp>
#include <qa/components/vision/MergedFiducialMarker.hpp>
#include <qa/components/vision/AggregatedFiducialMarker.hpp>
#include <qa/components/vision/TranslatedFiducialMarker.hpp>
#include <qa/components/vision/RotatedFiducialMarker.hpp>
#include <qa/components/vision/ProximityMarker.hpp>
#include <qa/components/vision/LimitedTimeMarker.hpp>
#include <qa/components/vision/DummyMarker.hpp>
#include <qa/components/vision/CachedFiducialMarker.hpp>
#include <qa/components/vision/FlippedMarker.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>

int DecoratorManager::sNextAnonymousMarkerGUID = 0;
int DecoratorManager::sNextAnonymousDecoratorGUID = 0;
std::map<std::string, decorators::FiducialDecorator * (*)(libconfig::Setting &, DecoratorManager &)> *DecoratorManager::mDecoratorFactories = 0;

void DecoratorManager::registerFactory(const std::string &pDecoratorName, decorators::FiducialDecorator * (*pFactory)(libconfig::Setting &, DecoratorManager &))
{
	if (!mDecoratorFactories) mDecoratorFactories = new std::map<std::string, decorators::FiducialDecorator * (*)(libconfig::Setting &, DecoratorManager &)>();
	(*mDecoratorFactories)[pDecoratorName] = pFactory;
}

DecoratorManager::DecoratorManager(
	const char *pActivitiesDirectory,
	OpenGl2DDisplay &pDisplay,
	const PlaneCoordinateMapper &pCam2World,
	const PlaneCoordinateMapper &pWorld2Proj,
	const IplImage *const *pInputImage,
	const char* pLogId ):
mMarkers(),
mOrderedDecorators(),
mDecorators(),
mDisplay(pDisplay),
mCam2World(pCam2World),
mWorld2Proj(pWorld2Proj),
mInputImage(pInputImage),
mLogId(pLogId)
{
    boost::filesystem::path directory(pActivitiesDirectory);
    if( exists( directory ) )
    {
        boost::filesystem::directory_iterator end;
        for( boost::filesystem::directory_iterator iter(directory) ; iter != end ; ++iter )
        {
            if ( is_regular( *iter ) && extension(*iter) == ".cfg" )
            {
            	mCurrentConfigFile = (*iter).path().string();
            	const char *tConfigFile = mCurrentConfigFile.c_str();
            	try {
            		mConfig.readFile( tConfigFile );
            	} catch(libconfig::FileIOException &) {
            		std::cerr << "Decorators configuration: FileIOException with "<< tConfigFile << std::endl;
            	} catch(libconfig::ParseException &e) {
            		std::cerr << mCurrentConfigFile << ", line "<< e.getLine() << ": " << e.getError() << std::endl;
            	}

            	try {
            		libconfig::Setting &tDecorators = mConfig.lookup("decorators");
					for (int i=0; i<tDecorators.getLength(); ++i)
					{
						loadDecorator(tDecorators[i]);
					}
            	}
            	catch (libconfig::SettingNotFoundException &e) {
					std::cerr << "Setting not found: " << e.getPath() << std::endl;
				}
            }
        }
    }
    else
    {
		std::cerr << "Directory \""<< pActivitiesDirectory << "\" does not exist" << std::endl;
    }
}

void DecoratorManager::run()
{
	for (std::list<decorators::FiducialDecorator *>::iterator it = mOrderedDecorators.begin(); it != mOrderedDecorators.end(); ++it) {
		(*it)->update();
	}
	for (std::map<int, decorators::FiducialDecorator *>::iterator it = mMarkerMasks.begin(); it != mMarkerMasks.end(); ++it) {
		(*it).second->update();
	}
}


DecoratorManager::~DecoratorManager()
{
	for (std::map<int, decorators::FiducialDecorator *>::iterator it = mMarkerMasks.begin(); it != mMarkerMasks.end(); ++it) {
		delete (*it).second;
	}
	for (std::list<decorators::FiducialDecorator *>::iterator it = mOrderedDecorators.begin(); it != mOrderedDecorators.end(); ++it) {
		delete (*it);
	}
	for (std::map<std::string, FiducialMarker *>::iterator it = mMarkers.begin(); it != mMarkers.end(); ++it) {
		delete (*it).second;
	}
}

FiducialMarker *DecoratorManager::loadMarker(libconfig::Setting &pMarkerSettings)
{
	std::string tMarkerName;
	libconfig::Setting *tMarkerSettings = &pMarkerSettings;
	if (tMarkerSettings->getType() == libconfig::Setting::TypeString) {
		std::string tNameSetting = pMarkerSettings;
		tMarkerName = tNameSetting;
		if (mMarkers.size() > 0)
		{
			std::map<std::string, FiducialMarker *>::iterator tResult = mMarkers.find(mCurrentConfigFile+tMarkerName);
			if (tResult != mMarkers.end() ) return (*tResult).second;
		}
		try
		{
			tMarkerSettings = &mConfig.lookup("markers."+tMarkerName);
		} catch(libconfig::SettingNotFoundException &e) {
			std::cerr << "Marker not found: " << e.getPath() << std::endl;
			return 0;
		}
	} else {
		std::stringstream tAnonymousGUID;
		tAnonymousGUID << "anonymous marker #" << sNextAnonymousMarkerGUID++;
		tMarkerName = tAnonymousGUID.str();
	}

	FiducialMarker* tMarker=0;
	try
	{
		std::string tMarkerType = (*tMarkerSettings)["type"];
		if (tMarkerType == "craftag") tMarker = loadCraftag(*tMarkerSettings);
		else if (tMarkerType == "combined") tMarker = loadCombined(*tMarkerSettings);
		else if (tMarkerType == "aggregated") tMarker = loadAggregated(*tMarkerSettings);
		else if (tMarkerType == "stabilized") tMarker = loadStabilized(*tMarkerSettings);
		else if (tMarkerType == "bistable") tMarker = loadBistable(*tMarkerSettings);
		else if (tMarkerType == "translated") tMarker = loadTranslated(*tMarkerSettings);
		else if (tMarkerType == "rotated") tMarker = loadRotated(*tMarkerSettings);
		else if (tMarkerType == "proximity") tMarker = loadProximity(*tMarkerSettings);
		else if (tMarkerType == "limitedtime") tMarker = loadLimitedTime(*tMarkerSettings);
		else if (tMarkerType == "dummy") tMarker = loadDummy(*tMarkerSettings);
		else if (tMarkerType == "cached") tMarker = loadCached(*tMarkerSettings);
		else if (tMarkerType == "flipped") tMarker = loadFlipped(*tMarkerSettings);
		else {
			std::cerr << "Unknown marker type: " << tMarkerType << std::endl;
			return 0;
		}
		mMarkers[mCurrentConfigFile+tMarkerName] = tMarker;
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Marker type not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Marker type not a string: " << e.getPath() << std::endl;
	}
	return tMarker;
}

FiducialMarker *DecoratorManager::loadCraftag(libconfig::Setting &pSetting)
{
	try {
		int tPersistence = 0;
		if (pSetting.exists("persistence"))
		{
			tPersistence = pSetting["persistence"];
		}
		int tId = pSetting["id"];
		Craftag *tMarker = new Craftag(tId, tPersistence);
		if (!mMarkerMasks[tId])
		{
			mMarkerMasks[tId] = new decorators::MarkerMask(*this, tMarker);
		}
		return tMarker;
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

FiducialMarker *DecoratorManager::loadCombined(libconfig::Setting &pSetting)
{
	MergedFiducialMarker *tMarker = new MergedFiducialMarker();
	try {
		libconfig::Setting & tComponentMarkers = pSetting["markers"];
		for (int i=0; i<tComponentMarkers.getLength(); ++i)
		{
			tMarker->addFiducialMarker(loadMarker(tComponentMarkers[i]));
		}
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return tMarker;
}

FiducialMarker *DecoratorManager::loadAggregated(libconfig::Setting &pSetting)
{
	AggregatedFiducialMarker *tMarker = 0;
	try {
		libconfig::Setting & tComponentMarkers = pSetting["markers"];
		int tNMarkers = tComponentMarkers.getLength();

		if (pSetting.exists("size"))
		{
			if (pSetting.exists("position"))
			{
				libconfig::Setting &tPosition = pSetting["position"];
				tMarker = new AggregatedFiducialMarker(pSetting["size"], tPosition[0], tPosition[1]);
			}
			else tMarker = new AggregatedFiducialMarker(pSetting["size"]);
		}
		else tMarker = new AggregatedFiducialMarker();

		for (int i=0; i<tNMarkers; ++i)
		{
			libconfig::Setting &tMarkerSettings = tComponentMarkers[i];
			libconfig::Setting &tMarkerPosition = tMarkerSettings["position"];
			if (tMarkerSettings.exists("size"))
			{
				tMarker->addFiducialMarker(loadMarker(tMarkerSettings["marker"]), tMarkerPosition[0], tMarkerPosition[1], tMarkerSettings["size"]);
			}
			else tMarker->addFiducialMarker(loadMarker(tMarkerSettings["marker"]), tMarkerPosition[0], tMarkerPosition[1]);
		}
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return tMarker;
}

FiducialMarker *DecoratorManager::loadStabilized(libconfig::Setting &pSetting)
{
	try {
		if (pSetting.exists("minTranslation"))
		{
			return new StabilizedFiducialMarker(*loadMarker(pSetting["marker"]), pSetting["minTranslation"]);
		}
		return new StabilizedFiducialMarker(*loadMarker(pSetting["marker"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

FiducialMarker *DecoratorManager::loadBistable(libconfig::Setting &pSetting)
{
	try {
		return new BistableFiducialMarker(*loadMarker(pSetting["marker"]), pSetting["low"], pSetting["high"]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

FiducialMarker *DecoratorManager::loadTranslated(libconfig::Setting &pSetting)
{
	try {
		libconfig::Setting &tTranslation = pSetting["translation"];
		return new TranslatedFiducialMarker(*loadMarker(pSetting["marker"]), tTranslation[0], tTranslation[1]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

FiducialMarker *DecoratorManager::loadRotated(libconfig::Setting &pSetting)
{
	try {
		return new RotatedFiducialMarker(*loadMarker(pSetting["marker"]), pSetting["rotation"]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

FiducialMarker *DecoratorManager::loadProximity(libconfig::Setting &pSetting)
{
	try {
		return new ProximityMarker(*loadMarker(pSetting["marker"]), *loadMarker(pSetting["markerToBeClose"]), pSetting["distance"]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

FiducialMarker *DecoratorManager::loadLimitedTime(libconfig::Setting &pSetting)
{
	try {
		return new LimitedTimeMarker(*loadMarker(pSetting["marker"]), pSetting["apparitions"]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

FiducialMarker *DecoratorManager::loadDummy(libconfig::Setting &pSetting)
{
	float x1 = 0.0f; float y1 = 0.0f;
	float x2 = 0.0f; float y2 = 0.0f;
	float x3 = 0.0f; float y3 = 0.0f;
	float x4 = 0.0f; float y4 = 0.0f;
	if (pSetting.exists("x1")) x1 = pSetting["x1"];
	if (pSetting.exists("y1")) y1 = pSetting["y1"];
	if (pSetting.exists("x2")) x2 = pSetting["x2"];
	if (pSetting.exists("y2")) y2 = pSetting["y2"];
	if (pSetting.exists("x3")) x3 = pSetting["x3"];
	if (pSetting.exists("y3")) y3 = pSetting["y3"];
	if (pSetting.exists("x4")) x4 = pSetting["x4"];
	if (pSetting.exists("y4")) y4 = pSetting["y4"];
	return new DummyMarker(x1, y1, x2, y2, x3, y3, x4, y4);
}

FiducialMarker *DecoratorManager::loadCached(libconfig::Setting &pSetting)
{
	try {
		return new CachedFiducialMarker(*loadMarker(pSetting["marker"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

FiducialMarker *DecoratorManager::loadFlipped(libconfig::Setting &pSetting)
{
	try {
		bool tOnLongEdge = true;
		if (pSetting.exists("onLongEdge")) tOnLongEdge = pSetting["onLongEdge"];
		return new FlippedMarker(*loadMarker(pSetting["marker"]), tOnLongEdge);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

/**
 * Creates and add a Decorator to the set of Decorators to update from a
 * libconfig Setting description.
 * 
 * The description can be a String/Setting pair
 * from the list of decorators, a String containing the of a Decorator, or an
 * anonmymous Setting defining a decorator inline.
 * 
 * If a decorator has the same name as the parameter, it is returned, and no
 * new decorator is created.
 * 
 * @param pDecoratorSettings The libconfig setting describing the decorator. 
 * @returns a pointer to the loaded Decorator, or a null pointer if a problem
 * occurred.
 */
decorators::FiducialDecorator *DecoratorManager::loadDecorator(libconfig::Setting &pDecoratorSettings)
{
  // We need a name at some point to store the decorator in the mDecorators
  // dictionnary. So this variable will have to be set.
	std::string tDecoratorName;

  // We store a reference to the parameter here, but we may have to update it
  //later to make it point to another Setting from the config file.
	libconfig::Setting *tDecoratorSettings = &pDecoratorSettings;
  
  // First, let's check if the setting is just the name of the decorator.
	if (tDecoratorSettings->getType() == libconfig::Setting::TypeString) {
		std::string tNameSetting = pDecoratorSettings;
		tDecoratorName = tNameSetting;
    // We append the name of the decorator to the filename of the path to avoid
    // collisions between decorators having the same name in different files.
    decorators::FiducialDecorator *tDecorator = mDecorators[mCurrentConfigFile+tDecoratorName];
    // If  decorator is already present in the dictionnary, that's easy:
		if (tDecorator) return tDecorator;
    // If the decorator has not been loaded yet, we'll do it now.
    // To do so, we'll fetch the setting containing the actual definition
    // and we'll go on as if we had received it as parameter.
		try
		{
			tDecoratorSettings = &mConfig.lookup("decorators."+tDecoratorName);
		} catch(libconfig::SettingNotFoundException &e) {
			std::cerr << "Named decorator not found: " << e.getPath() << std::endl;
			return 0;
		} catch(libconfig::SettingTypeException &e) {
			std::cerr << "Named decorator not a setting: " << e.getPath() << std::endl;
			return 0;
		}
  }
  // We don't have a String, so we assume that we have the definition of the
  // decorator. Now let's check if this definition is in the list of
  // decorators of the config file...
	else if (!std::strncmp(tDecoratorSettings->getParent().getName(), "decorators", 10+1)) {
    // In this case, the decorator should have a name already, so we'll use it.
		tDecoratorName = tDecoratorSettings->getName();
    // Let's check if the decorator has not been loaded already:
    // @Anna: that's what I think was missing, so that's what I added, so
    // that's what probably needs debuggin.
    decorators::FiducialDecorator *tDecorator = mDecorators[mCurrentConfigFile+tDecoratorName];
		if (tDecorator) return tDecorator;
	}
  // ... or embedded inside the definition of another decorator.
  else {
    // In this case there is no name for the decorator, so we generate one.
		std::stringstream tAnonymousGUID;
		tAnonymousGUID << "anonymous decorator #" << sNextAnonymousDecoratorGUID++;
		tDecoratorName = tAnonymousGUID.str();
	}

  // By now, we should have a name for the decorator, and the decorator has not
  // been loaded.
	decorators::FiducialDecorator* tDecorator=0;
	try {
    // The type will tell us which factory to call to create the object.
		std::string tDecoratorType = (*tDecoratorSettings)["type"];
		decorators::FiducialDecorator * (*tDecoratorFactory)(libconfig::Setting &, DecoratorManager &) = (*mDecoratorFactories)[tDecoratorType];
    // Then we call the factory
		if (tDecoratorFactory) tDecorator = tDecoratorFactory(*tDecoratorSettings, *this);
		else {
			std::cerr << "No implementation available to load: " << tDecoratorType << std::endl;
			return 0;
		}
    // And we store the newly created object into the dictionnary
		mDecorators[mCurrentConfigFile+tDecoratorName] = tDecorator;
		if (!tDecorator) {
			std::cerr << "Error loading: " << tDecoratorSettings->getName() << std::endl;
		}
    // We also keep track of the order in which decorators have been defined in
    // the config file; sometimes we need to control who gets updated first.
		mOrderedDecorators.push_back(tDecorator);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Decorator type not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Decorator type not a string: " << e.getPath() << std::endl;
	}
	return tDecorator;
}
