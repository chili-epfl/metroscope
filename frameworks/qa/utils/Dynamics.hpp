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

#ifndef Dynamics_HPP
#define Dynamics_HPP

#include <wykobi/wykobi.hpp>

namespace Dynamics {
    template <class T> inline void esponentialApproach(wykobi::point3d<T> * pCurrentPoint, const wykobi::point3d<T> & pDestinationPoint, float pSmoothFactor){
		
		pCurrentPoint->x = pCurrentPoint->x + (pDestinationPoint.x-pCurrentPoint->x) * pSmoothFactor;
		pCurrentPoint->y = pCurrentPoint->y + (pDestinationPoint.y-pCurrentPoint->y) * pSmoothFactor;
		pCurrentPoint->z = pCurrentPoint->z + (pDestinationPoint.z-pCurrentPoint->z) * pSmoothFactor;
	}

	template <class T> inline void esponentialApproach(wykobi::vector3d<T> * pCurrentPoint, const wykobi::vector3d<T> & pDestinationPoint, float pSmoothFactor){
		pCurrentPoint->x = pCurrentPoint->x + (pDestinationPoint.x-pCurrentPoint->x) * pSmoothFactor;
		pCurrentPoint->y = pCurrentPoint->y + (pDestinationPoint.y-pCurrentPoint->y) * pSmoothFactor;
		pCurrentPoint->z = pCurrentPoint->z + (pDestinationPoint.z-pCurrentPoint->z) * pSmoothFactor;
	}

	template <class T> inline wykobi::point3d<float> sinusoidalApproach(
	  const wykobi::point3d<float> & pStart,
	  const wykobi::point3d<float> & pEnd, 
	  int pCurrentIteration, 
	  int pFinalIteration){

		float tSinusoidalFactor = 0.5f * (1.0f-cos((float)pCurrentIteration/(float)pFinalIteration * wykobi::PI));

		wykobi::point3d<float> tCurrentPosition;

		tCurrentPosition[0] = pStart[0] + (pEnd[0] - pStart[0]) * tSinusoidalFactor;
		tCurrentPosition[1] = pStart[1] + (pEnd[1] - pStart[1]) * tSinusoidalFactor;
		tCurrentPosition[2] = pStart[2] + (pEnd[2] - pStart[2]) * tSinusoidalFactor;

		return tCurrentPosition;
	}

	template <class T> inline wykobi::vector3d<float> sinusoidalApproach(
	  const wykobi::vector3d<float> & pStart,
	  const wykobi::vector3d<float> & pEnd, 
	  int pCurrentIteration, 
	  int pFinalIteration){

		float tSinusoidalFactor = 0.5f * (1.0f-cos((float)pCurrentIteration/(float)pFinalIteration * wykobi::PI));
		tSinusoidalFactor*=tSinusoidalFactor;
		wykobi::vector3d<float> tCurrentPosition;

		tCurrentPosition[0] = pStart[0] + (pEnd[0] - pStart[0]) * tSinusoidalFactor;
		tCurrentPosition[1] = pStart[1] + (pEnd[1] - pStart[1]) * tSinusoidalFactor;
		tCurrentPosition[2] = pStart[2] + (pEnd[2] - pStart[2]) * tSinusoidalFactor;

		return tCurrentPosition;
	}

	template <class T> inline wykobi::point3d<float> squareSinusoidalApproach(
	  const wykobi::point3d<float> & pStart,
	  const wykobi::point3d<float> & pEnd, 
	  int pCurrentIteration, 
	  int pFinalIteration){

		float tSinusoidalFactor = 0.5f * (1.0f-cos((float)pCurrentIteration/(float)pFinalIteration * wykobi::PI));
		tSinusoidalFactor *= tSinusoidalFactor;

		wykobi::point3d<float> tCurrentPosition;

		tCurrentPosition[0] = pStart[0] + (pEnd[0] - pStart[0]) * tSinusoidalFactor;
		tCurrentPosition[1] = pStart[1] + (pEnd[1] - pStart[1]) * tSinusoidalFactor;
		tCurrentPosition[2] = pStart[2] + (pEnd[2] - pStart[2]) * tSinusoidalFactor;

		return tCurrentPosition;
	}

	template <class T> inline wykobi::vector3d<float> squareSinusoidalApproach(
	  const wykobi::vector3d<float> & pStart,
	  const wykobi::vector3d<float> & pEnd, 
	  int pCurrentIteration, 
	  int pFinalIteration){

		float tSinusoidalFactor = 0.5f * (1.0f-cos((float)pCurrentIteration/(float)pFinalIteration * wykobi::PI));
		tSinusoidalFactor *= tSinusoidalFactor;

		wykobi::vector3d<float> tCurrentPosition;

		tCurrentPosition[0] = pStart[0] + (pEnd[0] - pStart[0]) * tSinusoidalFactor;
		tCurrentPosition[1] = pStart[1] + (pEnd[1] - pStart[1]) * tSinusoidalFactor;
		tCurrentPosition[2] = pStart[2] + (pEnd[2] - pStart[2]) * tSinusoidalFactor;

		return tCurrentPosition;
	}
}

#endif
