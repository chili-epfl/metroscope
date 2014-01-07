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

#include <iostream>
#include <qa/utils/Tests.hpp>
#include <qa/utils/Math.hpp>

int main()
{
    Tests::TestSection("Math::linearInterpolate");
    const int size = 3;
    double sequence[size] = {3,5,4};
    Tests::verifyEqual(Math::linearInterpolate(0,sequence,size),3.0);
    Tests::verifyEqual(Math::linearInterpolate(0.5,sequence,size),5.0);
    Tests::verifyEqual(Math::linearInterpolate(1,sequence,size),4.0);
    Tests::verifyEqual(Math::linearInterpolate(0.75,sequence,size),4.5);
    Tests::verifyEqual(Math::linearInterpolate(-1,sequence,size),0.0);
    Tests::verifyEqual(Math::linearInterpolate(1.5,sequence,size),0.0);

    Tests::TestSection("Math::Factorial");
    Tests::verifyEqual((int) Math::Factorial<0>::value, 1);
    Tests::verifyEqual((int) Math::Factorial<12>::value, 479001600);

    Tests::TestSection("Math::Binomial");
    //Tests::verifyEqual((int) Math::Binomial<0,0>::value, 1);
    Tests::verifyEqual((int) Math::Binomial<7,0>::value, 1);
    Tests::verifyEqual((int) Math::Binomial<7,7>::value, 1);
    Tests::verifyEqual((int) Math::Binomial<7,6>::value, 7);
    Tests::verifyEqual((int) Math::Binomial<7,1>::value, 7);
    Tests::verifyEqual((int) Math::Binomial<7,4>::value, 35);
    //Tests::verifyEqual((int) Math::Binomial<4,7>::value, 479001600);
}
