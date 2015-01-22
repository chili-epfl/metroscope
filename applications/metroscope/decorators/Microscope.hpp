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

#ifndef Microscope_HPP
#define Microscope_HPP

#include <qa/pipeables/misc/DecoratorManager.hpp>
#include <GL/glut.h>

#include <vector>

namespace decorators {


class Microscope : public FiducialDecorator
{
	public:
		static decorators::FiducialDecorator *create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager);
		Microscope(
				DecoratorManager &pDecoratorManager,
				const FiducialMarker *pTLMarker,
				const FiducialMarker *pTRMarker,
				const FiducialMarker *pBRMarker,
				const FiducialMarker *pBLMarker,
				float pWorldWidth,
				float pWorldHeight,
				const IplImage *const *pInputImage);
		void AddAxis(const wykobi::line<float, 2>& pAxis);

		const IplImage *const *GetExtractedImage() {return mExtractedImage;}

		virtual ~Microscope();

	protected:
		void update();
		const FiducialMarker *mTRMarker;
		const FiducialMarker *mBRMarker;
		const FiducialMarker *mBLMarker;

		wykobi::polygon<float, 2> mAreaOfInterest;
		std::vector<wykobi::line<float, 2> > mAxes;
		wykobi::rectangle<float> mBoxOfInterest;
		wykobi::polygon<float, 2> mNormalizedAreaOfInterest;
		std::vector<wykobi::polygon<float, 2> > mMirroredAreasOfInterest;
		wykobi::quadix<float, 2> mSelection;
		wykobi::quadix<float, 2> mLocalSelection;
		float *mHomography;
		std::vector<Pipeable *> mPipeables;
		const GLuint *mTextureId;
		const IplImage *const *mExtractedImage;

	private:
		Microscope();
		Microscope(const Microscope&);
		Microscope& operator=(const Microscope& );
		static const std::string scDecoratorName;
		static const DecoratorManager::Registerer mRegisterer;
};

}
#endif
