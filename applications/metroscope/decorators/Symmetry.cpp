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

#include "Symmetry.hpp"
#include <iostream>
#include <GL/glut.h>

const std::string decorators::Symmetry::scDecoratorName("Symmetry");
const DecoratorManager::Registerer decorators::Symmetry::mRegisterer(decorators::Symmetry::scDecoratorName, &decorators::Symmetry::create);

decorators::FiducialDecorator *decorators::Symmetry::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tSize = pSetting["size"];
		return new decorators::Symmetry(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["recto"]), pDecoratorManager.loadMarker(pSetting["verso"]),
				pSetting["path"],
				tSize[0], tSize[1]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::Symmetry::Symmetry(DecoratorManager &pDecoratorManager, FiducialMarker *pRecto, FiducialMarker *pVerso,
		const char *pPath,
		float pImageWidth, float pImageHeight):
FiducialDecorator(pDecoratorManager, pRecto),
mVerso(pVerso),
mTextureId(mDecoratorManager.GetDisplay().LoadTexture(pPath)),
mHalfImageWidth(pImageWidth/2.0f),
mHalfImageHeight(pImageHeight/2.0f)
{
}

void decorators::Symmetry::update() {
	if (mMarker->isPresent() && mVerso->isPresent())
	{
		wykobi::segment<float, 2> tSegment = wykobi::make_segment(mMarker->getCenter(), mVerso->getCenter());
		mDecoratorManager.GetCam2World().InterpolatedMap(tSegment[0]);
		mDecoratorManager.GetCam2World().InterpolatedMap(tSegment[1]);
		tSegment = wykobi::rotate(90.0f, tSegment, 0.5f*(tSegment[0]+tSegment[1]));
		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tSegment[0]);
		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tSegment[1]);

		wykobi::point3d<float> p0 = wykobi::make_point(tSegment[0], 0.0f);
		wykobi::point3d<float> p1 = wykobi::make_point(tSegment[1], 0.0f);
		wykobi::point3d<float> p2 = wykobi::make_point(tSegment[1], -10.0f);
		wykobi::plane<float, 3> tPlane = wykobi::make_plane(p0, p1, p2);
		GLdouble tPlaneEquation[4] = { tPlane.normal[0], tPlane.normal[1], tPlane.normal[2], tPlane.constant };
//		mDecoratorManager.GetDisplay().RenderLine(tSegment[0].x, tSegment[0].y, tSegment[1].x, tSegment[1].y, 1.0f, 0.0f, 0.0f);

		glEnable(GL_CLIP_PLANE0);
		glClipPlane(GL_CLIP_PLANE0, tPlaneEquation);

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(*mMarker, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());

		mDecoratorManager.GetDisplay().RenderTexture(mTextureId,
				-mHalfImageWidth, -mHalfImageHeight,
				 mHalfImageWidth, -mHalfImageHeight,
				 mHalfImageWidth,  mHalfImageHeight,
				-mHalfImageWidth,  mHalfImageHeight);
//		mDecoratorManager.GetDisplay().RenderLine(0.0,0.0, 1.0,0.0, 0.0f, 0.0f, 0.0f);
//		mDecoratorManager.GetDisplay().RenderLine(0.0,0.0, 0.0,1.0, 0.0f, 0.0f, 0.0f, 0.2f);
//		mDecoratorManager.GetDisplay().RenderQuad(-1.0,-1.0, 1.0,-1.0, 1.0,1.0, -1.0,1.0, 0.0f, 0.0f, 0.0f);
		mDecoratorManager.GetDisplay().PopTransformation();
		glDisable(GL_CLIP_PLANE0);
	}
}
