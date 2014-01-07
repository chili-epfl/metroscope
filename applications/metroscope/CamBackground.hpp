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

#include <qa/pipeables/Pipeable.hpp>
#include <qa/pipeables/io/OpenGLTextureLoader.hpp>

class CamBackground : public Pipeable
{
	public:
		CamBackground(
			OpenGl2DDisplay& pDisplay,
			const IplImage *const *pInputImage )
			:mDisplay(pDisplay)
			,mInputImage(pInputImage)
			,mOpenGLTextureLoader(pInputImage)
			{}

	protected:
		void run() {
			mOpenGLTextureLoader.start();
			mDisplay.RenderTexture(
					*mOpenGLTextureLoader.Texture(),
					0.0f, 0.0f,
					(float) (*mInputImage)->width, 0.0f,
					(float) (*mInputImage)->width, (float) (*mInputImage)->height,
					0.0f, (float) (*mInputImage)->height);
		}

		OpenGl2DDisplay mDisplay;
		const IplImage *const *mInputImage;
		OpenGLTextureLoader mOpenGLTextureLoader;
};
