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

#ifndef PlaneCoordinateMapper_HPP
#define PlaneCoordinateMapper_HPP

#include <opencv2/opencv.hpp>
#include <wykobi/wykobi.hpp>

class PlaneCoordinateMapper
{
	public:
		PlaneCoordinateMapper(const char *pMappingFile, float pScale = 1.0f);
		virtual ~PlaneCoordinateMapper();

		void InterpolatedMap(float &pX, float &pY) const;

		template <class T>
		void InterpolatedMap(T &pPoint) const
		{
			InterpolatedMap(pPoint.x, pPoint.y);
		}

		template <class T>
		void InterpolatedMapOnQuad(T &pQuad) const
		{
			InterpolatedMap(pQuad[0].x, pQuad[0].y);
			InterpolatedMap(pQuad[1].x, pQuad[1].y);
			InterpolatedMap(pQuad[2].x, pQuad[2].y);
			InterpolatedMap(pQuad[3].x, pQuad[3].y);
		}


	protected:
		IplImage *mMap;
		float mScale;
		wykobi::rectangle<float> mMapBounds;
		wykobi::point2d<float> mMapCenter;
		wykobi::point2d<float> mMappedCenter;

	private:
		PlaneCoordinateMapper();
		PlaneCoordinateMapper(const PlaneCoordinateMapper& pPlaneCoordinateMapper);
		PlaneCoordinateMapper& operator=(const PlaneCoordinateMapper& pPlaneCoordinateMapper);
};
#endif
