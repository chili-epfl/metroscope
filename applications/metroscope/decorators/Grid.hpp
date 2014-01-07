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

#ifndef Grid_HPP
#define Grid_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>

namespace decorators {

class Grid : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		Grid(DecoratorManager &pDecoratorManager, 
				FiducialMarker *pMarker,
				float pRed, float pGreen, float pBlue,
				float pCellWidth, float pCellHeight,
				float pGridSize = scInfiniteGridSize);

	protected:
		void update();

		static const float scInfiniteGridSize;

		float mRed;
		float mGreen;
		float mBlue;
		float mCellWidth;
		float mCellHeight;
		float mHalfGridSize;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
};

}
#endif
