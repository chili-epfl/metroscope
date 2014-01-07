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

#include "Angle.hpp"


template <typename T>
Angle<T>::Angle(){
}

template <typename T>
Angle<T>::Angle(const T& pAngle){
	mAngle=in2PI(pAngle);
}

template <typename T> Angle<T>::Angle(const Angle & pAngle){
	mAngle=pAngle.getDegrees();
}

template <typename T>
T Angle<T>::in2PI(const T & pTheta) const{
	T tTheta=pTheta;
	while(tTheta<0){
		tTheta+=(T)360;
	}
	while(tTheta>=360){
		tTheta-=(T)360;
	}
	return tTheta;
}

template <typename T>
const Angle<T> Angle<T>::sc0(0);
template <typename T>
const Angle<T> Angle<T>::sc45(45);
template <typename T>
const Angle<T> Angle<T>::sc90(90);
template <typename T>
const Angle<T> Angle<T>::sc135(135);
template <typename T>
const Angle<T> Angle<T>::sc180(180);
template <typename T>
const Angle<T> Angle<T>::sc270(270);
template <typename T>
const float Angle<T>::scPI(3.141592f);

template class Angle<int>;
template class Angle<float>;
template class Angle<double>;
