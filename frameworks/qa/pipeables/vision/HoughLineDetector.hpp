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

#ifndef HoughLineDetector_HPP
#define HoughLineDetector_HPP

#include "../Pipeable.hpp"
#include <wykobi/wykobi.hpp>
#include <opencv2/opencv.hpp>
#include "Gray2Color.hpp"

template <bool scDraw>
class HoughLineDetector: public Pipeable
{
    public:
        static const int scHoughProbabilistic = CV_HOUGH_PROBABILISTIC;
        static const int scHoughStandard = CV_HOUGH_STANDARD;

        HoughLineDetector(
        	int pMethod,
            double pRho,
	        double pTheta,
			int pHoughThreshold,
			double pHoughParam1,
			double pHoughParam2,
			const IplImage *const *pInputImage);

		virtual ~HoughLineDetector();

		const CvSeq *const * GetLines() const {return &mLinesSequence;}
		const wykobi::segment<float,2> *const *GetSegments2D() const {return &mSegments2D;}
		const int *GetNumSegments2D() const{return &mNumSegments2D;}
		const IplImage * const *GetOutputImage() const{return &mOutputImage;}

    protected:
		int mMethod;
		double mRho;
		double mTheta;
		int mHoughThreshold;
		double mHoughParam1;
		double mHoughParam2;

		CvMemStorage * mStorage;
		CvSeq * mLinesSequence;

		wykobi::segment<float,2> *mSegments2D;
		int mNumSegments2D;
		static const int scMaxNumSegments;

		const IplImage *const *mInputImage;
		IplImage *mCopy;

		Gray2Color *mGray2Color;
		IplImage *mOutputImage;

        void run();
		void draw();
};

#endif
