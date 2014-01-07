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

#include "Color.hpp"
#include <math.h>

Color::Color():
mRed(0),
mGreen(0),
mBlue(0)
{
}

Color::Color (const int & pRed, const int & pGreen, const int & pBlue):
mRed(pRed),
mGreen(pGreen),
mBlue(pBlue)
{
}

int Color::getRed() const{
	return mRed;
}
int Color::getGreen() const{
	return mGreen;
}
int Color::getBlue() const{
	return mBlue;
}

void Color::set(const int & pRed, const int & pGreen, const int & pBlue)
{
	mRed=pRed;
	mGreen=pGreen;
	mBlue=pBlue;

	if (mRed<0) mRed=0;
	if (mRed>255) mRed=255;

	if (mGreen<0) mGreen=0;
	if (mGreen>255) mGreen=255;

	if (mBlue<0) mBlue=0;
	if (mBlue>255) mBlue=255;
}

bool Color::operator ==(const Color & pColor) const{
	return mRed==pColor.getRed() && mGreen==pColor.getGreen() && mBlue==pColor.getBlue();
}

bool Color::operator !=(const Color & pColor) const{
	return mRed!=pColor.getRed() || mGreen!=pColor.getGreen() || mBlue!=pColor.getBlue();
}

float Color::getDistance(const Color & pColor) const{
	return sqrt((float)
		(
		(mRed-pColor.getRed())*(mRed-pColor.getRed())+
		(mGreen-pColor.getGreen())*(mGreen-pColor.getGreen())+
		(mBlue-pColor.getBlue())*(mBlue-pColor.getBlue())
		)
		);
}

const Color *Color::cBlack   = new Color(0,0,0);
const Color *Color::cWhite   = new Color(255,255,255);
const Color *Color::cRed     = new Color(255,0,0);
const Color *Color::cGreen   = new Color(0,255,0);
const Color *Color::cBlue    = new Color(0,0,255);
const Color *Color::cYellow  = new Color(255,255,0);
const Color *Color::cMagenta = new Color(255,0,255);
const Color *Color::cCyan    = new Color(0,255,255);
const Color *Color::cPink    = new Color(255,100,100);
const Color *Color::cPurple  = new Color(155,0,0);
