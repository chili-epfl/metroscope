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

#include <iostream>
#include <qa/utils/Tests.hpp>
#include <qa/utils/Image.hpp>
#include <opencv2/opencv.hpp>

int main()
{
    Tests::TestSection("Image::matchImageFormats");
    IplImage *tSrcImage = cvCreateImage(cvSize(10,11), IPL_DEPTH_8U, 3);
    IplImage *tDstImage = cvCreateImage(cvSize(20,22), IPL_DEPTH_16U, 1);
    Tests::verifyEqual(tSrcImage->width, 10);
    Tests::verifyEqual(tSrcImage->height, 11);
    Tests::verifyEqual(tSrcImage->depth, IPL_DEPTH_8U);
    Tests::verifyEqual(tSrcImage->nChannels, 3);
    Tests::verifyEqual(tDstImage->width, 20);
    Tests::verifyEqual(tDstImage->height, 22);
    Tests::verifyEqual(tDstImage->depth, IPL_DEPTH_16U);
    Tests::verifyEqual(tDstImage->nChannels, 1);
    Image::matchImageFormats(tSrcImage, &tDstImage, true);
    Tests::verifyEqual(tSrcImage->width, 10);
    Tests::verifyEqual(tSrcImage->height, 11);
    Tests::verifyEqual(tSrcImage->depth, IPL_DEPTH_8U);
    Tests::verifyEqual(tSrcImage->nChannels, 3);
    Tests::verifyEqual(tDstImage->width, 10);
    Tests::verifyEqual(tDstImage->height, 11);
    Tests::verifyEqual(tDstImage->depth, IPL_DEPTH_16U);
    Tests::verifyEqual(tDstImage->nChannels, 1);
    Image::matchImageFormats(tSrcImage, &tDstImage);
    Tests::verifyEqual(tSrcImage->width, 10);
    Tests::verifyEqual(tSrcImage->height, 11);
    Tests::verifyEqual(tSrcImage->depth, IPL_DEPTH_8U);
    Tests::verifyEqual(tSrcImage->nChannels, 3);
    Tests::verifyEqual(tDstImage->width, 10);
    Tests::verifyEqual(tDstImage->height, 11);
    Tests::verifyEqual(tDstImage->depth, IPL_DEPTH_8U);
    Tests::verifyEqual(tDstImage->nChannels, 3);
    cvReleaseImage(&tSrcImage);
    cvReleaseImage(&tDstImage);
}
