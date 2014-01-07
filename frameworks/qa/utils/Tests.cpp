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

#include "Tests.hpp"
#include <iostream>
#include <wykobi/wykobi.hpp>
#include <qa/utils/Geometry_printing.hpp>

namespace Tests
{

	void TestSection(const std::string &pTitle)
	{
		std::cout << '\n' << pTitle << '\n' << std::endl;
	}

    template<class T> void verifyLessThan(const T &a, const T &b) {
    	std::cout << (a<b?"OK: ":"KO: ") << a << "<" << b << std::endl;
    }
	template void verifyLessThan(const int &a, const int &b);
	template void verifyLessThan(const unsigned int &a, unsigned const int &b);
	template void verifyLessThan(const long &a, const long &b);
	template void verifyLessThan(const bool &a, const bool &b);
	template void verifyLessThan(const float &a, const float &b);
	template void verifyLessThan(const double &a, const double &b);

	template<class T> void verifyEqual(const T &a, const T &b)
	{
		std::cout << (a==b?"OK: ":"KO: ") << a << "=" << b << std::endl;
	}
	template void verifyEqual(const int &a, const int &b);
	template void verifyEqual(const unsigned int &a, const unsigned int &b);
	template void verifyEqual(const size_t &a, const size_t &b);
	template void verifyEqual(const long &a, const long &b);
	template void verifyEqual(const bool &a, const bool &b);
	template void verifyEqual(const wykobi::point2d<float> &a, const wykobi::point2d<float> &b);

	template<class T> inline void verifyEqual(const T &a, const T &b, const T &eps)
	{
		T tDiff = a-b;
		std::cout << ((tDiff>-eps && tDiff<eps)?"OK: ":"KO: ") << a << "=" << b << std::endl;
	}
	template void verifyEqual(const float &a, const float &b, const float &eps);
	template void verifyEqual(const double &a, const double &b, const double &eps);

	template<> void verifyEqual(const float &a, const float &b) { verifyEqual(a,b,1.0e-10f); }
	template<> void verifyEqual(const double &a, const double &b) { verifyEqual(a,b,1.0e-10); }

	template<class Iterable> inline void verifyEqualIterables(const Iterable &a, const Iterable &b) {
		if (a.size() != b.size()) {
			std::cout << "KO: size(" << a.size() << ") != size(" << b.size() << ")" << std::endl;
			return;
		}
		bool tResult = true;
		for( wykobi::polygon<float, 2>::const_iterator ita = a.begin(), itb = b.begin();
				tResult && ita != a.end() && itb != b.end();
				++ita, ++itb) {
			tResult = (*ita == *itb);
		}
		if (a.size() < 1) {
			std::cout << (tResult?"OK":"KO") << ":[]=[]" << std::endl;
			return;
		}
		std::cout << (tResult?"OK:[":"KO:[") << a.front() << "..." << a.back() << "]=[" << b.front() << "..." << b.back() << "]" << std::endl;
	}

	template<> void verifyEqual(const wykobi::polygon<float, 2> &a, const wykobi::polygon<float, 2> &b) { verifyEqualIterables(a,b); }

	template<class T> void verifyEqual(const T *a, const T *b, size_t pSize)
	{
		bool tResult = true;
		for (size_t i = 0; tResult && i < pSize; ++i)
		{
			tResult = (a[i] == b[i]);
		}
		std::cout << (tResult?"OK:[":"KO:[") << a[0] << "..." << a[pSize-1] << "]=[" << b[0] << "..." << b[pSize-1] << "]" << std::endl;
	}
	template void verifyEqual(const int *a, const int *b, size_t pSize);
	template void verifyEqual(const unsigned int *a, const unsigned int *b, size_t pSize);
	template void verifyEqual(const long *a, const long *b, size_t pSize);
	template void verifyEqual(const bool *a, const bool *b, size_t pSize);

	template<class T> inline void verifyEqual(const T *a, const T *b, size_t pSize, T eps)
	{
		
		bool tResult = true;
		for (size_t i = 0; tResult && i < pSize; ++i)
		{
			T tDiff = a[i]-b[i];
			tResult = (tDiff>-eps && tDiff<eps);
		}
		std::cout << (tResult?"OK:[":"KO:[") << a[0] << "..." << a[pSize-1] << "]=[" << b[0] << "..." << b[pSize-1] << "]" << std::endl;
	}
	template void verifyEqual(const float *a, const float *b, size_t pSize, float eps);
	template void verifyEqual(const double *a, const double *b, size_t pSize, double eps);

	template<> void verifyEqual(const float *a, const float *b, size_t pSize) { verifyEqual(a,b,pSize,1.0e-10f); }
	template<> void verifyEqual(const double *a, const double *b, size_t pSize) { verifyEqual(a,b,pSize,1.0e-10); }
}
