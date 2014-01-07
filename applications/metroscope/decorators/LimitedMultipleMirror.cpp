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

#include "LimitedMultipleMirror.hpp"
#include <iostream>

#include <qa/pipeables/vision/PerspectiveWarper.hpp>
#include <qa/pipeables/vision/Normalize.hpp>
#include <qa/pipeables/vision/Sharpen.hpp>
#include <qa/pipeables/vision/Gray2Color.hpp>
#include <qa/pipeables/io/OpenGLTextureLoader.hpp>

#include <qa/utils/Geometry.hpp>
#include <qa/utils/CvWykobiBridge.hpp>

//#define DEBUG_LimitedMultipleMirror
#ifdef DEBUG_LimitedMultipleMirror
#include <opencv2/highgui/highgui.hpp>
#include <qa/utils/Geometry_printing.hpp>
#endif

const std::string decorators::LimitedMultipleMirror::scDecoratorName("LimitedMultipleMirror");
const DecoratorManager::Registerer decorators::LimitedMultipleMirror::mRegisterer(decorators::LimitedMultipleMirror::scDecoratorName, &decorators::LimitedMultipleMirror::create);

namespace {
	const bool scExtractOnly = false;
}

decorators::FiducialDecorator *decorators::LimitedMultipleMirror::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		libconfig::Setting & tWorldSize = pSetting["worldSize"];
		LimitedMultipleMirror *tLimitedMultipleMirror = new decorators::LimitedMultipleMirror(pDecoratorManager,
				pDecoratorManager.loadMarker(pSetting["TLmarker"]),
				pDecoratorManager.loadMarker(pSetting["TRmarker"]),
				pDecoratorManager.loadMarker(pSetting["BRmarker"]),
				pDecoratorManager.loadMarker(pSetting["BLmarker"]),
				tWorldSize[0], tWorldSize[1],
				new Craftag(pSetting["id"],1), //FIXME, I'm MemoryLeak, son of Lazyness
				pDecoratorManager.GetInputImage());
		libconfig::Setting & tAxes = pSetting["axes"];
		int tNAxes = tAxes.getLength();
		for (int i = 0; i < tNAxes; ++i) {
			libconfig::Setting & tAxis = tAxes[i];
			tLimitedMultipleMirror->AddAxis(wykobi::make_line((float) tAxis[0], (float) tAxis[1], (float) tAxis[2], (float) tAxis[3]));
		}
		return tLimitedMultipleMirror;
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::LimitedMultipleMirror::LimitedMultipleMirror(DecoratorManager &pDecoratorManager, 
		const FiducialMarker *pTLMarker,
		const FiducialMarker *pTRMarker,
		const FiducialMarker *pBRMarker,
		const FiducialMarker *pBLMarker,
		float pWorldWidth,
		float pWorldHeight,
		const FiducialMarker *pIdMarker,
		const IplImage *const *pInputImage):
FiducialDecorator(pDecoratorManager, pTLMarker),
mTRMarker(pTRMarker),
mBRMarker(pBRMarker),
mBLMarker(pBLMarker),
mIdMarker(pIdMarker),
mMaxApparitions(333),
mApparitionsRemaining(mMaxApparitions),
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
#ifdef DEBUG_LimitedMultipleMirror
	cvNamedWindow("DEBUG_LimitedMultipleMirror");
#endif
}

void decorators::LimitedMultipleMirror::AddAxis(const wykobi::line<float, 2>& pAxis)
{
#ifdef DEBUG_LimitedMultipleMirror
	std::cout << mAreaOfInterest << std::endl;
#endif
	mAxes.push_back(pAxis);
	wykobi::polygon<float, 2> tSplit1;
	wykobi::polygon<float, 2> tSplit2;
	Geometry::split(mAreaOfInterest, pAxis, tSplit1, tSplit2);
	mAreaOfInterest = tSplit1;
#ifdef DEBUG_LimitedMultipleMirror
	std::cout << mAreaOfInterest << std::endl;
#endif

	mNormalizedAreaOfInterest = mAreaOfInterest;
	Geometry::normalize(mBoxOfInterest, mNormalizedAreaOfInterest);

	mMirroredAreasOfInterest.clear();
	//coding every possible combination of axes in binary
	for (int tCombinationCode = 1; tCombinationCode < (1 << mAxes.size()); ++tCombinationCode) {
		wykobi::polygon<float, 2> tMirrored = mAreaOfInterest;
		for (size_t i = 0, tCombinationId = 1; i < mAxes.size(); ++i, tCombinationId <<= 1) {
			if (tCombinationId & tCombinationCode) {
				tMirrored = wykobi::mirror(tMirrored, mAxes[i]);
			}
		}
		mMirroredAreasOfInterest.push_back(tMirrored);
	}
}

decorators::LimitedMultipleMirror::~LimitedMultipleMirror()
{
	for(std::vector<Pipeable *>::iterator it = mPipeables.begin(); it != mPipeables.end(); ++it)
	{
	    delete *it;
	}
	delete[] mHomography;
}

void decorators::LimitedMultipleMirror::update() {


	if (mMarker->isPresent()
			&& mTRMarker->isPresent()
			&& mBRMarker->isPresent()
			&& mBLMarker->isPresent()
			&& mIdMarker->isPresent())
	{


		static const float scX1 = 0.0f;
		static const float scX2 = 18.0f;
		static const float scY1 = -2.0f;
		static const float scY2 = -1.0f;

		float tRemainingRatio = mApparitionsRemaining/(float) mMaxApparitions;

		float tPartialX2 = scX1+(scX2-scX1)*tRemainingRatio;

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(*mMarker, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
		mDecoratorManager.GetDisplay().RenderQuadFilled(
				scX1, scY1,
				tPartialX2, scY1,
				tPartialX2, scY2,
				scX1, scY2,
				1.0f-tRemainingRatio, tRemainingRatio, 0.0f);
		mDecoratorManager.GetDisplay().PopTransformation();
		glColor4f(1.0f,1.0f,1.0f,1.0f);

		if (mApparitionsRemaining > 0) {
			--mApparitionsRemaining;

			mSelection[0] = mMarker->getCenter();
			mSelection[1] = mTRMarker->getCenter();
			mSelection[2] = mBRMarker->getCenter();
			mSelection[3] = mBLMarker->getCenter();
			mPipeables.front()->start();

	#ifdef DEBUG_LimitedMultipleMirror
			cvShowImage("DEBUG_LimitedMultipleMirror", *((Gray2Color *) (mPipeables[mPipeables.size()-2]))->GetOutputImage());
			cvWaitKey(10);
	#endif

			if (scExtractOnly) return;

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
}
