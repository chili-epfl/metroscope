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

#ifndef RhombusPuzzle_HPP
#define RhombusPuzzle_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <wykobi/wykobi.hpp>
#include <set>
#include <list>
#include <string>

namespace decorators{

class RhombusPuzzle : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		RhombusPuzzle (DecoratorManager &pDecoratorManager, FiducialMarker *pMarker,
				float pTopBoxX, float pTopBoxY);

	protected:
		virtual void update();
		char findBoxAssociatedToContent (FiducialMarker *pExpectedContent);
		char findBox(wykobi::point2d<float> tPos);
		void drawPuzzle();
		void checkAnswer();
		wykobi::point2d<float> mRelativeBox0;
		wykobi::point2d<float> mRelativeBox1;
		wykobi::point2d<float> mRelativeBox2;
		wykobi::point2d<float> mRelativeBox3;
		wykobi::point2d<float> mRelativeBox4;
		wykobi::point2d<float> mRelativeBox5;
		wykobi::point2d<float> mRelativeBox6;
		wykobi::point2d<float> mRelativeBox7;
		wykobi::point2d<float> mRelativeBox8;
		wykobi::point2d<float> mRelativeBox9;

		wykobi::point2d<float> mRelativeBox6_2;
		wykobi::point2d<float> mRelativeBox6_3;
		wykobi::point2d<float> mRelativeBox7_2;
		wykobi::point2d<float> mRelativeBox8_2;
		wykobi::point2d<float> mRelativeBox9_2;
		wykobi::point2d<float> mRelativeBox9_3;

		//std::vector<FiducialMarker *> mExpectedContent;
		std::vector<int> mExpectedContent;
		std::vector<int> mContentIsShown;

		//std::vector<wykobi::point2d<float>> mBox;

		wykobi::point2d<float> *mBox = new wykobi::point2d<float> [10];

		wykobi::point2d<float> mBox0;
		wykobi::point2d<float> mBox1;
		wykobi::point2d<float> mBox2;
		wykobi::point2d<float> mBox3;
		wykobi::point2d<float> mBox4;
		wykobi::point2d<float> mBox5;
		wykobi::point2d<float> mBox6;
		wykobi::point2d<float> mBox7;
		wykobi::point2d<float> mBox8;
		wykobi::point2d<float> mBox9;
		wykobi::point2d<float> mBox6_2;
		wykobi::point2d<float> mBox6_3;
		wykobi::point2d<float> mBox7_2;
		wykobi::point2d<float> mBox8_2;
		wykobi::point2d<float> mBox9_2;
		wykobi::point2d<float> mBox9_3;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

};
}
#endif
