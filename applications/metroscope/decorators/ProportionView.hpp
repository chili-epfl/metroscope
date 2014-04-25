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
#include <fstream>

namespace decorators {

class ProportionView : public FiducialDecorator
{
	public:
		static FiducialDecorator *create (libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		ProportionView (DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, AngleModel *pAngleModel);

	protected:
		void update();

		AngleModel *mAngleModel;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
};
}

#endif /* PROPORTIONVIEW_HPP_ */
