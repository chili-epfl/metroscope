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

#ifndef Mirror_HPP
#define Mirror_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <GL/glut.h>

namespace decorators {

class Mirror : public FiducialDecorator
{
	public:
		static decorators::FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		Mirror(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker,
				float pImageWidth, float pImageHeight,
				const IplImage *const *pInputImage);
		virtual ~Mirror();

	protected:
		void update();

		float mImageWidth;
		float mHalfImageHeight;

		wykobi::quadix<float ,2> mSelection;
		std::vector<Pipeable *> mPipeables;
		const GLuint *mTextureId;

	private:
		Mirror();
		Mirror(const Mirror&);
		Mirror& operator=(const Mirror& );
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
};

}
#endif
