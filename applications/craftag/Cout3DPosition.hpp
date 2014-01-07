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

#ifndef Cout3DPosition_HPP
#define Cout3DPosition_HPP

#include <qa/pipeables/Pipeable.hpp>
#include <qa/components/vision/Craftag.hpp>
#include <iostream>
#include <iomanip>

class Cout3DPosition: public Pipeable
{
    protected:
		void run(){
			
			float tWid=1280.0f;
			float tHei=960.0f;
			float tFx=2136.883057f;
			float tFy=2154.24707f;

			Craftag tTest0(0);
			wykobi::point3d<float> tCenter0 = tTest0.WorldCenter(100.0f, tWid, tHei, tFx, tFy);
			Craftag tTest1(1);
			wykobi::point3d<float> tCenter1 = tTest1.WorldCenter(100.0f, tWid, tHei, tFx, tFy);
			Craftag tTest2(2);
			wykobi::point3d<float> tCenter2 = tTest2.WorldCenter(20.0f, tWid, tHei, tFx, tFy);
			Craftag tTest3(3);
			wykobi::point3d<float> tCenter3 = tTest3.WorldCenter(20.0f, tWid, tHei, tFx, tFy);

			wykobi::quadix<float, 3> tSuperCorners0 = tTest0.WorldCorners2(100.0f, tWid, tHei, tFx, tFy);
			wykobi::point3d<float> tSuperCenter0 = 0.25f*(tSuperCorners0[0]+tSuperCorners0[1]+tSuperCorners0[2]+tSuperCorners0[3]);
			wykobi::quadix<float, 3> tSuperCorners1 = tTest1.WorldCorners2(100.0f, tWid, tHei, tFx, tFy);
			wykobi::point3d<float> tSuperCenter1 = 0.25f*(tSuperCorners1[0]+tSuperCorners1[1]+tSuperCorners1[2]+tSuperCorners1[3]);
			wykobi::quadix<float, 3> tSuperCorners2 = tTest2.WorldCorners2(20.0f, tWid, tHei, tFx, tFy);
			wykobi::point3d<float> tSuperCenter2 = 0.25f*(tSuperCorners2[0]+tSuperCorners2[1]+tSuperCorners2[2]+tSuperCorners2[3]);
			wykobi::quadix<float, 3> tSuperCorners3 = tTest3.WorldCorners2(20.0f, tWid, tHei, tFx, tFy);
			wykobi::point3d<float> tSuperCenter3 = 0.25f*(tSuperCorners3[0]+tSuperCorners3[1]+tSuperCorners3[2]+tSuperCorners3[3]);

			std::cout << tCenter0.x << ", " << tCenter0.y << ", " << tCenter0.z << std::endl;
			return;

			if (tTest0.isPresent() && tTest1.isPresent())
			{
//				std::cout << wykobi::distance(tCenter0,tCenter1) << "\t";
//				std::cout << wykobi::distance(tSuperCenter0,tSuperCenter1) << "\t";
			}
//			else std::cout << "    \t    \t";
			if (tTest2.isPresent() && tTest3.isPresent())
			{
//				std::cout << wykobi::distance(tCenter2,tCenter3) << "\t";
//				std::cout << wykobi::distance(tSuperCenter2,tSuperCenter3) << "\t";
			}
			else std::cout << "    \t    \t";
/*
			if (tTest0.isPresent()) std::cout << tCenter0.x << "," << tCenter0.y << "," << tCenter0.z << "\t";
			else std::cout << "               \t";
			if (tTest1.isPresent()) std::cout << tCenter1.x << "," << tCenter1.y << "," << tCenter1.z << "\t";
			else std::cout << "               \t";
			if (tTest2.isPresent()) std::cout << tCenter2.x << "," << tCenter2.y << "," << tCenter2.z << "\t";
			else std::cout << "               \t";
			if (tTest3.isPresent()) std::cout << tCenter3.x << "," << tCenter3.y << "," << tCenter3.z << "\t";
			else std::cout << "               \t";
*/
//			std::cout << std::endl;
		}
};

#endif
