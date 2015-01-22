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

#include "Microscope.hpp"
#include <iostream>

#include <qa/pipeables/vision/PerspectiveWarper.hpp>
#include <qa/pipeables/vision/Normalize.hpp>
#include <qa/pipeables/vision/Sharpen.hpp>
#include <qa/pipeables/vision/Gray2Color.hpp>
#include <qa/pipeables/io/OpenGLTextureLoader.hpp>

#include <qa/utils/Geometry.hpp>
#include <qa/utils/CvWykobiBridge.hpp>

//#define DEBUG_Microscope
#ifdef DEBUG_Microscope
#include <opencv2/highgui/highgui.hpp>
#include <qa/utils/Geometry_printing.hpp>
#endif

const std::string decorators::Microscope::scDecoratorName("Microscope");
const DecoratorManager::Registerer decorators::Microscope::mRegisterer(decorators::Microscope::scDecoratorName, &decorators::Microscope::create);

namespace {
	const bool scExtractOnly = false;
}

decorators::FiducialDecorator *decorators::Microscope::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tWorldSize = pSetting["worldSize"];
		Microscope *tMicroscope = new decorators::Microscope(
				pDecoratorManager,
				pDecoratorManager.loadMarker(pSetting["TLmarker"]),
				pDecoratorManager.loadMarker(pSetting["TRmarker"]),
				pDecoratorManager.loadMarker(pSetting["BRmarker"]),
				pDecoratorManager.loadMarker(pSetting["BLmarker"]),
				tWorldSize[0], tWorldSize[1],
				pDecoratorManager.GetInputImage());
		libconfig::Setting & tAxes = pSetting["axes"];
		int tNAxes = tAxes.getLength();
		for (int i = 0; i < tNAxes; ++i) {
			libconfig::Setting & tAxis = tAxes[i];
			tMicroscope->AddAxis(wykobi::make_line((float) tAxis[0], (float) tAxis[1], (float) tAxis[2], (float) tAxis[3]));
		}
		return tMicroscope;
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::Microscope::Microscope(
		DecoratorManager &pDecoratorManager,
		const FiducialMarker *pTLMarker,
		const FiducialMarker *pTRMarker,
		const FiducialMarker *pBRMarker,
		const FiducialMarker *pBLMarker,
		float pWorldWidth,
		float pWorldHeight,
		const IplImage *const *pInputImage):
FiducialDecorator(pDecoratorManager, pTLMarker),
mTRMarker(pTRMarker),
mBRMarker(pBRMarker),
mBLMarker(pBLMarker),
mAreaOfInterest(),
mAxes(),
mBoxOfInterest(),
mNormalizedAreaOfInterest(),
mMirroredAreasOfInterest(),
mSelection(),
mLocalSelection(),
mHomography(new float[9]),
mPipeables(),
mTextureId(0),
mExtractedImage(0)
{
	mAreaOfInterest.push_back(wykobi::make_point(-pWorldWidth/2.0f, -pWorldHeight/2.0f));
	mAreaOfInterest.push_back(wykobi::make_point( pWorldWidth/2.0f, -pWorldHeight/2.0f));
	mAreaOfInterest.push_back(wykobi::make_point( pWorldWidth/2.0f,  pWorldHeight/2.0f));
	mAreaOfInterest.push_back(wykobi::make_point(-pWorldWidth/2.0f,  pWorldHeight/2.0f));

	mBoxOfInterest[0] = mAreaOfInterest[0];
	mBoxOfInterest[1] = mAreaOfInterest[2];

	mLocalSelection[0] = mAreaOfInterest[0];
	mLocalSelection[1] = mAreaOfInterest[1];
	mLocalSelection[2] = mAreaOfInterest[2];
	mLocalSelection[3] = mAreaOfInterest[3];

	static const float scMM2Px = 32.0f;

	mPipeables.push_back(new PerspectiveWarper(pInputImage, &mSelection, (int)(scMM2Px*pWorldWidth), (int)(scMM2Px*pWorldHeight)));
	if (scExtractOnly)
	{
		mExtractedImage = ((PerspectiveWarper *)mPipeables.back())->GetOutputImage();
	}
		else
	{
		mPipeables.push_back(new Sharpen(((PerspectiveWarper *)mPipeables.back())->GetOutputImage()));
		mPipeables.push_back(new Normalize(((Sharpen *)mPipeables.back())->GetOutputImage()));
		mPipeables.push_back(new Gray2Color(((Normalize *)mPipeables.back())->GetOutputImage()));
		mPipeables.push_back(new OpenGLTextureLoader(((Gray2Color *)mPipeables.back())->GetOutputImage()));
		mTextureId = ((OpenGLTextureLoader *)mPipeables.back())->Texture();

		for(std::vector<Pipeable *>::iterator it = mPipeables.begin(); (it+1) != mPipeables.end(); ++it)
		{
			**it | **(it+1); //yeaaah
		}
	}
#ifdef DEBUG_Microscope
	cvNamedWindow("DEBUG_Microscope");
#endif
}

void decorators::Microscope::AddAxis(const wykobi::line<float, 2>& pAxis)
{
#ifdef DEBUG_Microscope
	std::cout << mAreaOfInterest << std::endl;
#endif
	mAxes.push_back(pAxis);
	wykobi::polygon<float, 2> tSplit1;
	wykobi::polygon<float, 2> tSplit2;
	Geometry::split(mAreaOfInterest, pAxis, tSplit1, tSplit2);
	mAreaOfInterest = tSplit1;

	//Let's try making the AOI smaller
	mAreaOfInterest[0].x = mAreaOfInterest[0].x / 4.0f;
	mAreaOfInterest[0].y = mAreaOfInterest[0].y / 4.0f;
	mAreaOfInterest[1].x = mAreaOfInterest[1].x / 4.0f;
	mAreaOfInterest[1].y = mAreaOfInterest[1].y / 4.0f;
	mAreaOfInterest[2].x = mAreaOfInterest[2].x / 4.0f;
	mAreaOfInterest[2].y = mAreaOfInterest[2].y / 4.0f;
	mAreaOfInterest[3].x = mAreaOfInterest[3].x / 4.0f;
	mAreaOfInterest[3].y = mAreaOfInterest[3].y / 4.0f;


#ifdef DEBUG_Microscope
	std::cout << mAreaOfInterest << std::endl;
#endif

	mNormalizedAreaOfInterest = mAreaOfInterest;
	Geometry::normalize(mBoxOfInterest, mNormalizedAreaOfInterest);

	mMirroredAreasOfInterest.clear();
	//coding every possible combination of axes in binary
	for (int tCombinationCode = 1; tCombinationCode < (1 << mAxes.size()); ++tCombinationCode) {
		wykobi::polygon<float, 2> tMirrored = mAreaOfInterest;
//		for (size_t i = 0, tCombinationId = 1; i < mAxes.size(); ++i, tCombinationId <<= 1) {
//			if (tCombinationId & tCombinationCode) {
//				tMirrored = wykobi::mirror(tMirrored, mAxes[i]);
//			}
//		}
		tMirrored = wykobi::translate(mAreaOfInterest[2][1]*2.5f,0.0f,tMirrored);
		tMirrored = wykobi::scale(6.0f,6.0f,tMirrored);
		mMirroredAreasOfInterest.push_back(tMirrored);
	}
}

decorators::Microscope::~Microscope()
{
	for(std::vector<Pipeable *>::iterator it = mPipeables.begin(); it != mPipeables.end(); ++it)
	{
	    delete *it;
	}
	delete[] mHomography;
}

void decorators::Microscope::update() {
	if (mMarker->isPresent()
			&& mTRMarker->isPresent()
			&& mBRMarker->isPresent()
			&& mBLMarker->isPresent())
	{
		mSelection[0] = mMarker->getCenter();
		mSelection[1] = mTRMarker->getCenter();
		mSelection[2] = mBRMarker->getCenter();
		mSelection[3] = mBLMarker->getCenter();
		mPipeables.front()->start();

#ifdef DEBUG_Microscope
		cvShowImage("DEBUG_Microscope", *((Gray2Color *) (mPipeables[mPipeables.size()-2]))->GetOutputImage());
		cvWaitKey(10);
#endif

		if (scExtractOnly) return;

		mDecoratorManager.GetDisplay().RenderQuadFilled(0.0f,0.0f,
				10.0f,0.0f,
				10.0f,10.0f,
				0.0f,10.0f,
				1.0f,1.0f,1.0f,0.5f);

		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(mSelection);
		mDecoratorManager.GetWorld2Proj().InterpolatedMapOnQuad(mSelection);
		CvWykobiBridge::findHomography(mLocalSelection, mSelection, mHomography);

		for (size_t i = 0; i < mMirroredAreasOfInterest.size(); ++i)
		{
			wykobi::polygon<float, 2> tTransformedMirroredAreasOfInterest = mMirroredAreasOfInterest[i];
			CvWykobiBridge::applyHomography(mHomography, tTransformedMirroredAreasOfInterest);
			mDecoratorManager.GetDisplay().RenderTexture(*mTextureId, mNormalizedAreaOfInterest, tTransformedMirroredAreasOfInterest);

		}
	}
}
