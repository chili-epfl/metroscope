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

#ifndef NumberCard_HPP
#define NumberCard_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>

//COLORS
	struct color{
		float r;
		float g;
		float b;
		float a;
		color(float pR, float pG, float pB, float pA = 1.0f) {r = pR; g = pG; b = pB; a = pA;}
	};

	static const float scREAL_WORLD_MARKER_WIDTH_MM = 20.0f;
	static const float scREAL_WORLD_MARKER_HEIGHT_MM = 20.0f;
	static const char * const scTEXT_DELIMITERS = " -";
	static const float scCARD_WIDTH_IN_PROJECTOR_COORDS = 111.4f;
	static const float scCARD_HEIGHT_IN_PROJECTOR_COORDS = 154.1f;
	static const float scHINT_MESSAGE_SCALE = 1.5f;
	static const float scHINT_MESSAGE_OFFSET_X = 0.2*scCARD_WIDTH_IN_PROJECTOR_COORDS;
	static const float scHINT_MESSAGE_OFFSET_Y = -0.2*scCARD_HEIGHT_IN_PROJECTOR_COORDS;
	static const float scHINT_MESSAGE_WIDTH = 0.5*scCARD_WIDTH_IN_PROJECTOR_COORDS;
	static const color scGREEN			(0.1725f, 0.7451f, 0.2941f);
	static const color scBLACK			(0.0f, 0.0f, 0.0f);
	static const color * const scHINT_MESSAGE_COLOR = &scBLACK;

namespace decorators {

class NumberCard : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		NumberCard(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const int pNumber);
		virtual ~NumberCard(){};

		int GetNumber() const {return mNumber;}
		bool IsPresent()const {return mMarker->isPresent();}
		wykobi::point2d<float> GetLocation() const {
			wykobi::point2d<float> tLocation;
			if (mMarker->isPresent())return mMarker->getCenter();
			else return tLocation;
		}

		const FiducialMarker *GetMarker() const {return mMarker;}


	protected:
		void update(){}
		int mNumber;



	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;


};

}
#endif
