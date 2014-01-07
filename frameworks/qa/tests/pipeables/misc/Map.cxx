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

#include <qa/pipeables/misc/Map.hpp>
#include <iostream>

class TestObject
{
	public:
	double mData;
};

class Generator: public Pipeable
{
    public:
    	Generator():
    	mObjectCount(10),
    	mObjects(new TestObject[mObjectCount]),
    	mCounter(0)
    	{
    		for (unsigned int i = 0; i < mObjectCount; ++i)
    			mObjects[i].mData = i+1;
    	}
        void run(){
        	std::cout << std::endl;
    		for (unsigned int i = 0; i < mObjectCount; ++i)
    			mObjects[i].mData += 0.1;
    		if(mCounter++ >= 10) stop();
        }
        const size_t *ObjectCount(){return &mObjectCount;}
        const TestObject *const *Objects() const {return &mObjects;}
        
        const size_t mObjectCount;
        TestObject *mObjects;

        unsigned int mCounter;
};

class Function: public Pipeable
{
    public:
    	Function(const TestObject *pInput):mInput(pInput){}
        void run() { std::cout << mInput->mData << " "; }
        const TestObject *mInput;

        const TestObject **InputPtr() {return &mInput;}
};

int main(int , char *[])
{
	Generator tGenerator;
	Map<TestObject> tMap(tGenerator.Objects(), tGenerator.ObjectCount());
	Function tFunction(tMap.Variable());
	tMap.Function(&tFunction);

	tGenerator | tMap | tGenerator;
	tGenerator.start();

	return 0;
}
