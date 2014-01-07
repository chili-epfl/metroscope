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

#ifndef CraftagRegistrar_HPP
#define CraftagRegistrar_HPP

#include "TagTranscoder.hpp"
#include <opencv2/opencv.hpp>

class CraftagRegistrar
{
public:
	static CraftagRegistrar &GetDefault()
	{
		if (!sDefaultCraftagRegistrar) sDefaultCraftagRegistrar=new CraftagRegistrar;
		return *sDefaultCraftagRegistrar;
	}

	CraftagRegistrar();
	virtual ~CraftagRegistrar();
	TagTranscoder & GetTranscoder() {return mTagTranscoder;}

	void registerCraftag(int pId);

	void reset();
	void set(int pId, const CvPoint2D32f *pCorners);

	const CvPoint2D32f *getCorners(int pId) const {return mCorners[mTagTranscoder.getTrackingId(pId)];}
	int getLastDetectedFrame(int pId) const {return mLastDetectedFrame[mTagTranscoder.getTrackingId(pId)];}
	int getFrameId() const {return mFrameId;}

	const CvPoint2D32f *getCornersFromInternalId(int pInternalId) const {return mCorners[pInternalId];}
	int getLastDetectedFrameFromInternalId(int pInternalId) const {return mLastDetectedFrame[pInternalId];}

protected:
	static CraftagRegistrar *sDefaultCraftagRegistrar;
	TagTranscoder mTagTranscoder;
	CvPoint2D32f **mCorners;
	int mFrameId;
	int *mLastDetectedFrame;

private:
	CraftagRegistrar(const CraftagRegistrar&);
	CraftagRegistrar& operator=(const CraftagRegistrar&);
};
#endif
