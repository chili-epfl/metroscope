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

#ifndef BattleshipAnalysisSheet_HPP
#define BattleshipAnalysisSheet_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <GL/glut.h>
#include "../FractionsConstants.hpp"

#include <vector>

static const float scPX2MM = 1.72f;

static const float scAxesOnSheet = 0.82f; //How much of the sheet represents the 1 unit of coords


namespace decorators {


class BattleshipAnalysisSheet : public FiducialDecorator
{
	public:
		static decorators::FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		BattleshipAnalysisSheet(
				DecoratorManager &pDecoratorManager,
				const FiducialMarker *pTLMarker,
				const FiducialMarker *pTRMarker,
				const FiducialMarker *pBRMarker,
				const FiducialMarker *pBLMarker,
				float pWorldWidth,
				float pWorldHeight,
				int pTeam, float pOriginCoordsX, float pOriginCoordsY,
				float pOriginMMX, float pOriginMMY);

		virtual ~BattleshipAnalysisSheet();

	protected:
		void update();
		void DisplayGrid();
		void DisplayMoves();
		void DisplayVirus();
		void DisplayOriginMarker(move thismove);
		wykobi::point2d<float> ConvertCoords2MM(wykobi::point2d<float> pCoords);
		wykobi::polygon<float, 2> ConvertPolyCoords2MM(wykobi::polygon<float, 2> pPolyCoords);

		const FiducialMarker *mTRMarker;
		const FiducialMarker *mBRMarker;
		const FiducialMarker *mBLMarker;

		const int mTeam;
		wykobi::point2d<float> mInitialOriginCoords;
		wykobi::point2d<float> mInitialOriginMM;//This goes from 0 to 1, representing in the screen 0 to width/height;

		wykobi::polygon<float, 2> mAreaOfInterest;


	private:
		BattleshipAnalysisSheet();
		BattleshipAnalysisSheet(const BattleshipAnalysisSheet&);
		BattleshipAnalysisSheet& operator=(const BattleshipAnalysisSheet& );
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		wykobi::polygon<float, 2> rotatePolygon(wykobi::polygon<float, 2> polygon, int rotation);
		std::vector<float> polygonToVertices(wykobi::polygon<float, 2> polygon);

};

}
#endif
