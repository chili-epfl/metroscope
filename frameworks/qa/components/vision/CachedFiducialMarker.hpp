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

#ifndef CachedFiducialMarker_HPP
#define CachedFiducialMarker_HPP

#include "FiducialMarker.hpp"
#include "CraftagRegistrar.hpp"

class CachedFiducialMarker : public FiducialMarker
{
	public:
	static const float scDefaultMinTagTranslation;

	CachedFiducialMarker(const FiducialMarker &pFiducialMarker, const CraftagRegistrar &pCraftagRegistrar = CraftagRegistrar::GetDefault()):
		mFiducialMarker(pFiducialMarker),
		mCraftagRegistrar(pCraftagRegistrar),
		mCachedPresent(pFiducialMarker, mCraftagRegistrar.getFrameId()),
		mCachedCorners(pFiducialMarker, mCraftagRegistrar.getFrameId()),
		mCachedCenter(pFiducialMarker, mCraftagRegistrar.getFrameId()){}

	virtual bool isPresent() const {return mCachedPresent.get(mCraftagRegistrar.getFrameId());}
	virtual wykobi::quadix<float, 2> getCorners() const {return mCachedCorners.get(mCraftagRegistrar.getFrameId());}
	wykobi::point2d<float> getCenter() const {return mCachedCenter.get(mCraftagRegistrar.getFrameId());}

	protected:
	CachedFiducialMarker();
	const FiducialMarker &mFiducialMarker;
	const CraftagRegistrar &mCraftagRegistrar;

	template <class CachedType, CachedType (FiducialMarker::*accessor)() const>
	class CachedMember
	{

	public:
		CachedMember(const FiducialMarker &pMarker, int pCurrentId):mCacheId(pCurrentId-1), mMarker(pMarker){}

		CachedType &get(int pFrameId)
		{
			if (mCacheId != pFrameId)
			{
				mCache = (mMarker.*accessor)();
				mCacheId = pFrameId;
			}
			return mCache;
		}

	private:
		int mCacheId;
		CachedType mCache;
		const FiducialMarker &mMarker;
		CachedMember();
		CachedMember(const CachedMember& );
		CachedMember& operator=(const CachedMember& );
	};

	mutable CachedMember<bool, &FiducialMarker::isPresent> mCachedPresent;
	mutable CachedMember<wykobi::quadix<float, 2>, &FiducialMarker::getCorners> mCachedCorners;
	mutable CachedMember<wykobi::point2d<float>, &FiducialMarker::getCenter> mCachedCenter;
};
#endif
