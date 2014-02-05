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

#include "NumberActivityCard.hpp"
#include <iostream>

const std::string decorators::NumberActivityCard::scDecoratorName("NumberActivityCard");
namespace{


}

	decorators::NumberActivityCard::NumberActivityCard(DecoratorManager &pDecoratorManager,
			FiducialMarker *pMarker,
			NumberModel *pModel,
			const char * const pActivityName):
				FiducialDecorator(pDecoratorManager, pMarker),
				mNumberModel(pModel),
				mActivityName(pActivityName),
				mPreviouslyCurrentActivity(false),
				mOperatorsPresent(false)
				{
				}

decorators::NumberActivityCard::~NumberActivityCard(){

}

void decorators::NumberActivityCard::update() {
	if(mMarker->isPresent()){
		showInstruction(); //FIXME: The instruction is always blinking
	}
}

void decorators::NumberActivityCard::showInstruction(){
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderText("Group the numbers that add up XX", 300.0f, 100.0f, scHINT_MESSAGE_SCALE,scHINT_MESSAGE_COLOR->r, scHINT_MESSAGE_COLOR->g, scHINT_MESSAGE_COLOR->b, scHINT_MESSAGE_COLOR->a);
	mDecoratorManager.GetDisplay().PopTransformation();
}


