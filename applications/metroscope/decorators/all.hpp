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

#include "Image.hpp"
#include "InterventionLogger.hpp"
#include <qa/components/decorators/TagLogger.hpp>
#include "Logger.hpp"
#include "LogBuffer.hpp"
#include "LogBufferDisplay.hpp"

//carte
#include "Compass.hpp"
#include "Flipper.hpp"

//classification
#include "SetModel.hpp"
#include "SetViewMembership.hpp"

//surfaces
#include "Grid.hpp"

//quadrilateres
#include "QuadModel.hpp"
#include "QuadViewPerimeter.hpp"
#include "QuadViewLines.hpp"
#include "QuadViewSideMeasures.hpp"
#include "QuadViewAngleMeasures.hpp"
#include "QuadrilaterValidation2Boxes.hpp"
#include "QuadrilaterValidation3Boxes.hpp"

//angles
#include "Line.hpp"
#include "Link.hpp"
#include "AngleModel.hpp"
#include "AngleViewMeasure.hpp"
#include "AngleViewArc.hpp"

//space junk
#include "JunkModel.hpp"
#include "JunkViewSpace.hpp"
#include "JunkViewLaser.hpp"
#include "JunkShot.hpp"
#include "JunkLaserSelector.hpp"

//symmetry
#include "Symmetry.hpp"
#include "Mirror.hpp"
#include "MultipleMirror.hpp"
#include "LimitedMultipleMirror.hpp"
#include "QuadMirror.hpp"
#include "AxisModel.hpp"
#include "FoldedMirror.hpp"


//fractions
/*#include "FractionCard.hpp"
#include "FractionModel.hpp"
#include "FractionSortingModel.hpp"
#include "FractionArithmeticModel.hpp"
#include "FractionComparisonModel.hpp"
#include "NumberDial.hpp"
#include "VariableFractionCard.hpp"
#include "VariableFractionController.hpp"
#include "FractionActivityCard.hpp"
#include "FractionAnswerChecker.hpp"
#include "FractionHint.hpp"
#include "FractionSimplifier.hpp"

//silly examples
#include "SimpleText.hpp"

//arithmetics
#include "SumClassificationModel.hpp"
//#include "SumClassification.hpp"
#include "NumberCard.hpp"
#include "NumberActivityCard.hpp"
//#include "RhombusPuzzle.hpp"
#include "BlankNumberCard.hpp"
#include "BlankNumberModel.hpp"
#include "BlankActivityCard.hpp"
#include "RegroupDigits.hpp"
#include "Regrouper.hpp"
*/
//proportions
//#include "ProportionView.hpp"
#include "DenominatorsModel.hpp"
#include "DenominatorCard.hpp"
#include "TokenModel.hpp"
#include "FractionToken.hpp"
#include "RectangleFractionModel.hpp"
#include "Carte.hpp"
#include "KillBugModel.hpp"
#include "FractionCard.hpp"
#include "CircularFractionModel.hpp"
#include "FractionBugHint.hpp"
#include "FractionViewMeasure.hpp"
#include "EquivalentFractionView.hpp"
#include "FractionComparisionView.hpp"
#include "FractionSum.hpp"

//networking
#include "OrchestrationPauseFlipper.hpp"


//Battleship
#include "PolyModel.hpp"
#include "PolyViewPerimeter.hpp"
