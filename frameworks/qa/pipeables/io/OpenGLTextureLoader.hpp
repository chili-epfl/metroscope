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

#ifndef OpenGLTextureLoader_HPP
#define OpenGLTextureLoader_HPP

#include "../Pipeable.hpp"
#include <GL/glut.h>
#include <opencv2/opencv.hpp>

class OpenGLTextureLoader: public Pipeable
{
public :
	//FIXME? Two assumptions here:
	// - IplImage is BGR and GLTexture is RGBA
	// - widthstep == width (i.e. width is a multiple of 4)
	OpenGLTextureLoader(const IplImage *const *pInputImage);
	virtual ~OpenGLTextureLoader();

	const GLuint *Texture() const { return &mTextureID; }

protected:
	void run();

	const IplImage *const *mInputImage;
	GLuint mTextureID;

	void loadTexture();
};

#endif
