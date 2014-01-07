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

#include "Chessboard.hpp"
#include <GL/glut.h>

Chessboard::Chessboard(
    	int pPositionX,
    	int pPositionY,
    	int pNSquareRows,
    	int pNSquareCols,
    	int pSquareWidth,
    	int pSquareHeight,
    	int pR1, int pG1, int pB1,
    	int pR2, int pG2, int pB2):
cPositionX(pPositionX),
cPositionY(pPositionY),
cNSquareCols(pNSquareCols),
cNSquareRows(pNSquareRows),
cSquareWidth(pSquareWidth),
cSquareHeight(pSquareHeight),
cR1(pR1), cG1(pG1), cB1(pB1),
cR2(pR2), cG2(pG2), cB2(pB2)
{
}

void Chessboard::run()
{
	glBegin(GL_QUADS);

	glColor3ub(cR2, cG2, cB2);
	glVertex2i(cPositionX                            , cPositionY + cNSquareRows*cSquareHeight);
	glVertex2i(cPositionX                            , cPositionY                             );
	glVertex2i(cPositionX + cNSquareCols*cSquareWidth, cPositionY                             );
	glVertex2i(cPositionX + cNSquareCols*cSquareWidth, cPositionY + cNSquareRows*cSquareHeight);

	glColor3ub(cR1, cG1, cB1);
	for (int x = 0, tAlternate = 0; x < cNSquareCols; ++x, tAlternate = 1-tAlternate) {
		for (int y = tAlternate; y < cNSquareRows; y+=2) {
			glVertex2i(cPositionX +   x   * cSquareWidth, cPositionY +   y   * cSquareHeight);
			glVertex2i(cPositionX +   x   * cSquareWidth, cPositionY + (y+1) * cSquareHeight);
			glVertex2i(cPositionX + (x+1) * cSquareWidth, cPositionY + (y+1) * cSquareHeight);
			glVertex2i(cPositionX + (x+1) * cSquareWidth, cPositionY +   y   * cSquareHeight);
		}
	}

	glEnd();
}
