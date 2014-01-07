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

#ifndef CraftagRefine_HPP
#define CraftagRefine_HPP

#include <qa/pipeables/Pipeable.hpp>
#include <opencv2/opencv.hpp>
#include "../../components/vision/CraftagRegistrar.hpp"

class CraftagRefine: public Pipeable
{
    public:

        CraftagRefine(
            const IplImage *const *pInputImage,
            const int *pDecodedTag,
        	CraftagRegistrar &pCraftagRegistrar);
		virtual ~CraftagRefine();

		const CvPoint2D32f*const *GetRefinedCorners() const {return &mOrderedCorners;}

    protected:
        void run();

        const IplImage *const *mInputImage;
        const int *mDecodedTag;

        CvRect mBoundingBox;
        CvRect mROI;
        CvPoint *mInternalCorners;
        IplImage *mROICopy;
        IplImage *mMask;
    	IplImage *mTempImg;
    	IplImage *mEigenImg;

    	int mNCorners;
    	CvPoint2D32f* mRefinedCorners;
    	CvPoint2D32f* mOrderedCorners;

    	CraftagRegistrar &mCraftagRegistrar;

    private:
        CraftagRefine();
        CraftagRefine(const CraftagRefine& pCraftagRefine);
        CraftagRefine& operator=(const CraftagRefine& pCraftagRefine);
};

#endif
