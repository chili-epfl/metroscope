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

#ifndef AngleShoot_HPP
#define AngleShoot_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>

namespace decorators {

class AngleShoot : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		AngleShoot(DecoratorManager &pDecoratorManager, FiducialMarker *pOrigin, FiducialMarker *pStart, FiducialMarker *pEnd, bool pClockwise);
		AngleShoot(DecoratorManager &pDecoratorManager, FiducialMarker *pOrigin, FiducialMarker *pEnd, bool pClockwise);

		bool isPresent() const { return mMarker->isPresent() && (!mStart || mStart->isPresent()) && mEnd->isPresent(); }

		bool Clockwise() const {return mOrientation == wykobi::Clockwise;}
		const wykobi::point2d<float> &OriginPoint() const {return mOriginPoint;}
		const wykobi::point2d<float> &StartPoint() const {return mStartPoint;}
		const wykobi::point2d<float> &EndPoint() const {return mEndPoint;}
		float Angle() const {return mAngle;}


		static void NormalizeAngle(float &pAngle);
		static float AbsoluteDifference(float pAngle1, float pAngle2);

	protected:
		void update();

		FiducialMarker *mStart;
		FiducialMarker *mEnd;
		int mOrientation;

		wykobi::point2d<float> mOriginPoint;
		wykobi::point2d<float> mStartPoint;
		wykobi::point2d<float> mEndPoint;
		float mAngle;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
};

}
#endif
