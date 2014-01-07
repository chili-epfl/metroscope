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

#ifndef QuadixFinder_H
#define QuadixFinder_H

#include "../Pipeable.hpp"
#include <opencv2/opencv.hpp>
#include <wykobi/wykobi.hpp>

class QuadixFinder: public Pipeable
{
    public:
        QuadixFinder(const IplImage *const *pBinaryImage);
        ~QuadixFinder();

		const wykobi::quadix2d *const *QuadCorners() const {return &mQuadsCorners;}
		const size_t *NumQuads() const {return &mNumQuads;}

	protected:
        virtual void run();

        const IplImage *const *mBinaryImage;
        static const int scScaledCopiesCount = 3;
        IplImage *mScaledCopies[scScaledCopiesCount];
        CvMemStorage* mStorage;

		static const int scMaxNumQuads = 1000;
		wykobi::quadix2d *const mQuadsCorners;
		size_t mNumQuads;

	private:
		QuadixFinder(const QuadixFinder&):mQuadsCorners(0){}
		QuadixFinder& operator=(const QuadixFinder&){return *this;}
};
#endif
