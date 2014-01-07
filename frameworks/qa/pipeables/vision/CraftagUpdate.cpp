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

#include "CraftagUpdate.hpp"

#include "CannyEdgeDetector.hpp"
#include "QuadixFinder.hpp"
#include "../misc/Map.hpp"
#include "CraftagUndistort.hpp"
#include "CraftagReadBits.hpp"
#include "CraftagDecode.hpp"
#include "CraftagRegister.hpp"
#include "CraftagRefine.hpp"

CraftagUpdate::CraftagUpdate(
	const IplImage *const *pInputImage,
	CraftagRegistrar &pCraftagRegistrar):
mCraftagRegistrar(pCraftagRegistrar)
{
	CannyEdgeDetector *tCannyEdgeDetector = new CannyEdgeDetector(100, 200, 3, pInputImage);
	mPipeables.push_back(tCannyEdgeDetector);
	QuadixFinder *tQuadixFinder = new QuadixFinder(tCannyEdgeDetector->GetOutputImage());
	mPipeables.push_back(tQuadixFinder);
	Map<wykobi::quadix2d> *tMap = new Map<wykobi::quadix2d>(
			tQuadixFinder->QuadCorners(),
			tQuadixFinder->NumQuads());
	mPipeables.push_back(tMap);
	*tCannyEdgeDetector | *tQuadixFinder | *tMap;

	CraftagUndistort *tCraftagUndistort = new CraftagUndistort(pInputImage, tMap->Variable());
	mPipeables.push_back(tCraftagUndistort);
	CraftagReadBits *tCraftagReadBits = new CraftagReadBits(tCraftagUndistort->GetOutputImage());
	mPipeables.push_back(tCraftagReadBits);
	CraftagDecode *tCraftagDecode = new CraftagDecode(tCraftagReadBits->GetBits(), pCraftagRegistrar.GetTranscoder());
	mPipeables.push_back(tCraftagDecode);
	CraftagRegister *tCraftagRegister = new CraftagRegister(tCraftagDecode->GetDecodedTag(), tMap->Variable(), tCraftagDecode->GetOrientation(), pCraftagRegistrar);
	mPipeables.push_back(tCraftagRegister);
	CraftagRefine *tCraftagRefine = new CraftagRefine(pInputImage, tCraftagDecode->GetDecodedTag(), pCraftagRegistrar);
	mPipeables.push_back(tCraftagRefine);
	*tCraftagUndistort | *tCraftagReadBits | *tCraftagDecode | *tCraftagRegister | *tCraftagRefine;
	tMap->Function(tCraftagUndistort);

	mPipeline = tCannyEdgeDetector;

	mEdgeImage = tCannyEdgeDetector->GetOutputImage();
}

CraftagUpdate::~CraftagUpdate()
{
	for(std::vector<Pipeable *>::iterator it = mPipeables.begin(); it != mPipeables.end(); ++it)
	{
	    delete *it;
	}
}
