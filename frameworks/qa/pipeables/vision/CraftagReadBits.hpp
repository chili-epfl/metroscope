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

#ifndef CraftagReadBits_HPP
#define CraftagReadBits_HPP

#include "../Pipeable.hpp"
#include <wykobi/wykobi.hpp>
#include <opencv2/opencv.hpp>
#include "AdaptiveThreshold.hpp"

class CraftagReadBits: public Pipeable
{
    public:
        CraftagReadBits(const IplImage *const *pInputImage);
        const unsigned char *const *GetBits() const {return &mMatrix;}

        virtual ~CraftagReadBits();

    protected:
        void run();

		AdaptiveThreshold mAdaptiveThreshold;

		unsigned char *mMatrix;

    private:
		CraftagReadBits();
		CraftagReadBits(const CraftagReadBits&);
		CraftagReadBits& operator=(const CraftagReadBits&);
};

#endif
