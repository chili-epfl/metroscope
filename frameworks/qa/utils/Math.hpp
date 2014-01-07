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

#ifndef Math_HPP
#define Math_HPP

namespace Math {
	template <class T> inline int round(T val){return (int) (val + 0.5);}

    template <class T> inline T square(T value){return value*value;}
    template <class T> inline T squaredDist(T a, T b){return square(a-b);}
    template <class T> inline T squaredDist(T x1, T y1, T x2, T y2){return square(x1-x2)+square(y1-y2);}

    template <class T> inline T linearInterpolate(float relativePosition, const T *sequence, int size)
    {
    	//TODO investigate border effect
        const int lastPos = (size-1);
        const float x = relativePosition*lastPos;
        if (x<0 || x > lastPos || lastPos < 1)
        {
            if (lastPos == 0 && 0 <= relativePosition && relativePosition <= 1)
            {
                return sequence[0];
            }
            return 0;
        }
        const int x0 = (int)x;
        if (x0 >= lastPos) return sequence[lastPos];
        return (T)(sequence[x0] + (x-x0) * ( sequence[x0+1] - sequence[x0] ));
    }

	template <class T> inline T factorial(T pVal)
	{
		T tResult = 1;
		for(T i = 2; i <= pVal; ++i) tResult *= pVal;
		return tResult;
	}

	inline int combination(int k, int n)
	{
	    if (k==0 || k==n) return 1;
	    return Math::combination(k,n-1) + Math::combination(k-1,n-1);
	}

	template <int N> struct Factorial { enum { value = N * Factorial<N - 1>::value };};
	template <> struct Factorial<0> { enum { value = 1 };};

	//aka combination
	template <int N, int K> struct Binomial { enum { value = Binomial<N-1, K-1>::value + Binomial<N-1, K>::value };};
	template <int N> struct Binomial<N, N> { enum { value = 1 };};
	template <int N> struct Binomial<N, 0> { enum { value = 1 };};
}

#endif