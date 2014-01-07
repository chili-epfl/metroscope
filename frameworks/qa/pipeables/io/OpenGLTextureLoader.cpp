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

#include "OpenGLTextureLoader.hpp"

#ifndef GL_BGR
#define GL_BGR GL_BGR_EXT
#endif

//#define DEBUG_OpenGLTextureLoader
#ifdef DEBUG_OpenGLTextureLoader
#include <opencv2/highgui/highgui.hpp>
#endif

OpenGLTextureLoader::OpenGLTextureLoader(const IplImage *const *pInputImage):
mInputImage(pInputImage) {
	glGenTextures(1, &mTextureID);
#ifdef DEBUG_OpenGLTextureLoader
	cvNamedWindow("DEBUG_OpenGLTextureLoader");
#endif
}

OpenGLTextureLoader::~OpenGLTextureLoader(){
	glDeleteTextures(1, &mTextureID);
}

void OpenGLTextureLoader::run(){

#ifdef DEBUG_OpenGLTextureLoader
		cvShowImage("DEBUG_OpenGLTextureLoader", *mInputImage);
		cvWaitKey(10);
#endif

	glEnable(GL_TEXTURE_2D);
    // select our current texture
    glBindTexture( GL_TEXTURE_2D, mTextureID );

    // select modulate to mix texture with color for shading
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                     GL_LINEAR_MIPMAP_NEAREST );

	// when texture area is large, bilinear filter the first mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // build our texture mipmaps
    const IplImage* tInputImage = *mInputImage;
	gluBuild2DMipmaps( GL_TEXTURE_2D,
			GL_RGBA, tInputImage->width, tInputImage->height,
			GL_BGR, GL_UNSIGNED_BYTE, tInputImage->imageData);
	glDisable(GL_TEXTURE_2D);
}
