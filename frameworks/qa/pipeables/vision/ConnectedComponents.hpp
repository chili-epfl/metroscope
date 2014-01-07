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

#ifndef ConnectedComponents_HPP
#define ConnectedComponents_HPP

#include "../Pipeable.hpp"
#include <vector>
#include <opencv2/opencv.hpp>
#include <wykobi/wykobi.hpp>
#include "../../components/geometry/Angle.hpp"
#include "../../components/vision/Color.hpp"

struct Blob
{
	int mArea;
	int mXAccumulator;
	int mYAccumulator;
	int mLUX;
	int mLUY;
	int mRDX;
	int mRDY;
};

class LineBlob
{
	public:
	void set(int pMin, int pMax, int *pBlobId, bool pAttached){
		mMin=pMin;
		mMax=pMax;
		mBlobId=pBlobId;
		mAttached=pAttached;
	}

	int mMin, mMax;
	int *mBlobId;
	bool mAttached;
};

class Component
{
	public:
		Component(){}
		void set(float pX, float pY, int pArea){mX=pX;mY=pY;mArea=pArea;}
		float mX;
		float mY;
		int mArea;
		int mLUX;
		int mLUY;
		int mRDX;
		int mRDY;

		float getCompactness() const{
			if ((mRDX - mLUX)==0 || (mRDX - mLUX)==0){
				return 0.0f;
			}
			else{
				return ((float)mArea) / ((mRDX - mLUX) * (mRDY - mLUY));
			}
		}

		void draw (IplImage *pDstImage, const CvFont * pFont, CvScalar pColour) const{
		
				int tThickNess = std::max(pDstImage->width,pDstImage->height)/500;
				cvCircle(pDstImage,cvPoint((int)mX,(int)mY),1,cvScalar(0,0,255),2);
				cvRectangle(pDstImage,cvPoint((int)mLUX,(int)mLUY),cvPoint((int)mRDX,(int)mRDY),pColour,tThickNess);
				cvLine(pDstImage,cvPoint((int)(mX-3.0f),(int)mY),cvPoint((int)(mX+3.0f),(int)mY),pColour,tThickNess);
				cvLine(pDstImage,cvPoint((int)mX,(int)(mY-3.0f)),cvPoint((int)mX,(int)(mY+3.0f)),pColour,tThickNess);

				std::stringstream ssArea;
				ssArea << mArea;
				cvPutText(pDstImage,ssArea.str().c_str(),cvPoint((int)mLUX,(int)mRDY+10),pFont,pColour);
		}

		friend std::ostream& operator<<(std::ostream & os, const Component & pComponent)
		{
			os << 
				"x=" << pComponent.mX <<
				"y=" << pComponent.mY <<
				"luX=" << pComponent.mLUX <<
				"luY=" << pComponent.mLUY <<
				"rdX=" << pComponent.mRDX <<
				"rdY=" << pComponent.mRDY <<
				"area=" << pComponent.mArea << std::endl;

			return os;
		}
};

template <bool scDraw = false>
class ConnectedComponents: public Pipeable
{
public :
	
	ConnectedComponents(
		int pMinBlobArea,
		int pMaxBlobArea,
		int pMaxNumBlobs,
		const IplImage *const *pBlackWhiteImage,
		bool pIsBlobBlack = true);

	const Component * const* const *GetComponents() const{return &mComponents;}
	const wykobi::point2d<float> * const* GetCenters() const{return &mCenters;}
	const int * const *GetAreas() const{return &mAreas;}
	const int * GetNumComponents() const{return &mNumComponent;}

    const IplImage *const *GetOutputImage() const {return &mOutputImage;}

	~ConnectedComponents();

protected:
	void run();
	void draw();

	bool isSquareToBeSkipped(int pX, int pY, int tSquareSide);
	void SquaricalInspection(const IplImage *const tInputImage, int tSquareSide);
	void ComputeLinesFromSquares(const IplImage *const tInputImage, int tSquareSide);
	void ComputeLinesFromPixels(const IplImage *const tInputImage);
	void ComputeLinesMerge();
	void ComputeLinesRemerge();
	void SortGroupBlobs();
	void StoreBlobs();



	static const int scMinBlobSide;
	int mMinBlobArea;
	int mMaxBlobArea;
	int mMaxNumBlobs;
	const IplImage *const *mBlackWhiteImage;
	int mBlackWhiteImageWidth;
	int mBlackWhiteImageHeight;
	int mBlobColour;
	
	Component **mComponents;
	wykobi::point2d<float> * mCenters;
	int *mAreas;
	int mNumComponent;
	IplImage *mOutputImage;

	int *mBlobIds;
	int mLineBlobCounter;

	LineBlob **mBlobLines;
	int *mBlobLinesCounter;
	std::map<int, Blob> mBlobs;

	int **mFilteredCordinates;
	int *mFilteredCordinatesPositions;

private:
	void checkDataStructure(const IplImage * pImage);

	const double cFontScale;
	CvFont mFont;

};

#endif
