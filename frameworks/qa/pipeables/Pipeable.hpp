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

        /// call run(), fetch the next Pipeable, and repeat until next is null
        void start();

        /// removes the next Pipeable, effectively stopping the pipeline
        void stop() {mNext = 0;}

        /// check whether the next Pipeable is null, i.e. the pipeline is stopped
		bool isStopped() const {return mNext == 0;}

        /// Like start(), but print a sequence number before and after each call to run
        /// Useful to know which Pipeable crashes
        void startDebug();

        /// Like start(), but prints the time (in ms) spent in run() for each Pipeable
        void startProfile();

        /// Like start(), but asynchronous, i.e. returns immediately and runs the pipeline in another thread
        void startNoWait();

        /// Brings the asynchronous thread back into the main thread
		void join();

    protected:

        /// The mandatory method to implement by Pipeable subclasses
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
