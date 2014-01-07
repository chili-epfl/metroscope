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

#ifndef TimeStampRenderer_HPP
#define TimeStampRenderer_HPP

class TimeStampRenderer: public Pipeable
{
    public:

	TimeStampRenderer(const long *pTime, OpenGl2DDisplay *pOpenGl2DDisplay):
    mTime(pTime),
    mOpenGl2DDisplay(pOpenGl2DDisplay)
    {}

    protected:
        void run()
        {
        	long tTimestamp = *mTime;
        	long tMillis = tTimestamp%1000l;
        	tTimestamp /= 1000l;
        	long tSeconds = tTimestamp%60l;
        	tTimestamp /= 60l;
        	long tMinutes = tTimestamp%60l;
        	tTimestamp /= 60l;
        	long tHours = 2l+tTimestamp%24l;
        	char tText[64];
        	sprintf(tText, "%.2ld:%.2ld:%.2ld.%.2ld", tHours, tMinutes, tSeconds, tMillis);
        	mOpenGl2DDisplay->RenderText(tText, 500.0f, 50.0f);
        }

        const long *mTime;
        OpenGl2DDisplay *mOpenGl2DDisplay;

    private:
        TimeStampRenderer();
        TimeStampRenderer(const TimeStampRenderer&);
        TimeStampRenderer& operator=(const TimeStampRenderer&);
};

#endif
