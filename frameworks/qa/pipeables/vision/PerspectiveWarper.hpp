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

#ifndef PerspectiveWarper_HPP
#define PerspectiveWarper_HPP

#include <opencv2/opencv.hpp>

#include "../Pipeable.hpp"
#include <wykobi/wykobi.hpp>

class PerspectiveWarper: public Pipeable
{
	public:

		PerspectiveWarper(
	            const IplImage *const *pInputImage,
	            const wykobi::quadix<float, 2> *pSelection,
			    int pWidth, int pHeight);
        const IplImage *const *GetOutputImage() const {return &mWarpedImage;};

		~PerspectiveWarper();

	protected:
		PerspectiveWarper& operator=(const PerspectiveWarper& );
		PerspectiveWarper(const PerspectiveWarper& );

        void run();

        const IplImage *const *mInputImage;
        const wykobi::quadix<float, 2> *mSelection;
		CvPoint2D32f mSrc[4];
		CvPoint2D32f mDst[4];
        CvMat* mDistortionMatrix;
		IplImage *mWarpedImage;
};

#endif
