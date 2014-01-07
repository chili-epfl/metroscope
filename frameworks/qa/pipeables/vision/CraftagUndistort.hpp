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

#ifndef CraftagUndistort_HPP
#define CraftagUndistort_HPP

#include "../Pipeable.hpp"
#include <opencv2/opencv.hpp>
#include <wykobi/wykobi.hpp>

class CraftagUndistort: public Pipeable
{
    public:
        CraftagUndistort(
			const IplImage *const *pInputImage,
			const wykobi::quadix2d *pCorners);
        const IplImage *const *GetOutputImage() const {return &mUndistortedTag;}

        virtual ~CraftagUndistort();

    protected:
        void run();

        const IplImage *const *mInputImage;
        const wykobi::quadix2d *mCorners;

		CvPoint2D32f mSrcBoundaries[4];
		CvPoint2D32f mDstBoundaries[4];
		CvMat *mTransformation;

		IplImage *mUndistortedTag;

    private:
		CraftagUndistort();
		CraftagUndistort(const CraftagUndistort&);
		CraftagUndistort& operator=(const CraftagUndistort&);
};

#endif
