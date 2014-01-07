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

#ifndef DBScanPoint_HPP
#define DBScanPoint_HPP

#include <wykobi/wykobi.hpp>

template <class T, int D>
class DBScanPoint{

	public:
		DBScanPoint();
		DBScanPoint(int pClusterId, const wykobi::pointnd<T, D> & pPoint);

		void reset();

		friend std::ostream& operator<<(std::ostream & os, const DBScanPoint<T, D> & pDBScanPoint)
		{
			os << "ID:" << pDBScanPoint.mClusterId << " ";
			os << "P(" << pDBScanPoint.mPoint[0];
			for (int i=1; i<D; i++)
			    os << "," << pDBScanPoint.mPoint[i];
			os << ")";
			return os;
		}

		wykobi::pointnd<T, D> mPoint;
		int mClusterId;
};

#endif
