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

#ifndef Color_HPP
#define Color_HPP

#include <iostream>

class Color
{

	public:

		Color();
		Color (const int & pRed, const int & pGreen, const int & pBlue);

		void set (const int & pRed, const int & pGreen, const int & pBlue);

		int getRed() const;
		int getGreen() const;
		int getBlue() const;

		bool operator ==(const Color & pColor) const;
		bool operator !=(const Color & pColor) const;

		float getDistance(const Color & pPoint) const;

		static const Color *cBlack;
		static const Color *cWhite;
		static const Color *cRed;
		static const Color *cGreen;
		static const Color *cBlue;
		static const Color *cYellow;
		static const Color *cCyan;
		static const Color *cMagenta;
		static const Color *cPink;
		static const Color *cPurple;

		friend std::ostream& operator<<(std::ostream & os, const Color & pColor)
		{
			os << "a=" << pColor.getRed() << ", b=" << pColor.getGreen() << ", c=" << pColor.getBlue();
			return os;
		}

	protected:
		int mRed,
			mGreen,
			mBlue;

};

#endif
