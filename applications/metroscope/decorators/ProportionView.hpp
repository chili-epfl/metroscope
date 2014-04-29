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
#include <fstream>

namespace decorators {

class ProportionView : public FiducialDecorator
{
	public:
		static FiducialDecorator *create (libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		ProportionView (DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, AngleModel *pAngleModel, DenominatorsModel *pDenomModel);

	protected:
		void update();

		AngleModel *mAngleModel;
		DenominatorsModel *mDenomModel;

		void DrawRectangleProportion();
		void DrawCircunference();
		void DivideCircunference(int pParts);
		void DivideRectangleProportion(int pParts);

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
};
}

#endif /* PROPORTIONVIEW_HPP_ */
