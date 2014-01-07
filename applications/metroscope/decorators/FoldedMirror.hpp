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

#ifndef FoldedMirror_HPP
#define FoldedMirror_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include "QuadModel.hpp"
#include "AxisModel.hpp"

namespace decorators {

class FoldedMirror : public FiducialDecorator
{
	public:
		static FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		FoldedMirror(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const QuadModel &pQuadModel, const AxisModel &pOrgAxisModel, const AxisModel &pDstAxisModel, const FiducialMarker &pVerso);

	protected:
		void update();

		const QuadModel &mQuadModel;
		const AxisModel &mOrgAxisModel;
		const AxisModel &mDstAxisModel;
		const FiducialMarker &mVerso;

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
};

}
#endif
