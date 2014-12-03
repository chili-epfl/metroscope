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

#ifndef KILLBUGCONSTANT_HPP_
#define KILLBUGCONSTANT_HPP_

#include <math.h>

static const int scFractionCards = 40;
static const int scCarteCards = 12;
static const int scHintCards = 6;
static const int scTriangleProp = 4;

//Colors
//Proportion 1: Bluesh
static const float scProp1R = 0.282f;
static const float scProp1G = 0.239f;
static const float scProp1B = 0.545f;

//Proportion 2: Green
static const float scProp2R = 0.0f;
static const float scProp2G = 1.0f;
static const float scProp2B = 0.0f;

//Proportion 3: Red
static const float scProp3R = 1.0f;
static const float scProp3G = 0.0f;
static const float scProp3B = 0.0f;

//Proportion 4: Dark Yellow
static const float scProp4R = 1.0f;
static const float scProp4G = 0.5f;
static const float scProp4B = 0.0f;

//Width for the arrows (if 0, they will not be displayed)
static const int scArrowWidth = 4;
static const int scArrowStrokeLength = 4*scArrowWidth;
//Angle for the arrowheads (in radians)
static const float scArrowAngle = M_PI/6;

struct messages{
	std::string mapFinished;
	std::string newMap;
	std::string cannotGo;
	std::string ready;
	std::string resting;
	std::string wellDone;
	std::string iveDone;
	std::string steps;
	std::string more;
	std::string toFinish;
	std::string ohNo;
	std::string remember;
	std::string veryGood;
	std::string tryAgain;
};

static const messages scMessagesEnglish = {
		std::string("Map finished!"),
		std::string("New map!"),
		std::string("I cannot go there..."),
		std::string("Ready!"),
		std::string("Resting..."),
		std::string("Well Done!"),
		std::string("I've done"),
		std::string(" steps..."),
		std::string(" more"),
		std::string("to finish"),
		std::string("Oh no!"),
		std::string("Remember:"),
		std::string("Very good!"),
		std::string("Please try again")
};

static const messages scMessagesFrench = {
		std::string("Carte finie!"),
		std::string("Carte nouvelle!"),
		std::string("Je ne peux pas y aller..."),
		std::string("Prêt!"),
		std::string("En repos ..."),
		std::string("Bravo!"),
		std::string("J'ai fait"),
		std::string(" étapes..."),
		std::string(" de plus"),
		std::string("jusq'à la fin"),
		std::string("Oh non!"),
		std::string("N'oubliez pas:"),
		std::string("Très bien!"),
		std::string("Essayez une fois encore")
};

#endif /* KILLBUGCONSTANT_HPP_ */
