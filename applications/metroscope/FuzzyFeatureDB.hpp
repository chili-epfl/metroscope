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

#ifndef FuzzyFeatureDB_HPP
#define FuzzyFeatureDB_HPP

#include <nakai/FeaturePoint.hpp>
#include <boost/unordered_map.hpp>
#include <qa/pipeables/vision/ConnectedComponents.hpp>
#include <qa/utils/Math.hpp>
#include <fstream>
#include <wykobi/wykobi.hpp>

template <bool AFFINE, size_t COMBINE>
class FuzzyFeatureDB
{
public :
	typedef FeaturePoint<AFFINE, COMBINE> FeaturePointType;

	void save(const std::string &pFilename) const;
	void load(const std::string &pFilename);
	size_t update(int pNumFeaturePoints, const FeaturePointType* pFeaturePoints);
	bool contains(const FeaturePointType &pFeature, wykobi::point2d<float> & pOriginalPosition) const;
	static size_t index(const FeaturePointType & tFeaturePoint);

protected:

    struct hash
        : std::unary_function<FeaturePointType, size_t>
    {
        size_t operator()(FeaturePointType const& x) const
        {
        	static const size_t scHashSize = 10000;
        	static const double scQuantizationFactor = 1.20;
			long double tIndex = 0.0;
			long double tK = 1.0;
			for (size_t i = 0; i < x.getInvariantSize(); i++) {
				tIndex += tK * x[i];
				tK *= scQuantizationFactor;
			}

			size_t tRet = ((size_t) (tIndex+0.5)) % scHashSize;

			if (tRet > scHashSize){
				std::cout << "Error: Index > HashSize " << tRet << std::endl;
				return 0;
			}
			return tRet;
        }
    };

    struct equal_to
        : std::binary_function<FeaturePointType, FeaturePointType, bool>
    {
        bool operator()(FeaturePointType const& x,
        		FeaturePointType const& y) const
        {
    		for (size_t i = 0; i < FeaturePointType::scSize;++i)
    			if (x[i] != y[i]) return false;
    		return true;
        }
    };

	boost::unordered_map<FeaturePointType, int, hash, equal_to> mVotes;
};

#endif
