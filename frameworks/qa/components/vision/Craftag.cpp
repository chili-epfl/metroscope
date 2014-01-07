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

#include "Craftag.hpp"
#include <stdio.h>
#include "../../components/vision/Color.hpp"

Craftag::Craftag(int pMarkerId, int pNFramesToPersist, CraftagRegistrar &pCraftagRegistrar):
	mMarkerId(pMarkerId),
	mNFramesToPersist(pNFramesToPersist),
	mCraftagRegistrar(pCraftagRegistrar)
{
	mCraftagRegistrar.registerCraftag(mMarkerId);
}


bool Craftag::isPresent() const
{
	int tLastDetectedFrame = mCraftagRegistrar.getLastDetectedFrame(mMarkerId);
	return tLastDetectedFrame >=0 && (mCraftagRegistrar.getFrameId() - tLastDetectedFrame) <= mNFramesToPersist;
}

wykobi::quadix<float, 2> Craftag::getCorners() const
{
	const CvPoint2D32f *tCorners = mCraftagRegistrar.getCorners(mMarkerId);
	wykobi::quadix<float, 2> tResult;
	tResult[0].x = tCorners[0].x;
	tResult[0].y = tCorners[0].y;
	tResult[1].x = tCorners[1].x;
	tResult[1].y = tCorners[1].y;
	tResult[2].x = tCorners[2].x;
	tResult[2].y = tCorners[2].y;
	tResult[3].x = tCorners[3].x;
	tResult[3].y = tCorners[3].y;
	return tResult;
}

void Craftag::draw(IplImage* pImage, const wykobi::point2d<int>&, const Angle<float>&, float, const Color& pColor) const
{
	if (isPresent())
	{
		CvFont tFont;
		wykobi::point2d<float> tCenter = getCenter();
		CvPoint tCvCenter = cvPoint((int)tCenter.x, (int)tCenter.y);
		wykobi::point2d<float> tTop = tCenter+getYUnit();
		wykobi::point2d<float> tRight = tCenter+getXUnit();
		CvScalar tCvColor = CV_RGB(pColor.getRed(), pColor.getGreen(), pColor.getBlue());
		cvLine(pImage, tCvCenter, cvPoint((int)tTop.x, (int)tTop.y), tCvColor);
		cvLine(pImage, tCvCenter, cvPoint((int)tRight.x, (int)tRight.y), tCvColor);
		cvInitFont(&tFont,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, 1.0, 1.0);
		char tText[1+4+1];
#ifdef __STDC_SECURE_LIB__
		sprintf_s(tText, "c%d", mMarkerId);
#else
		sprintf(tText, "c%d", mMarkerId);
#endif
		cvPutText(pImage, tText, tCvCenter, &tFont, tCvColor);
	}
}

int Craftag::GetFramesNumberSinceLastDetected() {
		return (mCraftagRegistrar.getFrameId() - mCraftagRegistrar.getLastDetectedFrame(mMarkerId));
}
