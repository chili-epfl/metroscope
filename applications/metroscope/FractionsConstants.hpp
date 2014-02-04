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

#ifndef FractionsConstants_HPP
#define FractionsConstants_HPP

	//MATH
	static const float scRADIANS_IN_CIRCLE = 2*M_PI;
	static const float scDEGREES_IN_CIRCLE = 360.0f;

	//COLORS
	struct color{
		float r;
		float g;
		float b;
		float a;
		color() {r=0.0f;g=0.0f;b=0.0f;a=1.0f;}
		color(float pR, float pG, float pB, float pA = 1.0f) {r = pR; g = pG; b = pB; a = pA;}
	};

	static const color scBLACK			(0.0000f, 0.0000f, 0.0000f);
	static const color scGREY			(0.5020f, 0.5020f, 0.5020f);
	static const color scLIGHT_GREY		(0.8000f, 0.8000f, 0.8000f);
	static const color scWHITE			(1.0000f, 1.0000f, 1.0000f);

	static const color scDEEP_YELLOW    (0.6510f, 0.6510f, 0.6510f);
	static const color scYELLOW    	 	(1.0000f, 1.0000f, 0.1882f);
	static const color scLIGHT_YELLOW  	(1.0000f, 1.0000f, 0.3920f);


	static const color scDEEP_ORANGE	(0.6390f, 0.3250f, 0.0590f);
	static const color scORANGE			(0.9804f, 0.5490f, 0.1765f);
	static const color scLIGHT_ORANGE	(0.9920f, 0.6630f, 0.3800f);

	static const color scDEEP_RED		(0.6270f, 0.0510f, 0.0940f);
	static const color scRED	  		(0.9647f, 0.1608f, 0.2118f);
	static const color scLIGHT_RED	  	(0.9840f, 0.3690f, 0.4120f);

	static const color scDEEP_PINK		(0.6160f, 0.0510f, 0.3800f);
	static const color scPINK			(0.9490f, 0.1529f, 0.6275f);
	static const color scLIGHT_PINK		(0.9760f, 0.3610f, 0.7220f);

	static const color scDEEP_MAGENTA	(0.5180f, 0.0470f, 0.6000f);
	static const color scMAGENTA		(0.8157f, 0.1490f, 0.9216f);
	static const color scLIGHT_MAGENTA	(0.8710f, 0.3570f, 0.9610f);

	static const color scDEEP_PURPLE	(0.2270f, 0.0430f, 0.5840f);
	static const color scPURPLE			(0.3961f, 0.1373f, 0.8980f);
	static const color scLIGHT_PURPLE	(0.5530f, 0.3450f, 0.9490f);

	static const color scDEEP_BLUE		(0.0430f, 0.1450f, 0.5690f);
	static const color scBLUE			(0.1333f, 0.2745f, 0.8745f);
	static const color scLIGHT_BLUE		(0.3410f, 0.4590f, 0.9370f);

	static const color scDEEP_SKY_BLUE	(0.0470f, 0.4040f, 0.5410f);
	static const color scSKY_BLUE		(0.1412f, 0.6431f, 0.8353f);
	static const color scLIGHT_SKY_BLUE	(0.3450f, 0.7610f, 0.9180f);

	static const color scDEEP_TURQUOISE	(0.0510f, 0.5140f, 0.3960f);
	static const color scTURQUOISE		(0.1569f, 0.7882f, 0.6275f);
	static const color scLIGHT_TURQUOISE(0.3570f, 0.8940f, 0.7610f);

	static const color scDEEP_GREEN		(0.0550f, 0.48601f, 0.1490f);
	static const color scGREEN			(0.1725f, 0.7451f, 0.2941f);
	static const color scLIGHT_GREEN	(0.3690f, 0.8750f, 0.4820f);


	static const color scTRANSPARENT_SKY_BLUE (0.1412f, 0.6431f, 0.8353f, 0.2000f);
	static const color scTRANSPARENT_TURQUOISE (0.1569f, 0.7882f, 0.6275f, 0.2000f);
	static const color scTRANSPARENT_YELLOW (1.0000f, 1.0000f, 0.1882f, 0.2000f);
	static const color scTRANSPARENT_GREEN(scGREEN.r, scGREEN.g, scGREEN.b, 0.2000f);
	static const color scTRANSPARENT_ORANGE(scORANGE.r, scORANGE.g, scORANGE.b, 0.2000f);
	static const color scTRANSPARENT_PINK(0.9490f, 0.1529f, 0.6275f, 0.2000f);
	static const color scTRASPARENT_YELLOW_TURQUOISE_BLEND1((scYELLOW.r + scTURQUOISE.r)/2, (scYELLOW.g + scTURQUOISE.g)/2, (scYELLOW.b + scTURQUOISE.b)/2, 0.2000f);

	static const color * const scERROR_MESSAGE_COLOR = &scDEEP_RED;

	static const color * const scADDITION_ACTIVITY_COLOR = &scLIGHT_PURPLE;
	static const color * const scSUBTRACTION_ACTIVITY_COLOR = &scLIGHT_BLUE;
	static const color * const scMULTIPLICATION_ACTIVITY_COLOR = &scLIGHT_PINK;
	static const color * const scDIVISION_ACTIVITY_COLOR = &scLIGHT_ORANGE;
	static const color * const scSORTING_ACTIVITY_COLOR = &scLIGHT_MAGENTA;
	static const color * const scCOMPARISON_LESS_THAN_ACTIVITY_COLOR = &scLIGHT_RED;
	static const color * const scCOMPARISON_EQUAL_TO_ACTIVITY_COLOR = &scLIGHT_TURQUOISE;

	static const color * const scNUMERATOR_DIAL_COLOR = &scDEEP_MAGENTA;
	static const color * const scDENOMINATOR_DIAL_COLOR = &scSKY_BLUE;
	static const color * const scSIMPLIFIER_DIAL_COLOR = &scGREEN;
	static const color * const scDIAL_LOWLIGHT_COLOR = &scBLACK;
	static const color * const scVARIABLE_FRACTION_COLOR = &scBLACK;

	static const color * const scCORRECT_COLOR = &scDEEP_GREEN;
	static const color * const scINCORRECT_COLOR = &scDEEP_RED;
	static const color * const scCHECKING_TEXT_COLOR = &scORANGE;
	static const color * const scCHECKING_BAR_COLOR = &scORANGE;


	static const color * const scUNIT_BLOCK_OUTLINE_COLOR = &scGREY;
	static const color * const scUNIT_BLOCK_HIGHTLIGHTED_LINE_COLOR = &scRED;
	static const color * const scUNIT_BLOCK_OPERAND_1_COLOR = &scYELLOW;
	static const color * const scUNIT_BLOCK_OPERAND_2_COLOR = &scLIGHT_TURQUOISE;
	static const color * const scUNIT_BLOCK_RESULT_COLOR = &scLIGHT_SKY_BLUE;
	static const color * const scUNIT_BLOCK_OPERAND_1_COLOR_TRANSPARENT = &scTRANSPARENT_YELLOW;
	static const color * const scUNIT_BLOCK_OPERAND_2_COLOR_TRANSPARENT = &scTRANSPARENT_TURQUOISE;
	static const color * const scUNIT_BLOCK_OPERAND_BLEND_COLOR_TRANSPARENT = new color((scYELLOW.r + scTURQUOISE.r)/2, (scYELLOW.g + scTURQUOISE.g)/2, (scYELLOW.b + scTURQUOISE.b)/2, 0.2000f);
	static const color * const scUNIT_BLOCK_OVERFLOW_COLOR = &scRED;

	static const color * const scUNIT_BLOCK_LABEL_COLOR = &scBLACK;



static const color * const scPIE_CHART_FILL_COLOR = &scBLUE;
static const color * const scDIVIDED_PIE_CHART_COLOR = &scGREEN;
static const color * const scPIE_CHART_OUTLINE_COLOR = &scBLACK;

static const color * const scFILLED_BAR_COLOR = &scTURQUOISE;
static const color * const scFILLED_BAR_OUTLINE_COLOR = &scBLACK;

static const color * const scSHADED_PIECES_FILL_COLOR = &scLIGHT_GREY;
static const color * const scSHADED_PIECES_OUTLINE_COLOR = &scBLACK;

static const color * const scCOMMON_DENOMINATORS_NUM_COLOR = scNUMERATOR_DIAL_COLOR;
static const color * const scCOMMON_DENOMINATORS_DENOM_COLOR = scDENOMINATOR_DIAL_COLOR;
static const color * const scFRACTION_BAR_COLOR = &scBLACK;

static const color * const scDECIMAL_HINT_COLOR = &scRED;


static const float scAVERAGE_NUMBER_TEXT_WIDTH = 15.17f; //in projector pixels, scale size 1.0 font.
static const float scAVERAGE_NUMBER_TEXT_HEIGHT = 17.7f;


static const float scCAMERA_WIDTH = 1125.0;
static const float scCAMERA_HEIGHT = 711.0;

//Real-world dimensions!
static const float scCARD_WIDTH = 63.0f;
static const float scCARD_HEIGHT = 88.0f;

static const float scREAL_WORLD_MARKER_WIDTH_MM = 20.0f;
static const float scREAL_WORLD_MARKER_HEIGHT_MM = 20.0f;


	//FractionModel Graphics
	enum ErrorType {Unknown, FractionWithLabels, TwoFractionsWithGraphics, Type3, Type4, Type5, Type6, Type7, Type8};

	static const char * const scTEXT_DELIMITERS = " -";

	static const float scCARD_WIDTH_IN_PROJECTOR_COORDS = 111.4f;
	static const float scCARD_HEIGHT_IN_PROJECTOR_COORDS = 154.1f;

	static const float scFRACTION_PIECE_SIZE = 10.0f;

	static const float scHINT_MESSAGE_OFFSET_X = 1.5*scCARD_WIDTH_IN_PROJECTOR_COORDS;
	static const float scHINT_MESSAGE_OFFSET_Y = -0.4*scCARD_HEIGHT_IN_PROJECTOR_COORDS;
	static const float scHINT_MESSAGE_WIDTH = 2*scCARD_WIDTH_IN_PROJECTOR_COORDS;

	static const float scHINT_MESSAGE_SCALE = 0.75f;
	static const color * const scHINT_MESSAGE_COLOR = &scGREEN;

	enum HintType{PieChart, PieChartWithPieces, FilledBar, ShadedPieces, CommonDenominators, HelpfulMessage, Decimal, UnitBlocks};
	static const HintType scHINT_MESSAGE_GRAPHICS_TYPE = PieChart;


	static const float scHINT_GRAPHICS_WIDTH = 200.0f;   // the room for displaying graphics associated with the hint message.

	static const float scHINT_GRAPHICS_ICONS_WIDTH = scCARD_WIDTH_IN_PROJECTOR_COORDS/3;
	static const float scHINT_GRAPHICS_ICONS_HEIGHT = 2*scCARD_WIDTH_IN_PROJECTOR_COORDS/3;

	static const float scHINT_GRAPHICS_OFFSET_FROM_CARD_CENTER = scCARD_HEIGHT_IN_PROJECTOR_COORDS/2 +3*scHINT_GRAPHICS_ICONS_HEIGHT/4;
	static const float scHINT_GRAPHICS_VERTICAL_OFFSET = scHINT_GRAPHICS_ICONS_HEIGHT + 20.0f;

	static const float scHINT_GRAPHICS_FRACTIONS_SCALE = 1.5f;
	static const float scHINT_GRAPHICS_FRACTIONS_LABELS_SCALE = 0.6f;

	static const int scDECIMAL_HINT_PRECISION = 3;
	static const int scDECIMAL_HINT_SCALE = 1.25;

	static const float scUNIT_BLOCK_WIDTH = 100.0f;
	static const float scUNIT_BLOCK_SEPARATION = 10.0f;

	static const float scBORDER_THICKNESS = 13.0f;

	//FractionAnswerChecker
	static const long scANSWER_CHECKING_TIME = 2L * 1000L;  //2 seconds
	static const float scANSWER_CHECKER_TEXT_SCALE = 0.75f;
	static const float scCHECKING_BAR_WIDTH = scCARD_WIDTH_IN_PROJECTOR_COORDS*(5.0/6);
	static const float scCHECKING_BAR_HEIGHT = 5.0f;
	static const float scCHECKING_BAR_VERTICAL_OFFSET = 15.0f;


	//VARIABLE FRACTION
	static const float scDIAL_RADIUS = 150.0;


	static const float scDIAL_SELECTED_NUMBER_SCALE = 1.5f;
	static const float scDIAL_NUMBERS_MAX_SCALE = 0.7*scDIAL_SELECTED_NUMBER_SCALE;

	static const float scDIAL_HIGHTLIGHTED_NUMBER_BOX_HEIGHT = scDIAL_SELECTED_NUMBER_SCALE*scAVERAGE_NUMBER_TEXT_HEIGHT*2;

	static const float scsDIAL_TRANSPARENCY_GROWTH_RATE = 2.5f;

	static const int scDIAL_MAX_NUMBERS_PER_CIRCLE = 18;
	static const float scDIAL_DEGREES_PER_NOTCH = scDEGREES_IN_CIRCLE/scDIAL_MAX_NUMBERS_PER_CIRCLE;

	static const int scMIN_NUMERATOR = 0;
	static const int scMAX_NUMERATOR = 99;
	static const int scMAX_NUMERATOR_DIGITS = 2;

	static const int scMIN_DENOMINATOR = 1;
	static const int scMAX_DENOMINATOR = 99;
	static const int scMAX_DENOMINATOR_DIGITS = 2;

	static const int scMAX_DIGITS_IN_NUMERATOR_OR_DENOMINATOR  = 3;

	static const float scVARIABLE_FRACTION_NUM_AND_DENOM_VERTICAL_OFFSET = 15.0f;
	static const float scVARIABLE_FRACTION_NUMBER_SCALE = 1.0f;
	static const float scVARIABLE_FRACTION_DIVISION_BAR_WIDTH = scAVERAGE_NUMBER_TEXT_WIDTH*scVARIABLE_FRACTION_NUMBER_SCALE;

	static const int scMAX_SIMPLIFYING_FACTOR = 100;

	//FractionActivityCard

	static const float scACTIVITY_NAME_TEXT_SCALE = 1.0f;

	static const float scACTIVITY_ERROR_MESSAGE_OFFSET_X = 0.0f;
	static const float scACTIVITY_ERROR_MESSAGE_OFFSET_Y = .75*scCARD_HEIGHT_IN_PROJECTOR_COORDS;
	static const float scACTIVITY_ERROR_MESSAGE_SCALE = .75f;
	static const float scACTIVITY_ERROR_MESSAGE_WIDTH = 2*scCARD_WIDTH_IN_PROJECTOR_COORDS;
	static const color * const scACTIVITY_ERROR_MESSAGE_COLOR = &scRED;


	//Comparison Activity


	/*MESSAGES*/
	static const int scMAX_MESSAGE_STRING_LENGTH = 1000;

	static const char * const scADDITION_NAME = "ADDITION";
	static const char * const scSUBTRACTION_NAME = "SUBTRACTION";
	static const char * const scMULTIPLICATION_NAME = "MULTIPLICATION";
	static const char * const scDIVISION_NAME = "DIVISION";
	static const char * const scCOMPARISON_EQUAL_TO_NAME = "FINDING EQUAL FRACTIONS";
	static const char * const scCOMPARISON_LESS_THAN_NAME = "COMPARING UNEQUAL FRACTIONS";
	static const char * const scSORTING_NAME = "SORTING";


	static const char * const scTOO_MANY_COMPARISON_SIGNS_MESSAGE = "There are too many comparison signs on the table";
	static const char * const scTOO_FEW_COMPARISON_SIGNS_MESSAGE = "Please place a comparison sign on the table";

	static const char * const scARITHMETIC_INCORRECT_FORMAT_MESSAGE = "You don't have the right number of fraction cards in your equation.";
	static const char * const scCOMPARISON_INCORRECT_FORMAT_MESSAGE = "You don't have the right number of fraction cards in your comparison";

	static const char * const scCORRECT_MESSAGE = "Correct!";
	static const char * const scINCORRECT_MESSAGE = "Incorrect";
	static const char * const scNEED_TO_SIMPLIFY_MESSAGE = "You need to simplify your answer - ";


	static const char * const scEMPTY_SORTING_MESSAGE = "Place at least one fraction in-between the SMALLEST and LARGEST cards";

	static const char * const scREMOVE_ACTIVITY_MESSAGE_PART_1 = "To do this activity, please remove the";
	static const char * const scREMOVE_ACTIVITY_MESSAGE_PART_2_PLURAL = " activities from the table.";
	static const char * const scREMOVE_ACTIVITY_MESSAGE_PART_2_SINGULAR = " activity from the table.";

	//Hint Messages

	static const char * const scSORTING_HINT_MESSAGE_1 = "Remember, the numerator is the number of pieces you have, "
															"while the denominator is the number of pieces it takes to make a whole.";
	static const char * const scSORTING_HINT_MESSAGE_2= "Think about this. Which of these fractions is bigger? %d/%d or %d/%d?";


	static const char * const scARITHMETIC_HINT_MESSAGE_1 = "Remember, when you multiply and divide fractions you don't need to find a least common denominator. This will just make the numbers bigger, trickier to multiply, and will "
																"force you to reduce a lot later!";
	static const char * const scARITHMETIC_HINT_MESSAGE_2 = "You simplified your fraction part-way, but there is still a common factor that you can take out of the numerator and denominator!";
	static const char * const scARITHMETIC_HINT_MESSAGE_3 = "Which number divides evenly into both %d and %d?";
	static const char * const scARITHMETIC_HINT_MESSAGE_4 = "Make sure that you found the least common denominator instead of finding a number that divides into both denominators";
	static const char * const scARITHMETIC_HINT_MESSAGE_5 = "I think you %s both the numerators and denominators. Remember, when %s fractions, you first have to find a least common denominator. "
																"Then you just %s the numerators and keep the denominators the same."; //added/subtracted, adding/subtracting, add/subtract
	static const char * const scARITHMETIC_HINT_MESSAGE_6 = "Remember, you have to find a least common denominator before you can %s fractions";
	static const char * const scARITHMETIC_HINT_MESSAGE_7 = "Did you forget to flip %d/%d before multiplying?";
	static const char * const scARITHMETIC_HINT_MESSAGE_8 = "You can't cancel common factors out of the fractions until you have flipped the second one!";
	static const char * const scARITHMETIC_HINT_MESSAGE_9 = "Check that you flipped the correct fraction";
	static const char * const scARITHMETIC_HINT_MESSAGE_10 = "You only flip the second fraction when dividing fractions!";
	static const char * const scARITHMETIC_HINT_MESSAGE_12 = "Oops! Looks like you might have %s when you mean to to %s your fractions";
	static const char * const scARITHMETIC_HINT_DEFAULT_MESSAGE = "You should check your arithmetic.";

	static const char * const scCOMPARISON_LESS_THAN_HINT_MESSAGE_1 = "Remember, a bigger denominator does NOT mean that the fraction is bigger.";
	static const char * const scCOMPARISON_LESS_THAN_HINT_MESSAGE_2 = "The bigger fraction goes next to the open end of the comparison sign.";
	static const char * const scCOMPARISON_LESS_THAN_HINT_MESSAGE_3 = "Try finding a common denominator before comparing the fractions.";

	static const char * const scCOMPARISON_EQUAL_TO_HINT_MESSAGE_1 = "You multiply the numerator of %d/%d (%d) by %.0f to get the numerator of %d/%d (%d). "
																		"What would happen if you multiplied the denominator of %d/%d by %.0f?";
	static const char * const scCOMPARISON_EQUAL_TO_HINT_MESSAGE_2 = "You multiply the numerator of %d/%d (%d) by %.2f to get the numerator of %d/%d (%d). "
																			"What would happen if you multiplied the denominator of %d/%d by %.2f?";

	static const char * const scNUMERATOR_LABEL = "How many pieces there are";
	static const char * const scDENOMINATOR_LABEL = "How many pieces it takes to make a whole";





#endif
