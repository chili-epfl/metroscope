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

#ifndef CraftagDecode_HPP
#define CraftagDecode_HPP

#include "../Pipeable.hpp"
#include "../../components/vision/TagTranscoder.hpp"

class CraftagDecode: public Pipeable
{
    public:
        CraftagDecode(
        		const unsigned char *const *pMatrix,
    			TagTranscoder &pTagTranscoder);

        //-1 if no valid tag has been decoded
        const int *GetDecodedTag() const {return &mDecodedTag;}

        const int *GetOrientation() const {return &mOrientation;}

        virtual ~CraftagDecode();

    protected:
        void run();

		const unsigned char *const *mMatrix;
		unsigned char *mMatrix90;
		unsigned char *mMatrix180;
		unsigned char *mMatrix270;

        TagTranscoder &mTagTranscoder;

        int mDecodedTag;
        int mOrientation;

    private:
        CraftagDecode();
        CraftagDecode(const CraftagDecode&);
        CraftagDecode& operator=(const CraftagDecode&);
};

#endif
