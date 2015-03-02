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

#include "PlaneCoordinateMapper.hpp"
#include <iostream>
#include <qa/utils/Geometry_printing.hpp>

PlaneCoordinateMapper::PlaneCoordinateMapper(const char *pMappingFile, float pScale):
mMap(pMappingFile?(IplImage *) cvLoad(pMappingFile):0),
mScale(pScale)
{
	if (!mMap) {
		std::cerr << "Using dummy PlaneCoordinateMapper" << std::endl;
		return;
	}


	auto tMapMin = wykobi::make_point((float) mMap->width, (float) mMap->height);
	auto tMapMax = wykobi::make_point(0.0f, 0.0f);

	//FIXME: find the contour, and take the min of that...
//	auto tMapMinData = wykobi::make_point(9999.0f, 9999.0f);
//	auto tMapMaxData = wykobi::make_point(-1.0f, -1.0f);
	auto tLineStart = (float *) mMap->imageData;
	for (int y = 0; y < mMap->height; ++y) {
		auto tImageData = tLineStart;
		for (int x = 0; x < mMap->width; ++x) {
			float tImageXData = *tImageData;

			if (x < tMapMin.x && tImageXData > 10.0f)
			{
				tMapMin.x = x;
//				tMapMinData.x = tImageXData;
			}
			if (x > tMapMax.x && tImageXData > 10.0f)
			{
				tMapMax.x = x;
//				tMapMaxData.x = tImageXData;
			}
			++tImageData;
			float tImageYData = *tImageData;
			if (y < tMapMin.y && tImageYData > 10.0f)
			{
				tMapMin.y = y;
//				tMapMinData.y = tImageYData;
			}
			if (y > tMapMax.y && tImageYData > 10.0f)
			{
				tMapMax.y = y;
//				tMapMaxData.y = tImageYData;
			}
			++tImageData;
		}
		tLineStart+=mMap->widthStep/sizeof(float);
	}

	//FIXME: find the contour, and take the min of that...
	//then we don't need margins
	auto tMargin = wykobi::make_point(300.0f, 300.0f);
	mMapBounds[0] = tMapMin + tMargin;
	mMapBounds[1] = tMapMax - tMargin;
	mMapCenter = wykobi::segment_mid_point(tMapMin, tMapMax);
	mMappedCenter = mMapCenter;
	// We need to preemptively unscale the coordinates,
	// because we are dealing with the raw positions in the map.
	// Have you ever preemptively unscaled? I have.
	mMappedCenter.x /= mScale;
	mMappedCenter.y /= mScale;
	InterpolatedMap(mMappedCenter);

	//std::cout << mMapBounds[0] << "<" << mMapBounds[1] << std::endl;
	//std::cout << mMappedCenter << std::endl;
}

PlaneCoordinateMapper::~PlaneCoordinateMapper()
{
	cvReleaseImage(&mMap);
}

void PlaneCoordinateMapper::InterpolatedMap(float &pX, float &pY) const
{
	if (!mMap) return;
	auto tPoint = wykobi::make_point(mScale*pX, mScale*pY);

	//std::cout << tPoint << " in? " << mMapBounds << " ";
	if (wykobi::point_in_rectangle(tPoint, mMapBounds))
	{
//		std::cout << "interpolate" << std::endl;
		float tScaledX = mScale*pX;
		float tScaledY = mScale*pY;
		int x1 = (int) tScaledX;
		int x2 = x1+1;
		float x1x = tScaledX-(float) x1;
		float xx2 = ((float) x2)-tScaledX;
		int y1 = (int) tScaledY;
		int y2 = y1+1;
		float y1y = tScaledY-(float) y1;
		float yy2 = ((float) y2)-tScaledY;

//		std::cout << x1 << "/" << mMap->width << " " << y1 << "/" << mMap->height << std::endl;
		const int cStep = mMap->widthStep/sizeof(float);
		float *f = (float *) mMap->imageData+y1*cStep+x1*2;
		float f11x = f[0];
		float f11y = f[1];
		float f21x = f[2];
		float f21y = f[3];
		f += cStep;
		float f12x = f[0];
		float f12y = f[1];
		float f22x = f[2];
		float f22y = f[3];

		pX = (f11x*xx2 + f21x*x1x)*yy2 + (f12x*xx2 + f22x*x1x)*y1y;
		pY = (f11y*xx2 + f21y*x1x)*yy2 + (f12y*xx2 + f22y*x1x)*y1y;
	}
	else
	{
		//extrapolate
		//std::cout << "extrapolate " ;
		int tIntersectingEdgeNumber = 0;
		while ( tIntersectingEdgeNumber < 4
				&& !wykobi::intersect(wykobi::make_segment(mMapCenter, tPoint), edge(mMapBounds,tIntersectingEdgeNumber)))
		{
			++tIntersectingEdgeNumber;
		}
		//static const char *scEdgeNames[5] = {"top","right","bottom", "left", "???\n"};
//		std::cout << scEdgeNames[tIntersectingEdgeNumber] << "";
		if (tIntersectingEdgeNumber >= 4) {
//			std::cerr << "unable to find a point on the border of the map between the center and the point to extrainterpolate." << std::endl;
			return;
		}
		auto tBorderPoint = wykobi::intersection_point(wykobi::make_segment(mMapCenter, tPoint), edge(mMapBounds,tIntersectingEdgeNumber));
		float tScale = wykobi::distance(mMapCenter, tPoint)/ (float) wykobi::distance(mMapCenter, tBorderPoint);
		auto tMappedBorderPoint = wykobi::make_point((float) tBorderPoint.x, (float) tBorderPoint.y);
//		std::cout << mMapCenter << tBorderPoint << tPoint << std::endl;
		// preemptive unscaling II
		tMappedBorderPoint.x /= mScale;
		tMappedBorderPoint.y /= mScale;
		InterpolatedMap(tMappedBorderPoint);
		auto tVector = tMappedBorderPoint - mMappedCenter;
		auto tExtrapolatedPoint = mMappedCenter + (tScale * tVector);
//		std::cout << mMappedCenter << tMappedBorderPoint << tExtrapolatedPoint << std::endl;
//		std::cout << std::endl;
		pX = tExtrapolatedPoint.x;
		pY = tExtrapolatedPoint.y;
	}
	//std::cout << std::endl;
}

template void PlaneCoordinateMapper::InterpolatedMap(wykobi::point2d<float> &pPoint) const;
template void PlaneCoordinateMapper::InterpolatedMap(CvPoint2D32f &pPoint) const;

template void PlaneCoordinateMapper::InterpolatedMapOnQuad(wykobi::quadix<float, 2> &pQuad) const;
template void PlaneCoordinateMapper::InterpolatedMapOnQuad(wykobi::point2d<float> *&pQuad) const;
template void PlaneCoordinateMapper::InterpolatedMapOnQuad(CvPoint2D32f *&pQuad) const;

template void PlaneCoordinateMapper::InterpolatedMapOnPolygon(wykobi::polygon<float, 2> &pPoly, int size) const;
