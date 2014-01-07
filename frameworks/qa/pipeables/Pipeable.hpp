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

#ifndef PIPEABLE_HPP
#define PIPEABLE_HPP

#ifdef WIN32
#include <boost/thread/thread.hpp>
#else //that's sad, but boost::thread >= 1.42 and flycapture 2 don't play along well
#include <pthread.h>
#endif

class Pipeable
{
    public:
        Pipeable():mNext(0){}
		virtual ~Pipeable(){}

        Pipeable &operator|(Pipeable &pNext){ mNext = &pNext; return *mNext; }

        void start();
        void stop() {mNext = 0;}
		bool isStopped() const {return mNext == 0;}
        void startDebug();
        void startProfile();
        void startNoWait();
		void join();

    protected:

        virtual void run() = 0;

        Pipeable *mNext;
#ifdef WIN32
		boost::thread mThread;
#else
		pthread_t mThread;
        static void *startThread(void *pPipeable){
        	reinterpret_cast<Pipeable *>(pPipeable)->start();
        	return 0;
        }
#endif
};

#endif
