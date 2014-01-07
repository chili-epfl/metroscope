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

#ifndef NaiveCalib_HPP
#define NaiveCalib_HPP

#include <qa/pipeables/Pipeable.hpp>
#include <qa/components/vision/PlaneCoordinateMapper.hpp>
#include <opencv2/opencv.hpp>
#include <string>

class PlaneCalibration: public Pipeable
{
    public:

        PlaneCalibration(
        	int pSquareCols,
        	int pSquareRows,
        	int pMaxSampleCount,
            const IplImage *const *pInputImage);
		virtual ~PlaneCalibration();

        void Write(const char *pFileName, float pSquareSize, float pScale = 1.0f, PlaneCoordinateMapper *pCamPx2WorldMm = 0) const;

    protected:
        void run();

        bool foundChessboardIsGood() const;
        void addFoundChessboard();
        void getDestinationCorners(int x, int y, CvPoint2D32f *pDstCorners, float pScale) const;
        void extrapolateCorner(int x, int y, char pXDirection, char pYDirection, CvPoint2D32f &pResult) const;

        void removeSaltAndPepper(IplImage *pMap) const;

        const CvSize cCornerArraySize;
        const int cExpectedCornerCount;

        int mSampleCount;
        int mMaxSampleCount;
        const IplImage *const *mInputImage;

        CvPoint2D32f *mFoundCorners;
        CvPoint2D32f *mSumCorners;

    private:
        PlaneCalibration();
        PlaneCalibration(const PlaneCalibration& pNaiveCalib);
        PlaneCalibration& operator=(const PlaneCalibration& pNaiveCalib);
};

#endif
