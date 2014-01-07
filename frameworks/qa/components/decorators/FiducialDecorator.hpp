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

#ifndef FiducialDecorator_HPP
#define FiducialDecorator_HPP

#include <qa/components/vision/FiducialMarker.hpp>
class DecoratorManager;

namespace decorators {

class FiducialDecorator
{	public:
		virtual void update() = 0;
		const FiducialMarker& getMarker() const { return *mMarker; }
		virtual ~FiducialDecorator(){}

	protected:
		DecoratorManager &mDecoratorManager;
		const FiducialMarker *mMarker;

		FiducialDecorator(
				DecoratorManager &pDecoratorManager,
				const FiducialMarker *pMarker)
			: mDecoratorManager(pDecoratorManager)
			, mMarker(pMarker)
		{}

	private:
		FiducialDecorator();
		FiducialDecorator(const FiducialDecorator& );
		FiducialDecorator& operator=(const FiducialDecorator& );
};

}
#endif
