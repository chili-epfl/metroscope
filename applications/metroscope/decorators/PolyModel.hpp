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

#ifndef PolyModel_HPP
#define PolyModel_HPP

#include <vector>
#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <wykobi/wykobi.hpp>

namespace decorators {

class PolyModel : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		PolyModel(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, int pNumVertices,
				std::vector<float> pCoords, std::vector<float> pOrigin);
		const wykobi::polygon<float, 2> &getPolygon() const {return mPolygon;}
		int getSize() const {return mNumVertices;}
		const wykobi::point2d<float> &getOrigin() const {return mOrigin;}

	protected:
		void update(){}
		wykobi::polygon<float, 2> mPolygon;
		wykobi::point2d<float> mOrigin;
		int mNumVertices;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
};

}
#endif
