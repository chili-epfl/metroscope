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

#ifndef FLIPPERKILLBUG_HPP_
#define FLIPPERKILLBUG_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "Flipper.hpp"

namespace decorators {

class FlipperKillBug : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		FlipperKillBug (DecoratorManager &pDecoratorManager , Flipper *pFlipper , int pType); // 0 non oriented ; 1 highly oriented
		~FlipperKillBug();

		int GetType() {return mType;}
		Flipper* GetFlipper() {return mFlipper;}
		bool IsPresent() {return mFlipper->IsPresent();}
		bool IsFlipped() const {return mFlipper->IsFlipped();}
		const FiducialMarker *GetCurrentSide() const {return mFlipper->GetCurrentSide();}


	protected:
		void update();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		Flipper *mFlipper;
		int mType;
};

}

#endif /* FLIPPERKILLBUG_HPP_ */
