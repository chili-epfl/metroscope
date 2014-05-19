/*
 * ProportionView.hpp
 *
 *  Created on: Apr 23, 2014
 *      Author: daniela
 */

#ifndef PROPORTIONVIEW_HPP_
#define PROPORTIONVIEW_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "AngleModel.hpp"
#include "DenominatorsModel.hpp"
#include "Flipper.hpp"
#include "TokenModel.hpp"
#include "RectangleFractionModel.hpp"
#include "Carte.hpp"
#include <fstream>

namespace decorators {

class ProportionView : public FiducialDecorator
{
	public:
		static FiducialDecorator *create (libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		ProportionView (DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, AngleModel *pAngleModel, DenominatorsModel *pDenomModel, Flipper *pFlipper, TokenModel *pTokenModel, RectangleFractionModel *pRectangleModel, Carte *pCarte);

	protected:
		void update();

		AngleModel *mAngleModel;
		DenominatorsModel *mDenomModel;
		Flipper *mFlipper;
		TokenModel *mTokenModel;
		RectangleFractionModel *mRectangleModel;
		Carte *mCarte;

		void DisplayMap();
		void DrawRectangleProportion();
		void DrawCircunference();
		void DivideCircunference(int pParts);
		void DivideRectangleProportion(int pParts);
		void DrawSaveRectangles();
		void SaveFraction();
		void ShowProportion(int pNumerator, int pDenominator);
		void ShowProportion(float pProportion);
		void ShowProportion(float pProportion,wykobi::point2d<float>);
		void ShowSquare();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
		float mOriginBox1X;
		float mOriginBox1Y;
		float mBoxWidth;
		float mBoxHeight;

		float mOriginBox1Y2;
		float mOriginBox1X4;

		float mOriginBox1X2, mOriginBox1X3;
		float mOriginBox1Y3, mOriginBox1Y4;

		float mAngle;
		float mProportion;
};
}

#endif /* PROPORTIONVIEW_HPP_ */
