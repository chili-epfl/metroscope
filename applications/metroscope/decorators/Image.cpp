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

#include "Image.hpp"

#include <iostream>

const std::string decorators::Image::scDecoratorName("Image");
const  DecoratorManager::Registerer decorators::Image::mRegisterer(decorators::Image::scDecoratorName, &decorators::Image::create);

decorators::FiducialDecorator *decorators::Image::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tSize = pSetting["size"];
			return new decorators::Image(pDecoratorManager,
					pDecoratorManager.loadMarker(pSetting["marker"]),
					pSetting["path"],
					tSize[0], tSize[1]);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::Image::Image(DecoratorManager &pDecoratorManager, 
		FiducialMarker *pMarker,
		const char *pPath,
		float pImageWidth,
		float pImageHeight):
FiducialDecorator(pDecoratorManager, pMarker),
mTextureId(mDecoratorManager.GetDisplay().LoadTexture(pPath)),
mHalfImageWidth(pImageWidth/2.0f),
mHalfImageHeight(pImageHeight/2.0f)
{
}

void decorators::Image::update()
{
	if (mMarker->isPresent())
	{
		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(*mMarker, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
		mDecoratorManager.GetDisplay().RenderTexture(mTextureId,
				-mHalfImageWidth,  mHalfImageHeight,
				 mHalfImageWidth,  mHalfImageHeight,
				 mHalfImageWidth, -mHalfImageHeight,
				-mHalfImageWidth, -mHalfImageHeight);
		mDecoratorManager.GetDisplay().PopTransformation();
	}
}
