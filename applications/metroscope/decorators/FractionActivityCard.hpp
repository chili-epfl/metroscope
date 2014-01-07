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

#ifndef FractionActivityCard_HPP
#define FractionActivityCard_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "FractionModel.hpp"
#include "FractionAnswerChecker.hpp"

namespace decorators {

class FractionActivityCard : public FiducialDecorator
{
	public:
		FractionActivityCard(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker,
								FiducialMarker *pStabilizedGraphicsMarker,
								FractionModel *pModel,
								FractionAnswerChecker *pChecker,
								const char * const pActivityName,
								const color *pActivityColor);
		virtual ~FractionActivityCard();


	protected:
		void update();
		virtual void displayErrorMessage();

		FiducialMarker *mStabilizedGraphicsMarker;

		FractionModel *mFractionModel;
		FractionAnswerChecker *mChecker;

		const char *const mActivityName;
		const color * const mActivityColor;

		const char *mErrorMessage;
		const color *mErrorMessageColor;
		bool mOperatorsPresent;
		bool mOperandsPresent;

		bool mPreviouslyCurrentActivity; //Tells whether this card was the current activity on the last cycle

		char *mStringBuffer;
		char *mErrorStringBuffer;

		void PlaceCenteredTextAtLocationRelativeToMarker(const char *pText, const FiducialMarker *pMarker, float pTextOffsetX, float pTextOffsetY, float pTextScale, float pTextWidth, float pR, float pG, float pB, float pA);


		void displayActivityGraphics();
		void drawScreenBorder(float pThickness, const color *pColor);
		const char *otherActivityCardsPresentMessage(); //Returns a pointer to the String Buffer, which contains the message



	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		/*Returns true if the proper operators necessary for the activity are present (equals sign, plus sign, etc). Else,
		 * updates mErrorMessage and returns false.
		 * */
		virtual bool properOperatorsPresent() = 0;

		/* Sets operand fields to NULL. Then finds and stores the proper operands in appropriate fields for the activity.
		 * If there is an error, updates mErrorMessage and returns false. Else, returns true.
		 *  */
		virtual bool getOperands() = 0;

		/* Returns true if the equation is correct given the current values in the operand fields. Else, returns false.*/
		virtual bool answerIsCorrect () = 0;

		/*Must update mMessage to say correct. Resets the current hint state.*/
		//virtual void notifyCorrect() = 0;

		/*Updates hint data stored for the various activity. Updates the FractionModel message if necessary*/
		virtual void updateHintState() = 0;

		/*Updates which cards are in currently being used by the activity (the cards in the equation or in between the sorting markers)*/
		virtual void updateActiveCards() = 0;




};

}
#endif
