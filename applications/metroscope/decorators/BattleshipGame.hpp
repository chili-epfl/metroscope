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

#ifndef BATTLESHIPGAME_HPP_
#define BATTLESHIPGAME_HPP_

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <qa/components/misc/NetworkedStateManager.hpp>
#include <wykobi/wykobi_utilities.hpp>
#include <string>
#include <iostream>
#include <sstream>

#include "Flipper.hpp"
#include "PolyModel.hpp"
#include "RotationShoot.hpp"
#include "LinearShoot.hpp"
#include "../FractionsConstants.hpp"


//	static const char * const scMainMenu = "MainMenu";
//	static const char * const scGameNewTurn = "GameNewTurn";
//	static const char * const scGameAnalysis = "GameAnalysis";
//	static const char * const scGameShoot = "GameShoot";
//	static const char * const scGameResolve = "GameResolve";
//	static const char * const scGameWin = "GameWin";
//	static const char * const scGameOver = "GameOver";
	static const char * const scGameInitial = "Initial";
	static const char * const scGameNewTurn = "NewTurn";
	static const char * const scGameShoot = "Shoot";
	static const char * const scGameResolve = "Resolution";
	static const char * const scGameOver = "EndGame";

	static const float scMM2DisplayMult = 25.0f;
	static const float scBoard2MMMult = 17.0f;


namespace decorators{



class BattleshipGame : public FiducialDecorator
{
	public:
		static FiducialDecorator *create (libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		BattleshipGame (DecoratorManager &pDecoratorManager, FiducialMarker *pMarker,
				int pNumPolygons, PolyModel ** pPolyModels,
				RotationShoot *pRotation, LinearShoot *pLinearX, LinearShoot *pLinearY, FiducialMarker *pMarkerShoot);
		~BattleshipGame();


	protected:
		void update();
		void DisplayPhase();
		void GreyOutScreen();
//		void UpdateDeviceStatus();

		bool isPolygonPresent();
		void DisplayPolygonAxes();
		wykobi::polygon<float,2> getFirstPolygon();
		void DisplayFirstPolygon();
		void DisplayTranslationArrow(float pX, float pY);
		void DisplayRotationAngle(float pX, float pY, int pRotation);
		void DisplayTransformedPolygon(float pX, float pY, int pRotation);
		std::vector<float> polygonToVertices(wykobi::polygon<float, 2> polygon);



	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		const int mNumPolygons;
		PolyModel **mPolyModels;
		RotationShoot *mRotation;
		LinearShoot *mLinearX;
		LinearShoot *mLinearY;
		FiducialMarker *mMarkerShoot;

};

}

#endif /* BattleshipGame_HPP_ */

