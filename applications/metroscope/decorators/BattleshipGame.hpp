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
#include <string>
#include <iostream>
#include <sstream>

#include "Flipper.hpp"
#include "PolyModel.hpp"
#include "../FractionsConstants.hpp"


	static const char * const scMainMenu = "MainMenu";
	static const char * const scGameNewTurn = "GameNewTurn";
	static const char * const scGameAnalysis = "GameAnalysis";
	static const char * const scGameShoot = "GameShoot";
	static const char * const scGameResolve = "GameResolve";
	static const char * const scGameWin = "GameWin";
	static const char * const scGameOver = "GameOver";



namespace decorators{



class BattleshipGame : public FiducialDecorator
{
	public:
		static FiducialDecorator *create (libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);

		BattleshipGame (DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, int pNumPolygons, PolyModel ** pPolyModels);
		~BattleshipGame();


	protected:
		void update();
		void DisplayPhase();
		void GreyOutScreen();
//		void UpdateDeviceStatus();

	private:
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;

		const int mNumPolygons;
		PolyModel **mPolyModels;


};

}

#endif /* BattleshipGame_HPP_ */

