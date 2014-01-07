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

#ifndef QuadrilaterValidation2Boxes_HPP
#define QuadrilaterValidation2Boxes_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <wykobi/wykobi.hpp>
#include <set>
#include <list>
#include <string>

namespace decorators {

class QuadrilaterValidation2Boxes : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		QuadrilaterValidation2Boxes(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker,
				float pBox1x1, float pBox1y1, float pBox1x2, float pBox1y2,
				float pBox2x1,float pBox2y1,float pBox2x2,float pBox2y2);

	protected:
		virtual void update();
		char findBoxAssociatedToContent(std::set<FiducialMarker *> &pExpectedContent);
		char findBox(wykobi::point2d<float> tPos);

		float mRelativeBox1x1;
		float mRelativeBox1y1;
		float mRelativeBox1x2;
		float mRelativeBox1y2;
		float mRelativeBox2x1;
		float mRelativeBox2y1;
		float mRelativeBox2x2;
		float mRelativeBox2y2;
		float mPx2CM;
		wykobi::point2d<float> mBox1P1;
		wykobi::point2d<float> mBox1P2;
		wykobi::point2d<float> mBox2P1;
		wykobi::point2d<float> mBox2P2;
		std::set<FiducialMarker *> mExpectedContent1;
		std::set<FiducialMarker *> mExpectedContent2;
		std::list<std::string> mMessage;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
};

}
#endif
