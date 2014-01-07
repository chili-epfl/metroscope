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

#ifndef SlideShow_HPP
#define SlideShow_HPP

#include <qa/pipeables/Pipeable.hpp>
#include <libconfig.h++>
#include <GL/glut.h>
#include <map>
#include <string>

class SlideShow : public Pipeable
{
	public:
		SlideShow(
			OpenGl2DDisplay& pDisplay,
			CraftagRegistrar &pCraftagRegistrar,
			const char* pConfigFileName )
			:mDisplay(pDisplay)
			,mCraftagRegistrar(pCraftagRegistrar)
			,mSlidePaths()
			,mIsShowingSlide(false)
			,mLastTag(-1)
			,mCurrentTexture(0)
			{
				libconfig::Config tConfig;
            	try {
            		tConfig.readFile( pConfigFileName );
            	} catch(libconfig::FileIOException &) {
            		std::cerr << "SlideShow configuration: FileIOException with "<< pConfigFileName << std::endl;
            	} catch(libconfig::ParseException &e) {
            		std::cerr << pConfigFileName << ", line "<< e.getLine() << ": " << e.getError() << std::endl;
            	}

            	try {
            		libconfig::Setting &tSlides = tConfig.lookup("slides");
					mNSlides = (std::size_t) tSlides.getLength();
					for (std::size_t i=0; i<mNSlides && i<scNTags; ++i)
					{
						std::string tPath = tSlides[i];
						mSlidePaths[i] = tPath;
					}
            	}
            	catch (libconfig::SettingNotFoundException &e) {
					std::cerr << "Setting not found: " << e.getPath() << std::endl;
				}
			}

	protected:
		void run() {
			for(std::size_t i = 1; i<scNTags && i < mNSlides; ++i)
			{
				if (Craftag(i, 3, mCraftagRegistrar).isPresent())
				{
					if (!mSlidePaths[i-1].empty())
					{
						if (mLastTag != i)
						{
							mDisplay.ReleaseTexture(mCurrentTexture);
							mCurrentTexture =
								mDisplay.LoadTexture(mSlidePaths[i-1].c_str());
							mLastTag = i;
						}
						mIsShowingSlide = true;
					}
					else
					{
						mIsShowingSlide = false;
					}
				}
			}
			if (mIsShowingSlide)
			{
				static const int scWidth =
					(float) mDisplay.GetWidth();
				static const int scHeight =
					(float) mDisplay.GetHeight();
				mDisplay.RenderTexture(
						mCurrentTexture,
						   0.0f, 0.0f,
						scWidth, 0.0f,
						scWidth, scHeight,
						   0.0f, scHeight);
			}
		}

		OpenGl2DDisplay mDisplay;
		CraftagRegistrar &mCraftagRegistrar;
		static const std::size_t scNTags = 1024;
		std::size_t mNSlides;
		std::string mSlidePaths[scNTags];
		bool mIsShowingSlide;
		std::size_t mLastTag;
		GLuint mCurrentTexture;
};

#endif
