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

#ifndef CraftagReplay_HPP
#define CraftagReplay_HPP

#include <qa/pipeables/Pipeable.hpp>
#include <qa/components/vision/CraftagRegistrar.hpp>
#include <libconfig.h++>

class CraftagReplay: public Pipeable
{
    public:

        CraftagReplay(
        		const char *pFilename,
        		CraftagRegistrar &pCraftagRegistrar = CraftagRegistrar::GetDefault());
		virtual ~CraftagReplay();

		const long *Timestamp() {return &mNow;}

    protected:
        void run();

        CraftagRegistrar &mCraftagRegistrar;
        libconfig::Config mLogfile;
        int mLogIndex;
        long mTimeShift;
        long mNow;

    private:
        CraftagReplay();
        CraftagReplay(const CraftagReplay& pPipeableStub);
        CraftagReplay& operator=(const CraftagReplay& pPipeableStub);
};

#endif
