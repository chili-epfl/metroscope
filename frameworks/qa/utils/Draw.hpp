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

#ifndef Draw_HPP
#define Draw_HPP

#include <wykobi/wykobi.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>
#include "Camera3DProjections.hpp"

namespace Draw {
    template <class T> inline void draw(const wykobi::quadix<T,2> & pQuadix, IplImage * pImage, CvScalar pColour, int pThickness) {
		if ((pQuadix[0].x > -pImage->width && pQuadix[0].x < 2*pImage->width) && (pQuadix[0].y > -pImage->height && pQuadix[0].y < 2*pImage->height) &&
			(pQuadix[1].x > -pImage->width && pQuadix[1].x < 2*pImage->width) && (pQuadix[1].y > -pImage->height && pQuadix[1].y < 2*pImage->height))
			cvLine(pImage,cvPoint((int)pQuadix[0].x, (int)pQuadix[0].y),cvPoint((int)pQuadix[1].x, (int)pQuadix[1].y),pColour,pThickness);
		if ((pQuadix[2].x > -pImage->width && pQuadix[2].x < 2*pImage->width) && (pQuadix[2].y > -pImage->height && pQuadix[2].y < 2*pImage->height) &&
			(pQuadix[1].x > -pImage->width && pQuadix[1].x < 2*pImage->width) && (pQuadix[1].y > -pImage->height && pQuadix[1].y < 2*pImage->height))
			cvLine(pImage,cvPoint((int)pQuadix[1].x, (int)pQuadix[1].y),cvPoint((int)pQuadix[2].x, (int)pQuadix[2].y),pColour,pThickness);
		if ((pQuadix[2].x > -pImage->width && pQuadix[2].x < 2*pImage->width) && (pQuadix[2].y > -pImage->height && pQuadix[2].y < 2*pImage->height) &&
			(pQuadix[3].x > -pImage->width && pQuadix[3].x < 2*pImage->width) && (pQuadix[3].y > -pImage->height && pQuadix[3].y < 2*pImage->height))
			cvLine(pImage,cvPoint((int)pQuadix[2].x, (int)pQuadix[2].y),cvPoint((int)pQuadix[3].x, (int)pQuadix[3].y),pColour,pThickness);
		if ((pQuadix[0].x > -pImage->width && pQuadix[0].x < 2*pImage->width) && (pQuadix[0].y > -pImage->height && pQuadix[0].y < 2*pImage->height) &&
			(pQuadix[3].x > -pImage->width && pQuadix[3].x < 2*pImage->width) && (pQuadix[3].y > -pImage->height && pQuadix[3].y < 2*pImage->height))
			cvLine(pImage,cvPoint((int)pQuadix[3].x, (int)pQuadix[3].y),cvPoint((int)pQuadix[0].x, (int)pQuadix[0].y),pColour,pThickness);
	}

    template <class T> inline void draw(const wykobi::rectangle<T> & pRectangle, IplImage * pImage, CvScalar pColour, int pThickness) {
		int tLUx = pRectangle[0].x;
		int tLUy = pRectangle[0].y;
		int tRUx = pRectangle[1].x;
		int tRUy = pRectangle[0].y;
		int tLDx = pRectangle[0].x;
		int tLDy=  pRectangle[1].y;
		int tRDx = pRectangle[1].x;
		int tRDy = pRectangle[1].y;

		if ((tLUx > 0 && tLUx < pImage->width) && (tLUy > 0 && tLUy < pImage->height) &&
			(tRUx > 0 && tRUx < pImage->width) && (tRUy > 0 && tRUy < pImage->height))
			cvLine(pImage,cvPoint((int)tLUx, (int)tLUy),cvPoint((int)tRUx, (int)tRUy),pColour,pThickness);
		if ((tLDx > 0 && tLDx < pImage->width) && (tLDy > 0 && tLDy < pImage->height) &&
			(tRUx > 0 && tRUx < pImage->width) && (tRUy > 0 && tRUy < pImage->height))
			cvLine(pImage,cvPoint((int)tRUx, (int)tRUy),cvPoint((int)tRDx, (int)tRDy),pColour,pThickness);
		if ((tLDx > 0 && tLDx < pImage->width) && (tLDy > 0 && tLDy < pImage->height) &&
			(tRDx > 0 && tRDx < pImage->width) && (tRDy > 0 && tRDy < pImage->height))
			cvLine(pImage,cvPoint((int)tLDx, (int)tLDy),cvPoint((int)tRDx, (int)tRDy),pColour,pThickness);
		if ((tLUx > 0 && tLUx < pImage->width) && (tLUy > 0 && tLUy < pImage->height) &&
			(tRDx > 0 && tRDx < pImage->width) && (tRDy > 0 && tRDy < pImage->height))
			cvLine(pImage,cvPoint((int)tLDx, (int)tLDy),cvPoint((int)tLUx, (int)tLUy),pColour,pThickness);
	}

    inline void draw(const std::string & pString, IplImage * pImage, const CvPoint & pPosition, CvFont pFont, CvScalar pColour) {
		cvPutText (pImage,pString.c_str(),pPosition, &pFont, pColour);
	}

	inline void shiftColour(IplImage * pImage, const CvPoint & pLUCorner, const CvPoint & pRDCorner, const CvScalar & pColour, float pWeight = 1.0f){
		char tBPixelValue;
		char tGPixelValue;
		char tRPixelValue;
		if(pImage->nChannels==3){
			for (int tX = std::max(pLUCorner.x,0); tX<std::min(pRDCorner.x,pImage->width); tX++){
				for (int tY = std::max(pLUCorner.y,0); tY<std::min(pRDCorner.y,pImage->height); tY++){
					tBPixelValue =(char)(pWeight*(float)pColour.val[0]+(float)((uchar *)(pImage->imageData + tY*pImage->widthStep))[tX*pImage->nChannels + 0]);
					tGPixelValue =(char)(pWeight*(float)pColour.val[1]+(float)((uchar *)(pImage->imageData + tY*pImage->widthStep))[tX*pImage->nChannels + 1]);
					tRPixelValue =(char)(pWeight*(float)pColour.val[2]+(float)((uchar *)(pImage->imageData + tY*pImage->widthStep))[tX*pImage->nChannels + 2]);

					((uchar *)(pImage->imageData + tY*pImage->widthStep))[tX*pImage->nChannels + 0] = (uchar)tBPixelValue;
					((uchar *)(pImage->imageData + tY*pImage->widthStep))[tX*pImage->nChannels + 1] = (uchar)tGPixelValue;
					((uchar *)(pImage->imageData + tY*pImage->widthStep))[tX*pImage->nChannels + 2] = (uchar)tRPixelValue;
				}
			}
		}
		if(pImage->nChannels==1){
			for (int tX = std::max(pLUCorner.x,0); tX<std::min(pRDCorner.x,pImage->width); tX++){
				for (int tY = std::max(pLUCorner.y,0); tY<std::min(pRDCorner.y,pImage->height); tY++){
					tBPixelValue =(char)(pWeight*(float)pColour.val[0]+(float)((uchar *)(pImage->imageData + tY*pImage->widthStep))[tX*pImage->nChannels + 0]);

					((uchar *)(pImage->imageData + tY*pImage->widthStep))[tX*pImage->nChannels + 0] = (uchar)tBPixelValue;

				}

			}

		}

	}

    inline void drawCartesianAxes(IplImage * pImage, const wykobi::point3d<float> & pOrigin, const Camera * pCamera) {
    	wykobi::point2d<float> tOrigin2d = Camera3DProjections::project(pOrigin,pCamera);
    	wykobi::point2d<float> tXVertex2d = Camera3DProjections::project<float>(wykobi::make_point<float>(10.0f+pOrigin.x,pOrigin.y,pOrigin.z),pCamera);
    	wykobi::point2d<float> tYVertex2d = Camera3DProjections::project<float>(wykobi::make_point<float>(pOrigin.x,10.0f+pOrigin.y,pOrigin.z),pCamera);
    	wykobi::point2d<float> tZVertex2d = Camera3DProjections::project<float>(wykobi::make_point<float>(pOrigin.x,pOrigin.y,10.0f+pOrigin.z),pCamera);


		cvLine(pImage,cvPoint((int)tOrigin2d.x, (int)tOrigin2d.y),cvPoint((int)tXVertex2d.x, (int)tXVertex2d.y),cvScalar(255,255,0),2);
		cvLine(pImage,cvPoint((int)tOrigin2d.x, (int)tOrigin2d.y),cvPoint((int)tYVertex2d.x, (int)tYVertex2d.y),cvScalar(0,255,0),2);

		if (pOrigin.z<wykobi::make_point<float>(pOrigin.x,pOrigin.y,10.0f+pOrigin.z).z)
			cvLine(pImage,cvPoint((int)tOrigin2d.x, (int)tOrigin2d.y),cvPoint((int)tZVertex2d.x, (int)tZVertex2d.y),cvScalar(0,0,255),2);
		else
			cvLine(pImage,cvPoint((int)tOrigin2d.x, (int)tOrigin2d.y),cvPoint((int)tZVertex2d.x, (int)tZVertex2d.y),cvScalar(120,120,120),2);

	}
}

#endif
