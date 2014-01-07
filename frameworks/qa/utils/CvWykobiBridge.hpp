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

#ifndef CvWykobiBridge_HPP
#define CvWykobiBridge_HPP

#include <wykobi/wykobi.hpp>
#include <opencv2/opencv.hpp>

namespace CvWykobiBridge {

	template <class T>
	void inline convertQuad(const T &pFrom, CvPoint2D32f *pTo) {
		for (unsigned int i = 0; i < T::PointCount; ++i) {
			pTo[i].x = pFrom[i].x;
			pTo[i].y = pFrom[i].y;
		}
	}

	template <class T>
	inline void findHomography(
			const T &pSrc,
			const T &pDst,
			float *pHomography)
	{
		const size_t cNPoints = pSrc.size();
		float *tSrcData = new float[cNPoints*2];
		float *tDstData = new float[cNPoints*2];
		for (size_t j = 0; j < pSrc.size(); ++j)
		{
			tSrcData[j*2] = pSrc[j].x;
			tSrcData[j*2+1] = pSrc[j].y;
			tDstData[j*2] = pDst[j].x;
			tDstData[j*2+1] = pDst[j].y;
		}

		CvMat *mHomography = cvCreateMat(3, 3, CV_32FC1);
		CvMat tSrcArray = cvMat(cNPoints, 2, CV_32FC1, tSrcData);
		CvMat tDstArray = cvMat(cNPoints, 2, CV_32FC1, tDstData);
		cvFindHomography(&tSrcArray, &tDstArray, mHomography);
		for (int i = 0; i < 9; ++i) pHomography[i] = mHomography->data.fl[i];
		cvReleaseMat(&mHomography);
		delete []tDstData;
		delete []tSrcData;
	}

	template <class T> void applyHomography(
			const float pHomography[9],
			T &pEntity)
	{
		float tHa = pHomography[0];
		float tHb = pHomography[1];
		float tHc = pHomography[2];
		float tHd = pHomography[3];
		float tHe = pHomography[4];
		float tHf = pHomography[5];
		float tHg = pHomography[6];
		float tHh = pHomography[7];
		float tHi = pHomography[8];

		for (size_t i = 0; i < pEntity.size(); ++i)
		{
			float tX = pEntity[i].x;
			float tY = pEntity[i].y;
			float tZ = tHg * tX + tHh * tY + tHi;
			pEntity[i].x = (tHa * tX + tHb * tY + tHc)/tZ;
			pEntity[i].y = (tHd * tX + tHe * tY + tHf)/tZ;
		}
	}
}

#endif //CvWykobiBridge_HPP
