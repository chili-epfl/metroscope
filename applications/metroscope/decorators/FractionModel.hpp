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

#ifndef FractionModel_HPP
#define FractionModel_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <wykobi/wykobi.hpp>
#include "FractionCard.hpp"
#include "VariableFractionCard.hpp"
#include <list>
#include <queue>
#include <deque>

namespace decorators {

class FractionModel : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		FractionModel(DecoratorManager &pDecoratorManager, FractionCard **pCards, const int pNumCards, VariableFractionCard *pVariableFractionCard);
		~FractionModel();

		FractionCard **GetCardArray() const {return mCards;}
		int GetNumCards() const {return mNumCards;}


		std::vector<FractionCard *> & GetActiveCards();
		void ClearActiveCards();


		void SetCurrentActivity (const char *pActivityName);
		const char *GetCurrentActivity () const;
		void RemoveActivity(const char *pActivityName);
		std::vector<const char*> & GetPresentActivities();

		int AddHintType(const HintType pHintType); //returns hint number that the hint card is (for graphics placing purposes)
		void RemoveHintType(const HintType pHintType);
		bool HintTypeIsPresent(const HintType pHintType);

		void ResetHintState();
		void IncreaseHintLevel();
		int	GetHintLevel();


		void SetHintErrorType(ErrorType pType);
		ErrorType GetHintErrorType();

		const char *GetHintMessage();
		void SetHintMessage(const char *pMessage);
		void SetHintMessage(char *pMessage);
		void SetHintMessageGraphicsType(HintType pType);
		HintType GetHintMessageGraphicsType();
		void SetHintMessageGraphicsFractions(int pNum1, int pDenom1, int pNum2, int pDenom2);
		void GetHintMessageGraphicsFractions(int &pNum1, int &pDenom1, int &pNum2, int &pDenom2);



		void getCardsInsideRectangle(std::vector<FractionCard *> & pCardArray, wykobi::point2d<float> pOrigin, wykobi::vector2d<float> pTransformXVector, wykobi::vector2d<float> pTransformYVector, float pXMin, float pXMax, float pYMin, float pYMax);

		static float CalculateTransformAngle(wykobi::point2d<float> &pOrigin, wykobi::point2d<float> &pPointOnPosXAxis);
		static void transformPoint(wykobi::point2d<float> & pPoint, wykobi::point2d<float> pOrigin, float pTransformAngle);

		bool IsReduced(int pNumerator, int pDenominator);
		int GCD(int x, int y);
		int LCM(int x, int y);
		int FractionLCD(std::vector<FractionCard *> &pFractions);
		void getClosestFactors(int pNumber, int &pSmallFactor, int &pLargeFactor);


		static bool FractionCardSortByValue(const decorators::FractionCard *card1, const decorators::FractionCard *card2);


	protected:
		void update();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		const int mNumCards;
		FractionCard **mCards; //array of pointers to all of the FractionCard objects
		VariableFractionCard *mVariableFractionCard;

		std::vector<const char *> mPresentActivities; //front of this vector is the current activity

		std::vector<FractionCard *> mActiveCards;
		std::vector<HintType> mCurrentHintTypes;


		struct HintState{
			int level;  //How extensive of a hint do they need? i.e. How many times have they been wrong?
			static const int numLevels = 5;
			ErrorType errorType;
			const char *message;
			char *strBuffer;
			HintType messageGraphicsType;

			int graphicsNum1;
			int graphicsDenom1;
			int graphicsNum2;
			int graphicsDenom2;

			HintState(){level = 0; errorType = Unknown; message = ""; strBuffer = new char[scMAX_MESSAGE_STRING_LENGTH];}
			~HintState(){delete strBuffer;}
		};

		HintState *mHintState;

		bool valueIsInRange(float &pValue, float pRangeStart, float pRangeEnd);





};

}
#endif
