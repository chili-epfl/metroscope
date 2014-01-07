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

#ifndef CraftagUpdate_HPP
#define CraftagUpdate_HPP

#include "../Pipeable.hpp"
#include "../../components/vision/CraftagRegistrar.hpp"

class CraftagUpdate: public Pipeable
{
	public:
		CraftagUpdate(
				const IplImage *const *pInputImage,
				CraftagRegistrar &pCraftagRegistrar = CraftagRegistrar::GetDefault());
		virtual ~CraftagUpdate();

		const IplImage *const *GetOutputImage() const {return mEdgeImage;}

	protected:
		void run() {
			mCraftagRegistrar.reset();
			mPipeline->start();
		}

		Pipeable *mPipeline;
		std::vector<Pipeable *> mPipeables;

		CraftagRegistrar &mCraftagRegistrar;

		const IplImage *const *mEdgeImage;

	private:
		CraftagUpdate();
		CraftagUpdate(const CraftagUpdate&);
		CraftagUpdate& operator=(const CraftagUpdate&);
};

#endif
