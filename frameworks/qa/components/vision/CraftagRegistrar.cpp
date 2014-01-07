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

#include "CraftagRegistrar.hpp"
#include <iostream>

CraftagRegistrar *CraftagRegistrar::sDefaultCraftagRegistrar = 0;

CraftagRegistrar::CraftagRegistrar():
mTagTranscoder(10, 16, 10, "1010101010", "10001000000100001"),
mCorners(new CvPoint2D32f *[mTagTranscoder.getMaxTagsNumber()]),
mFrameId(0),
mLastDetectedFrame(new int[mTagTranscoder.getMaxTagsNumber()])
{
}

CraftagRegistrar::~CraftagRegistrar()
{
	int tNTotalTagsTracked = mTagTranscoder.getNTotalTagsTracked();
	for (int i = 0; i < tNTotalTagsTracked; ++i) {
		delete [] mCorners[i];
	}
	delete [] mLastDetectedFrame;
	delete [] mCorners;
}

void CraftagRegistrar::registerCraftag(int pId)
{
	int tCountBeforeAdding = mTagTranscoder.getNTotalTagsTracked();
	int tTrackingId = mTagTranscoder.addTagToTrackingList(pId);
	
	if (tCountBeforeAdding < mTagTranscoder.getNTotalTagsTracked())
	{
		mCorners[tTrackingId] = new CvPoint2D32f[4];
		mLastDetectedFrame[tTrackingId] = -1;
	}
}

void CraftagRegistrar::reset()
{
	++mFrameId;
	mFrameId = (mFrameId<0)?0:mFrameId;
}

void CraftagRegistrar::set(int pId, const CvPoint2D32f *pCorners)
{
	int tTrackingId = mTagTranscoder.getTrackingId(pId);
	mLastDetectedFrame[tTrackingId] = mFrameId;
	for (unsigned int i=0; i<4; ++i){
		mCorners[tTrackingId][i] = pCorners[i];
	}
	//memcpy(mCorners[tTrackingId], pCorners, 4*sizeof(CvPoint2D32f));
}
