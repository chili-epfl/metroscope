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

#ifndef _DBScanCluster_H_
#define _DBScanCluster_H_

#include "DBScanPoint.hpp"
#include "../vision/Color.hpp"
#include "../geometry//Angle.hpp"
#include <wykobi/wykobi.hpp>
#include <opencv2/opencv.hpp>

template <class T, int D>
class DBScanCluster{

	public:
		DBScanCluster();
		DBScanCluster(int pID);
		~DBScanCluster();

		void addElement(const DBScanPoint<T,D> & pPoint);
		void setID(const int & pID);
		int getID() const;
		void reset();
		void ComputeMetrics();

		const DBScanPoint<float,D> & getCentroid() const;
		const DBScanPoint<T,D> & getMinVertex() const;
		const DBScanPoint<T,D> & getMaxVertex() const;

		int getCardinality() const;

		const wykobi::quadix<T,2> & getBoundingBox() const;
		const wykobi::quadix<int,2> & MBR();
		const wykobi::quadix<int,2> & getMBR() const;
		void MBRApproxEllipse();
		float getRectangularity() const;
		const Angle<float> & getEllipseAngle() const;
		float getEllipseWidth() const;
		float getEllipseHeight() const;

		virtual void draw (
            IplImage *pDstImage,
            const wykobi::point2d<int> &,
            const Angle<float> &,
            float pScale,
            const Color & pColor
        ) const;

		const DBScanCluster<T,D> & operator =(const DBScanCluster<T,D> & pDBScanCluster);
		const DBScanPoint<T,D> &operator [](int pElement) const;
		DBScanPoint<T,D> & operator [](int pElement);

	private:

		static const int scMaxNumPoints = 256*16;

		DBScanPoint<T,D> **mDBScanPoints;
		DBScanPoint<float,D> mCentroid;
		DBScanPoint<T,D> mMinVertex;
		DBScanPoint<T,D> mMaxVertex;

		wykobi::quadix<T, 2> mBoundingBox;
		wykobi::quadix<int, 2> mMBR;
		float mRectangularity;

		int mID;
		int mCardinality;
		Angle<float> mAngle;
		float mEllipseWidth,mEllipseHeight;
};

#endif
