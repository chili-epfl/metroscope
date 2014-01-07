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

#ifndef Chessboard_HPP
#define Chessboard_HPP

#include <qa/pipeables/Pipeable.hpp>
#include <opencv2/opencv.hpp>
#include <string>

class Chessboard: public Pipeable
{
    public:

        Chessboard(
            	int pPositionX,
            	int pPositionY,
            	int pNSquareCols,
            	int pNSquareRows,
            	int pSquareWidth,
            	int pSquareHeight,
            	int pR1 = 0, int pG1 = 0, int pB1 = 0,
            	int pR2 = 255, int pG2 = 255, int pB2 = 255);

    protected:
        void run();

    	const int cPositionX;
    	const int cPositionY;
    	const int cNSquareCols;
    	const int cNSquareRows;
    	const int cSquareWidth;
    	const int cSquareHeight;
    	const int cR1;
    	const int cG1;
    	const int cB1;
    	const int cR2;
    	const int cG2;
    	const int cB2;

    private:
        Chessboard();
        Chessboard(const Chessboard& pChessboard);
        Chessboard& operator=(const Chessboard& pChessboard);
};

#endif
