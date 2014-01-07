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

#ifndef CraftagRegister_HPP
#define CraftagRegister_HPP

#include "../Pipeable.hpp"
#include "../../components/vision/CraftagRegistrar.hpp"
#include <wykobi/wykobi.hpp>

class CraftagRegister: public Pipeable
{
    public:
        CraftagRegister(
        		const int *pDecodedTag,
    			const wykobi::quadix2d *pCorners,
    			const int *pRotation,
        		CraftagRegistrar &pCraftagRegistrar);

        virtual ~CraftagRegister();

    protected:
        void run();

        const int *mDecodedTag;
        const wykobi::quadix2d *mCorners;
        const int *mRotation;
        CraftagRegistrar &mCraftagRegistrar;

        CvPoint2D32f *mCornersToStore;

    private:
        CraftagRegister();
        CraftagRegister(const CraftagRegister&);
        CraftagRegister& operator=(const CraftagRegister&);
};

#endif
