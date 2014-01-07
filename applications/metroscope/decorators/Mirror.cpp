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

#include "Mirror.hpp"
#include <iostream>

#include <qa/pipeables/vision/PerspectiveWarper.hpp>
#include <qa/pipeables/vision/Flip.hpp>
#include <qa/pipeables/vision/Normalize.hpp>
#include <qa/pipeables/vision/Sharpen.hpp>
#include <qa/pipeables/vision/Gray2Color.hpp>
#include <qa/pipeables/io/OpenGLTextureLoader.hpp>

//#define DEBUG_Mirror
#ifdef DEBUG_Mirror
#include <opencv2/highgui/highgui.hpp>
#endif

const std::string decorators::Mirror::scDecoratorName("Mirror");
const DecoratorManager::Registerer decorators::Mirror::mRegisterer(decorators::Mirror::scDecoratorName, &decorators::Mirror::create);

decorators::FiducialDecorator *decorators::Mirror::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tSize = pSetting["size"];
		return new decorators::Mirror(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),
				tSize[0], tSize[1],
				pDecoratorManager.GetInputImage());
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::Mirror::Mirror(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker,
		float pImageWidth, float pImageHeight,
		const IplImage *const *pInputImage):
FiducialDecorator(pDecoratorManager, pMarker),
mImageWidth(pImageWidth),
mHalfImageHeight(pImageHeight/2.0f)
{
	static const float scMM2Px = 16.0f;

	mPipeables.push_back(new PerspectiveWarper(pInputImage, &mSelection, scMM2Px*pImageWidth, scMM2Px*pImageHeight));
	mPipeables.push_back(new Flip(Flip::scHorizontally, ((PerspectiveWarper *)mPipeables.back())->GetOutputImage()));
	mPipeables.push_back(new Sharpen(((Flip *)mPipeables.back())->GetOutputImage()));
	mPipeables.push_back(new Normalize(((Sharpen *)mPipeables.back())->GetOutputImage()));
	mPipeables.push_back(new Gray2Color(((Normalize *)mPipeables.back())->GetOutputImage()));
	mPipeables.push_back(new OpenGLTextureLoader(((Gray2Color *)mPipeables.back())->GetOutputImage()));
	mTextureId = ((OpenGLTextureLoader *)mPipeables.back())->Texture();

	for(std::vector<Pipeable *>::iterator it = mPipeables.begin(); (it+1) != mPipeables.end(); ++it)
	{
	    **it | **(it+1); //yeaaah
	}
#ifdef DEBUG_Mirror
	cvNamedWindow("DEBUG_Mirror");
#endif
}

decorators::Mirror::~Mirror()
{
	for(std::vector<Pipeable *>::iterator it = mPipeables.begin(); it != mPipeables.end(); ++it)
	{
	    delete *it;
	}
}

void decorators::Mirror::update() {
	if (mMarker->isPresent())
	{
		wykobi::point2d<float> tOrigin;
		wykobi::point2d<float> tXUnit;
		wykobi::point2d<float> tYUnit;
		FiducialMarker::ComputeBasisFromSquare(mMarker->getCorners(), tOrigin, tXUnit, tYUnit);
		mSelection[0] = tOrigin -mImageWidth*tXUnit -mHalfImageHeight*tYUnit;
		mSelection[1] = tOrigin                     -mHalfImageHeight*tYUnit;
		mSelection[2] = tOrigin                     +mHalfImageHeight*tYUnit;
		mSelection[3] = tOrigin -mImageWidth*tXUnit +mHalfImageHeight*tYUnit;
		mPipeables.front()->start();

#ifdef DEBUG_Mirror
		cvShowImage("DEBUG_Mirror", *((PerspectiveWarper *)mPipeables[0])->GetOutputImage());
//		cvShowImage("DEBUG_Mirror", *((Dilate *) (mPipeables[2]))->GetOutputImage());
//		cvShowImage("DEBUG_Mirror", *((Blur *) (mPipeables[2]))->GetOutputImage());
//		cvShowImage("DEBUG_Mirror", *((AdaptiveThreshold *) (mPipeables[3]))->GetOutputImage());
//		cvShowImage("DEBUG_Mirror", *((Gray2Color *) (mPipeables[mPipeables.size()-2]))->GetOutputImage());
		cvWaitKey(10);
#endif

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(*mMarker, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
		mDecoratorManager.GetDisplay().RenderTexture(*mTextureId,
				0,            mHalfImageHeight,
				mImageWidth,  mHalfImageHeight,
				mImageWidth, -mHalfImageHeight,
				0,           -mHalfImageHeight);
		mDecoratorManager.GetDisplay().PopTransformation();
	}
}
