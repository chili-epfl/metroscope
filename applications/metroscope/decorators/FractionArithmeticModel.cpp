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

#include "FractionArithmeticModel.hpp"
#include <iostream>

const std::string decorators::FractionArithmeticModel::scDecoratorName("FractionArithmeticModel");
const  DecoratorManager::Registerer decorators::FractionArithmeticModel::mRegisterer(decorators::FractionArithmeticModel::scDecoratorName, &decorators::FractionArithmeticModel::create);

decorators::FiducialDecorator *decorators::FractionArithmeticModel::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		OperatorType pOperatorType;
		const char *pActivityName;
		const color *pActivityColor;
		switch ((int)pSetting["operator"]) {
				case 0: pOperatorType = Addition; pActivityName = scADDITION_NAME;
						pActivityColor = scADDITION_ACTIVITY_COLOR;
						break;
				case 1: pOperatorType = Subtraction;  pActivityName = scSUBTRACTION_NAME;
						pActivityColor = scSUBTRACTION_ACTIVITY_COLOR;
						break;
				case 2: pOperatorType = Multiplication;  pActivityName = scMULTIPLICATION_NAME;
						pActivityColor = scMULTIPLICATION_ACTIVITY_COLOR;
						break;
				case 3: pOperatorType = Division; pActivityName = scDIVISION_NAME;
						pActivityColor = scDIVISION_ACTIVITY_COLOR;
						break;
		}

		return new decorators::FractionArithmeticModel(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]),
														pDecoratorManager.loadMarker(pSetting["stabilized_graphics_marker"]),
														(FractionModel *)pDecoratorManager.loadDecorator(pSetting["fraction_model"]),
														(FractionAnswerChecker *)pDecoratorManager.loadDecorator(pSetting["answer_checker"]),
														pDecoratorManager.loadMarker((pSetting["equals_marker"])),
														pOperatorType,
														pActivityName,
														pActivityColor);
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::FractionArithmeticModel::FractionArithmeticModel(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, FiducialMarker *pStabilizedGraphicsMarker,
																FractionModel *pModel,
																FractionAnswerChecker *pChecker,
																FiducialMarker *pEqualsSign, OperatorType pOperatorType,
																const char* pActivityName, const color *pActivityColor):
FractionActivityCard(pDecoratorManager, pMarker, pStabilizedGraphicsMarker, pModel, pChecker, pActivityName, pActivityColor),
mOperand1(NULL),
mOperand2(NULL),
mResult(NULL),
mEqualsMarker(pEqualsSign),
mOperatorType(pOperatorType)
{

}
decorators::FractionArithmeticModel::~FractionArithmeticModel(){
	/*Empty*/
}


/*Returns true if the proper operators necessary for the activity are present (equals sign, plus sign, etc)*/
bool decorators::FractionArithmeticModel::properOperatorsPresent(){
	if(!mEqualsMarker->isPresent()) mErrorMessage = "Please place the EQUALS sign on the table";
	return mEqualsMarker->isPresent();
}

/*Sets operand fields to NULL. Then finds and stores the proper operands in appropriate fields for the activity.
 * Returns false if there is an error. Else, returns true.
 */
bool decorators::FractionArithmeticModel::getOperands(){
	wykobi::point2d<float> tOperatorCenter = mMarker->getCenter();//mOperationMarker->getCenter();
	wykobi::point2d<float> tEqualsCenter = mEqualsMarker->getCenter();
	mDecoratorManager.GetCam2World().InterpolatedMap(tOperatorCenter);
	mDecoratorManager.GetCam2World().InterpolatedMap(tEqualsCenter);

	wykobi::vector2d<float> tXVector = wykobi::make_vector(tEqualsCenter.x - tOperatorCenter.x, tEqualsCenter.y - tOperatorCenter.y);
	wykobi::vector2d<float> tYVector = -1.0f*wykobi::perpendicular(tXVector);

	float tXRange = wykobi::distance(tOperatorCenter, tEqualsCenter); //VECTOR NORM

	std::vector<decorators::FractionCard *> tCardArray;


	mFractionModel->getCardsInsideRectangle(tCardArray, tOperatorCenter, tXVector, tYVector, -tXRange, 0.0f, -scCARD_HEIGHT, scCARD_HEIGHT);
	if(tCardArray.size() != 1) {
		mErrorMessage = scARITHMETIC_INCORRECT_FORMAT_MESSAGE;
		return false;
	}
	tXVector.x > 0 ? mOperand1 = tCardArray[0] : mOperand2 = tCardArray[0] ;
	tCardArray.clear();

	mFractionModel->getCardsInsideRectangle(tCardArray, tOperatorCenter, tXVector, tYVector, 0, tXRange, -scCARD_HEIGHT, scCARD_HEIGHT);
	if(tCardArray.size() != 1) {
		mErrorMessage = scARITHMETIC_INCORRECT_FORMAT_MESSAGE;
		return false;
	}
	tXVector.x > 0 ? mOperand2 = tCardArray[0] : mOperand1 = tCardArray[0] ;
	tCardArray.clear();


	mFractionModel->getCardsInsideRectangle(tCardArray, tEqualsCenter, tXVector, tYVector, 0, tXRange, -scCARD_HEIGHT, scCARD_HEIGHT);
	if(tCardArray.size() != 1) {
		mErrorMessage = scARITHMETIC_INCORRECT_FORMAT_MESSAGE;
		return false;
	}
	mResult = tCardArray[0];
	return true;


}

/*Returns true if the equation is correct given the current values in the operand fields. Else, returns false.*/
bool decorators::FractionArithmeticModel::answerIsCorrect (){
	return operation(mOperand1, mOperand2) == mResult->GetValue() && mFractionModel->IsReduced(mResult->GetNumerator(), mResult->GetDenominator());

}



/*Updates hint data stored for the various activity. Updates the FractionModel message if necessary*/
void decorators::FractionArithmeticModel::updateHintState(){
	if(operation(mOperand1, mOperand2) == mResult->GetValue()){ //Answer is correct, just need to simplify
		sprintf(mStringBuffer, scNEED_TO_SIMPLIFY_MESSAGE);
		if((mOperatorType == Multiplication || mOperatorType == Division) && foundLCD()){
			sprintf(mStringBuffer + strlen(scNEED_TO_SIMPLIFY_MESSAGE), scARITHMETIC_HINT_MESSAGE_1); //Remember, in multiplication and Division you don't have to find a LCD
		} else if(answerIsPartiallyReduced()){ //Check if they reduced partially, but not all the way
			sprintf(mStringBuffer + strlen(scNEED_TO_SIMPLIFY_MESSAGE), scARITHMETIC_HINT_MESSAGE_2);
		} else { //No reduction
			sprintf(mStringBuffer + strlen (scNEED_TO_SIMPLIFY_MESSAGE), scARITHMETIC_HINT_MESSAGE_3, mResult->GetNumerator(), mResult->GetDenominator());
		}
		mFractionModel->SetHintMessage(mStringBuffer);
	} else{
		switch(mOperatorType){
			case Addition: case Subtraction:
				if (wrongOperation()){ /*Note: wrongOperation() updates the hint message based on what type of mistake was made*/}//check for wrong operator(i.e. multiplying instead of adding)
				else if(operationOnBothNumeratorAndDenominator()) {
					sprintf(mStringBuffer, scARITHMETIC_HINT_MESSAGE_5, mOperatorType == Addition?"added":"subtracted", mOperatorType == Addition?"adding":"subtracting", mOperatorType == Addition?"add":"subtract");
					mFractionModel->SetHintMessage(mStringBuffer);
				}//check for adding/subtracting both the numerator and the denominator
				else if (operationOnNumeratorsAndChoseADenominator()) {
					sprintf(mStringBuffer, scARITHMETIC_HINT_MESSAGE_6, mOperatorType == Addition?"add":"subtract");
					mFractionModel->SetHintMessage(mStringBuffer);
				}//check for adding/subtracting numerators, but just choosing one denominator? I.E. not finding the LCD
				else if(denominatorIsGCDInsteadOfLCM()){mFractionModel->SetHintMessage(scARITHMETIC_HINT_MESSAGE_4);}//Check for finding a number that goes into both of the denominators, NOT a number that both denominators go into.
				else{mFractionModel->SetHintMessage(scARITHMETIC_HINT_DEFAULT_MESSAGE);} /*DEFAULT ERROR MESSAGE*/
				break;
			case Multiplication:
				if(foundLCD()){mFractionModel->SetHintMessage(scARITHMETIC_HINT_MESSAGE_1);/*Remember, you don't need find an LCD when multiplying fractions. This will just make the numbers bigger, trickier to multiply, and will make it so you have to reduce a lot later!*/}
				else if (wrongOperation()){ /*Note: wrongOperation() updates the hint message based on what type of mistake was made*/} //wrongOperation should update error message based on what type of mistake was made
				else { mFractionModel->SetHintMessage(scARITHMETIC_HINT_DEFAULT_MESSAGE);	} /*DEFAULT ERROR MESSAGE*/
				break;
			case Division:
				if (forgotToFlipFraction()){
					sprintf(mStringBuffer, scARITHMETIC_HINT_MESSAGE_7, mOperand2->GetNumerator(), mOperand2->GetDenominator());
					mFractionModel->SetHintMessage(mStringBuffer);
				}
				else if (cancelledBeforeFlipping()){mFractionModel->SetHintMessage(scARITHMETIC_HINT_MESSAGE_8);} //check for canceling before you flip
				else if (wrongFractionFlipped()){mFractionModel->SetHintMessage(scARITHMETIC_HINT_MESSAGE_9);} //check for flipping the wrong fraction
				else if (bothFractionsFlipped()){mFractionModel->SetHintMessage(scARITHMETIC_HINT_MESSAGE_10);} //check for flipping both fractions
				else if (wrongOperation()){/*Note: wrongOperation() updates the hint message based on what type of mistake was made*/} //check for wrong operator(i.e. multiplying instead of adding)
				else {mFractionModel->SetHintMessage(scARITHMETIC_HINT_DEFAULT_MESSAGE);}/*DEFAULT ERROR MESSAGE*/
				break;
		}

	}

}


void decorators::FractionArithmeticModel::updateActiveCards(){
	mFractionModel->GetActiveCards().clear();
	mFractionModel->GetActiveCards().push_back(mOperand1);
	mFractionModel->GetActiveCards().push_back(mOperand2);
	mFractionModel->GetActiveCards().push_back(mResult);
}


float decorators::FractionArithmeticModel::operation(FractionCard *pOperand1, FractionCard *pOperand2, OperatorType pOperatorType){
	float tNum1 = pOperand1->GetNumerator();
	float tNum2 = pOperand2->GetNumerator();
	float tDenom1 = pOperand1->GetDenominator();
	float tDenom2 = pOperand2->GetDenominator();

	switch (pOperatorType) {
			case Addition:
				return (tNum1*tDenom2 + tNum2*tDenom1)/(tDenom1*tDenom2);
			case Subtraction:
				return (tNum1*tDenom2 - tNum2*tDenom1)/(tDenom1*tDenom2);
			case Multiplication:
				return (tNum1*tNum2)/(tDenom1*tDenom2);
			case Division:
				return (tNum1*tDenom2)/(tDenom1*tNum2);
		}
	return -1.0f;
}
float decorators::FractionArithmeticModel::operation(FractionCard *pOperand1, FractionCard *pOperand2){
	return operation(pOperand1, pOperand2, mOperatorType);
}

//Checks if a student found an LCD before multiplying or dividing and THEN multiplied
bool decorators::FractionArithmeticModel::foundLCD(){
	float tResultDenominator = mResult->GetDenominator();
	float tLCD = mFractionModel->LCM(mOperand1->GetDenominator(), mOperand2->GetDenominator());
	if(mOperatorType == Division) {
		float tLCD2 = mFractionModel->LCM(mOperand1->GetDenominator(), mOperand2->GetNumerator()); //If they flipped and THEN found the LCD;
		return  tLCD*tLCD*mOperand2->GetValue() == tResultDenominator || tLCD2*tLCD2 == tResultDenominator;
	}
	return tLCD*tLCD == mResult->GetDenominator();
}
bool decorators::FractionArithmeticModel::answerIsPartiallyReduced(){
	float tUnreducedDenominator;
	if (mOperatorType == Addition || mOperatorType == Subtraction){ tUnreducedDenominator = mFractionModel->LCM(mOperand1->GetDenominator(), mOperand2->GetDenominator());}
	else if (mOperatorType == Division){ tUnreducedDenominator = mOperand1->GetDenominator()*mOperand2->GetNumerator();}
	else {tUnreducedDenominator = mOperand1->GetDenominator()*mOperand2->GetDenominator();}
	return tUnreducedDenominator > mResult->GetDenominator();
}
bool decorators::FractionArithmeticModel::wrongOperation(){

	bool wrongOperation = false;
	const char *tAccidentalOperation;
	if(mOperatorType != Addition && operation(mOperand1, mOperand2, Addition) == mResult->GetValue()){
		tAccidentalOperation = "added";
		wrongOperation = true;
	}
	else if(mOperatorType != Subtraction && operation(mOperand1, mOperand2, Subtraction) == mResult->GetValue()){
		tAccidentalOperation = "subtracted";
		wrongOperation = true;
	}
	else if(mOperatorType != Multiplication && operation(mOperand1, mOperand2, Multiplication) == mResult->GetValue()){
		tAccidentalOperation = "multiplied";
		wrongOperation = true;
	}
	else if(mOperatorType != Division && operation(mOperand1, mOperand2, Division) == mResult->GetValue()){
		tAccidentalOperation = "divided";
		wrongOperation = true;
	}
	if(wrongOperation){
		const char *tDesiredOperation = "";
		switch(mOperatorType){
		case Addition: tDesiredOperation = "add"; break;
		case Subtraction: tDesiredOperation = "subtract"; break;
		case Multiplication: tDesiredOperation = "multiply"; break;
		case Division: tDesiredOperation = "divide"; break;
		}
		sprintf(mStringBuffer, scARITHMETIC_HINT_MESSAGE_12, tAccidentalOperation, tDesiredOperation);
		//mHintState->hintMessage = mStringBuffer;
	}
	return wrongOperation;
}
bool decorators::FractionArithmeticModel::denominatorIsGCDInsteadOfLCM(){
	return mFractionModel->GCD(mOperand1->GetDenominator(), mOperand2->GetDenominator()) == mResult->GetDenominator();
}
bool decorators::FractionArithmeticModel::operationOnBothNumeratorAndDenominator(){
	if(mOperatorType == Addition){
		return ((float)(mOperand1->GetNumerator() + mOperand2->GetNumerator()))/(mOperand1->GetDenominator() + mOperand2->GetDenominator()) == mResult->GetValue();
	}
	else if (mOperatorType == Subtraction){
		return ((float)(mOperand1->GetNumerator() - mOperand2->GetNumerator()))/(mOperand1->GetDenominator() - mOperand2->GetDenominator()) == mResult->GetValue();
	}
	return false;
}
bool decorators::FractionArithmeticModel::operationOnNumeratorsAndChoseADenominator(){
	if(mOperatorType == Addition){
			return ((float)(mOperand1->GetNumerator() + mOperand2->GetNumerator()))/(mOperand1->GetDenominator()) == mResult->GetValue()
					|| ((float)(mOperand1->GetNumerator() + mOperand2->GetNumerator()))/(mOperand2->GetDenominator()) == mResult->GetValue();
	}
	else if (mOperatorType == Subtraction){
		return ((float)(mOperand1->GetNumerator() - mOperand2->GetNumerator()))/(mOperand1->GetDenominator()) == mResult->GetValue()
							|| ((float)(mOperand1->GetNumerator() - mOperand2->GetNumerator()))/(mOperand2->GetDenominator()) == mResult->GetValue();
	}
		return false;
}
bool decorators::FractionArithmeticModel::forgotToFlipFraction(){
	return operation(mOperand1, mOperand2, Multiplication)== mResult->GetValue();
}
bool decorators::FractionArithmeticModel::cancelledBeforeFlipping(){
	int tNum1 = mOperand1->GetNumerator();
	int tNum2 = mOperand2->GetNumerator();
	int tDenom1 = mOperand1->GetDenominator();
	int tDenom2 = mOperand2->GetDenominator();

	if(mFractionModel->GCD(tNum1, tDenom2) != 1){
		for(int i = 1; i <= std::min(tNum1, tDenom2); i++){
			if(tNum1%i == 0 && tDenom2%i == 0){
				if(operation(mOperand1, mOperand2)/(i*i) == mResult->GetValue()) return true;
			}
		}
	}
	if(mFractionModel->GCD(tNum2, tDenom1) != 1){
		for(int i = 1; i <= std::min(tNum1, tDenom2); i++){
			if(tNum1%i == 0 && tDenom2%i == 0){
				if(operation(mOperand1, mOperand2)*(i*i) == mResult->GetValue()) return true;
			}
		}
	}

	return false;

}
bool decorators::FractionArithmeticModel::wrongFractionFlipped(){
	return operation(mOperand2, mOperand1) == mResult->GetValue();
}
bool decorators::FractionArithmeticModel::bothFractionsFlipped(){
	return 1/operation(mOperand1, mOperand2, Multiplication) == mResult->GetValue();
}


