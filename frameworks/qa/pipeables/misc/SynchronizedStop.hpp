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

#ifndef SynchronizedStop_HPP
#define SynchronizedStop_HPP

#include <qa/pipeables/Pipeable.hpp>

class SynchronizedStop: public Pipeable
{
    public:

        SynchronizedStop(const Pipeable &pPipeline):mPipeline(pPipeline){}

    protected:
        void run() { if (mPipeline.isStopped()) stop(); }
        const Pipeable &mPipeline;

    private:
        SynchronizedStop();
        SynchronizedStop(const SynchronizedStop& pPipeableStub);
        SynchronizedStop& operator=(const SynchronizedStop& pPipeableStub);
};

#endif
