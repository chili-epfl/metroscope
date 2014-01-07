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

#ifndef Map_HPP
#define Map_HPP

#include <qa/pipeables/Pipeable.hpp>
#include <cstddef>

template <class T>
class Map: public Pipeable
{
    public:

        Map(const T *const *pDomain,
        	const size_t *pSize):
		mFunction(0),
		mVariable(**pDomain),
		mDomain(pDomain),
		mSize(pSize){}

    	const T *Variable() const {return &mVariable;}
    	void Function(Pipeable *pFunction){mFunction=pFunction;}
        
    protected:
        void run()
        {
        	const unsigned int tSize = *mSize;
        	const T *const tDomain = *mDomain;
        	for (unsigned int i = 0; i < tSize; ++i)
        	{
        		mVariable = tDomain[i];
        		mFunction->start();
        	}
        }
        
		Pipeable *mFunction;
    	T mVariable;
    	const T *const *mDomain;
    	const size_t *mSize;
};

#endif
