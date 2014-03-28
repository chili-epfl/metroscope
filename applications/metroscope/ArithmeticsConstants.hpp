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

#ifndef ArithmeticsConstants_HPP
#define ArithmeticsConstants_HPP

static const float scINSTRUCTION_X = 300.0f;
static const float scINSTRUCTION_Y = 100.0f;
static const char* const scINSTRUCTION_TEXT = "Put the cards in the boxes";
static const float scBOXWIDTH = 123.0f;

static const float scCENT1_X1 = 15.0f;
static const float scCENT1_X2 = scCENT1_X1 + scBOXWIDTH - 1.0f;
static const float scTEN1_X1 = scCENT1_X2 + 1.0f;
static const float scTEN1_X2 = scTEN1_X1 + scBOXWIDTH - 1.0f;
static const float scUNIT1_X1 = scTEN1_X2 + 1.0f;
static const float scUNIT1_X2 = scUNIT1_X1 + scBOXWIDTH;

static const float scCENT2_X1 = scCENT1_X1 + 440;
static const float scCENT2_X2 = scCENT2_X1 + scBOXWIDTH - 1.0f;
static const float scTEN2_X1 = scCENT2_X2 + 1.0f;
static const float scTEN2_X2 = scTEN2_X1 + scBOXWIDTH - 1.0f;
static const float scUNIT2_X1 = scTEN2_X2 + 1.0f;
static const float scUNIT2_X2 = scUNIT2_X1 + scBOXWIDTH ;

static const float scCENT_SOLUTION_X1 = scCENT2_X1 + 440;
static const float scCENT_SOLUTION_X2 = scCENT_SOLUTION_X1 + scBOXWIDTH - 1.0f;
static const float scTEN_SOLUTION_X1 = scCENT_SOLUTION_X2 + 1.0f;
static const float scTEN_SOLUTION_X2 = scTEN_SOLUTION_X1 + scBOXWIDTH - 1.0f;
static const float scUNIT_SOLUTION_X1 = scTEN_SOLUTION_X2 + 1.0f;
static const float scUNIT_SOLUTION_X2 = scUNIT_SOLUTION_X1 + scBOXWIDTH;

static const float scPLUS_X = (scUNIT1_X2 + scCENT2_X1)/2.0;
static const float scEQUAL_X = (scUNIT2_X2 + scCENT_SOLUTION_X1)/2.0;

static const float scY1 = 150.0f;
static const float scY2 = 350.0;

static const float scCENT1_X_AVG = (scCENT1_X1 + scCENT1_X2)/2.0;
static const float scCENT2_X_AVG = (scCENT2_X1 + scCENT2_X2)/2.0;
static const float scTEN1_X_AVG = (scTEN1_X1 + scTEN1_X2)/2.0;
static const float scTEN2_X_AVG = (scTEN2_X1 + scTEN2_X2)/2.0;
static const float scUNIT1_X_AVG = (scUNIT1_X1 + scUNIT1_X2)/2.0;
static const float scUNIT2_X_AVG = (scUNIT2_X1 + scUNIT2_X2)/2.0;

static const float scY_AVG = (scY1 + scY2)/2.0;

static const float scGREEN_R = 0.0f;
static const float scGREEN_G = 153.0f;
static const float scGREEN_B = 0.0f;
static const float scBLUE_R = 0.0f;
static const float scBLUE_G = 0.0f;
static const float scBLUE_B = 204.0f;
static const float scYELLOW_R = 255.0f;
static const float scYELLOW_G = 255.0f;
static const float scYELLOW_B = 0.0f;

#endif
