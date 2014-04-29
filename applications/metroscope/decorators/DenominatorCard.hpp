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

#ifndef DENOMINATORCARD_HPP_
#define DENOMINATORCARD_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>

namespace decorators{
	class DenominatorCard : public FiducialDecorator
	{
	public:
		static FiducialDecorator *create (libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		DenominatorCard (DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, int pDenominator);
		~DenominatorCard (){}

		int GetDenominator() const {return mDenominator;}
		bool IsPresent()const {return mMarker->isPresent();}
		wykobi::point2d<float> GetLocation() const {
			wykobi::point2d<float> tLocation;
			if (mMarker->isPresent())return mMarker->getCenter();
			else return tLocation;
		}
		const FiducialMarker *GetMarker() const {return mMarker;}

	protected:
			void update();

	private:
			static const std::string scDecoratorName;
			static const DecoratorManager::Registerer mRegisterer;
			const int mDenominator;
	};
}

#endif /* DENOMINATORCARD_HPP_ */
