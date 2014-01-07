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

#ifndef Image_HPP
#define Image_HPP

#include <opencv2/opencv.hpp>

namespace Image {
	inline bool matchImageFormats(int pWidth, int pHeight, IplImage **pDstImage)
    {
        if (pWidth != (*pDstImage)->width
            || pHeight != (*pDstImage)->height)
        {
            int tDepth = (*pDstImage)->depth;
            int tChannels = (*pDstImage)->nChannels;
            cvReleaseImage(pDstImage);
            *pDstImage = cvCreateImage(cvSize(pWidth, pHeight), tDepth, tChannels);
            return true;
        }
        return false;
    }

    inline bool matchImageFormats(int pWidth, int pHeight, int pDepth, int pNChannels, IplImage **pDstImage)
    {
        if (pWidth != (*pDstImage)->width
            || pHeight != (*pDstImage)->height
            || pDepth != (*pDstImage)->depth
            || pNChannels != (*pDstImage)->nChannels)
        {
            cvReleaseImage(pDstImage);
            *pDstImage = cvCreateImage(cvSize(pWidth, pHeight), pDepth, pNChannels);
            return true;
        }
        return false;
    }

    inline bool matchImageFormats(const IplImage *pSrcImage, IplImage **pDstImage, bool pSizeOnly = false)
    {
        if (pSizeOnly)
        {
            return matchImageFormats(pSrcImage->width, pSrcImage->height, pDstImage);
        }
        return matchImageFormats(pSrcImage->width, pSrcImage->height, pSrcImage->depth, pSrcImage->nChannels, pDstImage);
    }
}

#endif
