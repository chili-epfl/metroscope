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

#ifndef Gray2Color_HPP
#define Gray2Color_HPP

#include <qa/pipeables/Pipeable.hpp>
#include <opencv2/opencv.hpp>

class Gray2Color: public Pipeable
{
    public:

        Gray2Color(
            const IplImage *const *pInputImage);
		virtual ~Gray2Color();

        Gray2Color *ConversionCode(int pNewValue) {mConversionCode = pNewValue; return this;}

        const IplImage *const *GetOutputImage() const {return &mOutputImage;}

    protected:
        void run();

        const IplImage *const *mInputImage;
        int mConversionCode;
        IplImage * mOutputImage;

    private:
        Gray2Color();
        Gray2Color(const Gray2Color& pGray2Color);
        Gray2Color& operator=(const Gray2Color& pGray2Color);
};

#endif
