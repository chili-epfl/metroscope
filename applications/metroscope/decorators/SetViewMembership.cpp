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

#include "SetViewMembership.hpp"
#include <iostream>

const std::string decorators::SetViewMembership::scDecoratorName("SetViewMembership");
const  DecoratorManager::Registerer decorators::SetViewMembership::mRegisterer(decorators::SetViewMembership::scDecoratorName, &decorators::SetViewMembership::create);

decorators::FiducialDecorator *decorators::SetViewMembership::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		decorators::SetViewMembership *tSetViewMembership = new decorators::SetViewMembership(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), pSetting["set_radius"]);
		libconfig::Setting & tClassMarkers = pSetting["class_markers"];
		for (int i=0; i<tClassMarkers.getLength(); ++i)
		{
			tSetViewMembership->addClass(pDecoratorManager.loadMarker(tClassMarkers[i]));
		}
		libconfig::Setting & tComplementMarkers = pSetting["complement_markers"];
		for (int i=0; i<tComplementMarkers.getLength(); ++i)
		{
			tSetViewMembership->addComplement(pDecoratorManager.loadMarker(tComplementMarkers[i]));
		}
		return tSetViewMembership;
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::SetViewMembership::SetViewMembership(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, float pSetRadius):
FiducialDecorator(pDecoratorManager, pMarker),
mClassList(),
mComplementList(),
mSetRadius(pSetRadius)
{
}

void decorators::SetViewMembership::update() {
	if (mMarker->isPresent())
	{
		bool tInClass = true;
		bool tInComplement = false;

		wykobi::point2d<float> tMarkerOrigin = mMarker->getCenter();
		mDecoratorManager.GetCam2World().InterpolatedMap(tMarkerOrigin);

		for (std::list<FiducialMarker *>::iterator mClassIt = mClassList.begin(); mClassIt != mClassList.end(); mClassIt++) {
			wykobi::point2d<float> tClassMarkerOrigin = (*mClassIt)->getCenter();
			mDecoratorManager.GetCam2World().InterpolatedMap(tClassMarkerOrigin);
			if (wykobi::distance(tMarkerOrigin, tClassMarkerOrigin) > mSetRadius) {
				tInClass = false;
			}
		}
		for (std::list<FiducialMarker*>::iterator tComplementIt = mComplementList.begin(); tComplementIt != mComplementList.end(); tComplementIt++) {
			wykobi::point2d<float> tComplementMarkerOrigin = (*tComplementIt)->getCenter();
			mDecoratorManager.GetCam2World().InterpolatedMap(tComplementMarkerOrigin);
			if (wykobi::distance(tMarkerOrigin, tComplementMarkerOrigin) <= mSetRadius) {
				tInComplement = true;
			}
		}

		static const float scDrawingSize = 20.0f;
		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tMarkerOrigin);
		if (tInClass && !tInComplement) {
			mDecoratorManager.GetDisplay().RenderEllipse(tMarkerOrigin.x, tMarkerOrigin.y, scDrawingSize, scDrawingSize, 0.0, 1.0, 0.0);
		} else if (tInComplement) {
			mDecoratorManager.GetDisplay().RenderLine(tMarkerOrigin.x-scDrawingSize, tMarkerOrigin.y+scDrawingSize, tMarkerOrigin.x+scDrawingSize, tMarkerOrigin.y-scDrawingSize, 1.0f, 0.0f, 0.0f);
			mDecoratorManager.GetDisplay().RenderLine(tMarkerOrigin.x+scDrawingSize, tMarkerOrigin.y+scDrawingSize, tMarkerOrigin.x-scDrawingSize, tMarkerOrigin.y-scDrawingSize, 1.0f, 0.0f, 0.0f);
		} else {
			mDecoratorManager.GetDisplay().RenderLine(tMarkerOrigin.x-scDrawingSize, tMarkerOrigin.y+scDrawingSize, tMarkerOrigin.x+scDrawingSize, tMarkerOrigin.y+scDrawingSize, 1.0f, 0.85f, 0.0f);
			mDecoratorManager.GetDisplay().RenderLine(tMarkerOrigin.x-scDrawingSize, tMarkerOrigin.y+scDrawingSize, tMarkerOrigin.x, tMarkerOrigin.y-scDrawingSize, 1.0f, 0.85f, 0.0f);
			mDecoratorManager.GetDisplay().RenderLine(tMarkerOrigin.x+scDrawingSize, tMarkerOrigin.y+scDrawingSize, tMarkerOrigin.x, tMarkerOrigin.y-scDrawingSize, 1.0f, 0.85f, 0.0f);
		}
	}
}
