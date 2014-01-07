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

#include "CraftagRegister.hpp"
#include "../../utils/CvWykobiBridge.hpp"

namespace {
	const int scNCornersInQuads = 4;
}

CraftagRegister::CraftagRegister(
		const int *pDecodedTag,
		const wykobi::quadix2d *pCorners,
		const int *pRotation,
		CraftagRegistrar &pCraftagRegistrar):
mDecodedTag(pDecodedTag),
mCorners(pCorners),
mRotation(pRotation),
mCraftagRegistrar(pCraftagRegistrar),
mCornersToStore(new CvPoint2D32f[scNCornersInQuads])
{
}

CraftagRegister::~CraftagRegister()
{
	delete[] mCornersToStore;
}


void CraftagRegister::run()
{
	int tDecodedTag = *mDecodedTag;
	if (tDecodedTag>-1)
	{
		int tRotation = *mRotation;
		const wykobi::quadix2d tCorners = *mCorners;
		wykobi::quadix2d tOrientedCorners;
		for (int i = 0; i < scNCornersInQuads; ++i) {
			tOrientedCorners[i] = tCorners[(i+tRotation)%4];
		}
		CvWykobiBridge::convertQuad(tOrientedCorners, mCornersToStore);
		mCraftagRegistrar.set(tDecodedTag, mCornersToStore);
	}
}
