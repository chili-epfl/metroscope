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

#include "ConnectedComponents.hpp"
#include "../../utils/Image.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <map>
#include <ctime>
//#include <opencv2/highgui/highgui.hpp>

template <bool scDraw>
ConnectedComponents<scDraw>::ConnectedComponents(
	int pMinBlobArea,
	int pMaxBlobArea,
	int pMaxNumBlobs,
	const IplImage *const *pBlackWhiteImage,
	bool pIsBlobBlack):
mMinBlobArea(pMinBlobArea),
mMaxBlobArea(pMaxBlobArea),
mMaxNumBlobs(pMaxNumBlobs),
mBlackWhiteImage(pBlackWhiteImage),
mNumComponent(0),
cFontScale(0.5)
{

	const IplImage *tBlackWhiteImage = *mBlackWhiteImage;
	
	mBlackWhiteImageWidth = std::max((int)tBlackWhiteImage->width,scMinBlobSide);
	mBlackWhiteImageHeight = std::max((int)tBlackWhiteImage->height,scMinBlobSide);
	mBlackWhiteImageWidth += mBlackWhiteImageWidth%2;
	mBlackWhiteImageHeight += mBlackWhiteImageHeight%2;
	mBlobColour = (pIsBlobBlack) ? 0 : 255;

	mCenters = new wykobi::point2d<float>[mMaxNumBlobs];
	mAreas = new int[mMaxNumBlobs];
	mBlobIds = new int [mBlackWhiteImageHeight * mBlackWhiteImageWidth / 2];
	mBlobLinesCounter = new int [mBlackWhiteImageHeight];
	mFilteredCordinatesPositions = new int[mBlackWhiteImageHeight];

	mComponents = new Component* [mMaxNumBlobs];
	for (int i=0; i<mMaxNumBlobs; i++)
	{
		mComponents[i] = new Component();
	}

	mBlobLines = new LineBlob* [mBlackWhiteImageHeight];
	for (int i=0; i<mBlackWhiteImageHeight; i++){
		mBlobLines[i] = new LineBlob[mBlackWhiteImageWidth/2];
	}

	mFilteredCordinates = new int * [mBlackWhiteImageHeight];
	for (int i=0; i<mBlackWhiteImageHeight; i++){
		mFilteredCordinates[i] = new int[(int)(0.5f+(float)mBlackWhiteImageWidth/(float)scMinBlobSide)];
	}

	cvInitFont(&mFont,CV_FONT_HERSHEY_COMPLEX_SMALL, cFontScale,cFontScale,0,1);

	if (scDraw) {
		mOutputImage = cvCreateImage(cvGetSize(*mBlackWhiteImage), (*mBlackWhiteImage)->depth, 1);
//		cvNamedWindow("ConnectedComponents");
	}

}

template <bool scDraw>
ConnectedComponents<scDraw>::~ConnectedComponents()
{
	delete [] mCenters;
	delete [] mAreas;
	delete [] mBlobIds;
	delete [] mBlobLinesCounter;
	delete [] mFilteredCordinatesPositions;

	for (int i=0; i<mMaxNumBlobs; i++)
	{
		delete mComponents[i];
	}
	delete [] mComponents;

	for (int i=0; i<mBlackWhiteImageHeight; i++){
		delete [] mBlobLines[i];
	}
	delete [] mBlobLines;

	for (int i=0; i<mBlackWhiteImageHeight; i++){
		delete [] mFilteredCordinates[i];
	}
	delete [] mFilteredCordinates;

	if (scDraw)  cvReleaseImage(&mOutputImage);
}

template <bool scDraw>
bool ConnectedComponents<scDraw>::isSquareToBeSkipped(int pX, int pY, int tSquareSide){
	uchar tPixValue;
	int tX;
	int tY=pY;
	const IplImage *tInputImage = *mBlackWhiteImage;
	for (tX=pX; tX<pX+tSquareSide; ++tX){
		tPixValue = ((uchar *)(tInputImage->imageData + tY*tInputImage->widthStep))[tX];
		if (tPixValue == mBlobColour) return true;
	}
	for (tY=pY+1; tY<pY+tSquareSide; ++tY){
		tPixValue = ((uchar *)(tInputImage->imageData + tY*tInputImage->widthStep))[tX];
		if (tPixValue == mBlobColour) return true;
	}
	for (tX=tX-1; tX>=pX; --tX){
		tPixValue = ((uchar *)(tInputImage->imageData + tY*tInputImage->widthStep))[tX];
		if (tPixValue == mBlobColour) return true;
	}
	for (tY=tY-1; tY>pY; --tY){
		tPixValue = ((uchar *)(tInputImage->imageData + tY*tInputImage->widthStep))[tX];
		if (tPixValue == mBlobColour) return true;
	}
	return false;
}

template <bool scDraw>
void ConnectedComponents<scDraw>::SquaricalInspection(const IplImage *const tInputImage, int tSquareSide){
	//Extracting Blob Lines
	bool tPrevSkipped=true;
	for (int tY=0; tY<mBlackWhiteImageHeight-tSquareSide; tY+=tSquareSide){
		mFilteredCordinatesPositions[tY]=0;
		tPrevSkipped=true;
		for (int tX=0; tX<mBlackWhiteImageWidth-tSquareSide; tX+=tSquareSide){
			if (isSquareToBeSkipped(tX,tY,tSquareSide)) {
				if (tPrevSkipped){
					mFilteredCordinates[tY][mFilteredCordinatesPositions[tY]]=tX;
					mFilteredCordinatesPositions[tY]+=1;
					tPrevSkipped=false;
				}
			}
			else{
				if (!tPrevSkipped){
					mFilteredCordinates[tY][mFilteredCordinatesPositions[tY]]=tX;
					mFilteredCordinatesPositions[tY]+=1;
					tPrevSkipped=true;
				}
			}
		}
		if (!tPrevSkipped){
			mFilteredCordinates[tY][mFilteredCordinatesPositions[tY]]=tInputImage->width-tSquareSide;
			mFilteredCordinatesPositions[tY]+=1;
			tPrevSkipped=true;
		}
	}
}

template <bool scDraw>
void ConnectedComponents<scDraw>::ComputeLinesFromSquares(const IplImage *const tInputImage, int tSquareSide){
	uchar tPixValue;
	int tStart;
	int tStop;
	mLineBlobCounter = 0;
	for (int tBY=0; tBY<mBlackWhiteImageHeight-tSquareSide; tBY+=tSquareSide) {
		for (int tBX=0; tBX<mFilteredCordinatesPositions[tBY]; tBX+=2) {
			for(int tY = tBY; tY < tBY+tSquareSide; ++tY) {
				for(int tX = mFilteredCordinates[tBY][tBX]; tX < mFilteredCordinates[tBY][tBX+1]; ++tX) {
					tPixValue = ((uchar *)(tInputImage->imageData + tY*tInputImage->widthStep))[tX];
					if(tPixValue == mBlobColour) {
						tStart = tX;
						for(;tX<mBlackWhiteImageWidth; ++tX){
							tPixValue = ((uchar *)(tInputImage->imageData + tY*tInputImage->widthStep))[tX];
							if (tPixValue != mBlobColour) break;
						}
						tStop = tX;
						mBlobIds[mLineBlobCounter]=mLineBlobCounter;
						mBlobLines[tY][mBlobLinesCounter[tY]].set(tStart, tStop, &mBlobIds[mLineBlobCounter], false);
						mBlobLinesCounter[tY]++;
						mLineBlobCounter++;
					}
				}
			}
		}
	}
}

template <bool scDraw>
void ConnectedComponents<scDraw>::ComputeLinesFromPixels(const IplImage *const tInputImage){
	uchar tPixValue;
	int tStart;
	int tStop;
	mLineBlobCounter = 0;
	for(int tY = 0; tY < mBlackWhiteImageHeight; ++tY) {
		for(int tX = 0; tX < mBlackWhiteImageWidth; ++tX) {
			tPixValue = ((uchar *)(tInputImage->imageData + tY*tInputImage->widthStep))[tX];
			if(tPixValue == mBlobColour) {
				tStart = tX;
				for(tX++;tX<mBlackWhiteImageWidth; ++tX){
					tPixValue = ((uchar *)(tInputImage->imageData + tY*tInputImage->widthStep))[tX];
					if (tPixValue != mBlobColour) break;
				}
				tStop = tX;
				mBlobIds[mLineBlobCounter]=mLineBlobCounter;
				mBlobLines[tY][mBlobLinesCounter[tY]].set(tStart, tStop, &mBlobIds[mLineBlobCounter], false);
				mBlobLinesCounter[tY]++;
				mLineBlobCounter++;
			}
		}
	}
}

template <bool scDraw>
void ConnectedComponents<scDraw>::ComputeLinesMerge(){
	for(int tY = 0; tY < mBlackWhiteImageHeight-1; ++tY){
		for(int tEntryLineMedium = 0; tEntryLineMedium < mBlobLinesCounter[tY]; ++tEntryLineMedium){
			for(int tEntryLineDown = 0; tEntryLineDown < mBlobLinesCounter[tY+1]; ++tEntryLineDown){
				if(!((mBlobLines[tY][tEntryLineMedium].mMax < mBlobLines[tY+1][tEntryLineDown].mMin) || (mBlobLines[tY][tEntryLineMedium].mMin > mBlobLines[tY+1][tEntryLineDown].mMax))){
					if(mBlobLines[tY+1][tEntryLineDown].mAttached){
						*mBlobLines[tY][tEntryLineMedium].mBlobId = *mBlobLines[tY+1][tEntryLineDown].mBlobId;
						mBlobLines[tY][tEntryLineMedium].mAttached = true;
					}
					else{
						mBlobLines[tY+1][tEntryLineDown].mBlobId = mBlobLines[tY][tEntryLineMedium].mBlobId;
						mBlobLines[tY+1][tEntryLineDown].mAttached = true;
					}
				}
			}
		}
	}
}

template <bool scDraw>
void ConnectedComponents<scDraw>::ComputeLinesRemerge(){

	for(int tY = mBlackWhiteImageHeight-1; tY >0; --tY){
		for(int tEntryLineMedium = 0; tEntryLineMedium < mBlobLinesCounter[tY]; ++tEntryLineMedium){
			for(int tEntryLineUp = 0; tEntryLineUp < mBlobLinesCounter[tY-1]; ++tEntryLineUp){
				if(!((mBlobLines[tY][tEntryLineMedium].mMax < mBlobLines[tY-1][tEntryLineUp].mMin) || (mBlobLines[tY][tEntryLineMedium].mMin > mBlobLines[tY-1][tEntryLineUp].mMax))){
					if(*mBlobLines[tY-1][tEntryLineUp].mBlobId != *mBlobLines[tY][tEntryLineMedium].mBlobId){
						*mBlobLines[tY-1][tEntryLineUp].mBlobId = *mBlobLines[tY][tEntryLineMedium].mBlobId;
					}
				}
			}
		}
	}
}

template <bool scDraw>
void ConnectedComponents<scDraw>::SortGroupBlobs(){
	int tBlobId;
	int tLineBlobLength=0;
	for(int tY = 0; tY < mBlackWhiteImageHeight; ++tY) {
		for(int tEntry = 0; tEntry < mBlobLinesCounter[tY]; ++tEntry) {
			const LineBlob tBlobLinestYtEntry = mBlobLines[tY][tEntry];
			tBlobId = *(tBlobLinestYtEntry.mBlobId);
			tLineBlobLength = tBlobLinestYtEntry.mMax - tBlobLinestYtEntry.mMin;
			if(mBlobs.find(tBlobId) == mBlobs.end()) {
				Blob blobData = {	tLineBlobLength,
									(tBlobLinestYtEntry.mMax + tBlobLinestYtEntry.mMin - 1 ) * tLineBlobLength,
									tY*tLineBlobLength,
									tBlobLinestYtEntry.mMin,
									tY,
									tBlobLinestYtEntry.mMax,
									tY};
				mBlobs[*tBlobLinestYtEntry.mBlobId] = blobData;
			}
			else{
				Blob &tBlobstBlobId = mBlobs[tBlobId];
				tBlobstBlobId.mXAccumulator += (tBlobLinestYtEntry.mMax + tBlobLinestYtEntry.mMin - 1) * tLineBlobLength;
				tBlobstBlobId.mYAccumulator += tY*tLineBlobLength;
				tBlobstBlobId.mArea += tLineBlobLength;

				if (tBlobLinestYtEntry.mMin < tBlobstBlobId.mLUX)
					tBlobstBlobId.mLUX = tBlobLinestYtEntry.mMin;
				else if (tBlobLinestYtEntry.mMax > tBlobstBlobId.mRDX)
					tBlobstBlobId.mRDX = tBlobLinestYtEntry.mMax;
				if (tY < tBlobstBlobId.mLUY)
					tBlobstBlobId.mLUY = tY;
				else if (tY > tBlobstBlobId.mRDY)
					tBlobstBlobId.mRDY = tY;
			}
		}
	}
}

template <bool scDraw>
void ConnectedComponents<scDraw>::StoreBlobs(){
	mNumComponent=0;
	float tCenterX;
	float tCenterY;
	int tArea;

	for(typename std::map<int, Blob>::iterator i = mBlobs.begin(); i != mBlobs.end(); ++i)
	{
		tArea = (*i).second.mArea;
		if (tArea>=mMinBlobArea && tArea<=mMaxBlobArea){
			tCenterX = (float)(*i).second.mXAccumulator / ((float)(*i).second.mArea * 2.0f);
			tCenterY = (float)(*i).second.mYAccumulator / ((float)(*i).second.mArea);

			mComponents[mNumComponent]->set(tCenterX,tCenterY,tArea);
			mComponents[mNumComponent]->mLUX = (*i).second.mLUX;
			mComponents[mNumComponent]->mLUY = (*i).second.mLUY;
			mComponents[mNumComponent]->mRDX = (*i).second.mRDX;
			mComponents[mNumComponent]->mRDY = (*i).second.mRDY;
			mCenters[mNumComponent].x = tCenterX;
			mCenters[mNumComponent].y = tCenterY;
			mAreas[mNumComponent] = tArea;

			mNumComponent++;
		
			if (mNumComponent>=mMaxNumBlobs){
				return;
			}
		}
	}
}

template <bool scDraw>
void ConnectedComponents<scDraw>::checkDataStructure(const IplImage * pImage){
	if (mBlackWhiteImageWidth!=(int)pImage->width || mBlackWhiteImageHeight!=(int)pImage->height){
		delete [] mBlobIds;
		delete [] mBlobLinesCounter;
		delete [] mFilteredCordinatesPositions;

		for (int i=0; i<mBlackWhiteImageHeight; i++){
			delete [] mBlobLines[i];
		}
		delete [] mBlobLines;

		for (int i=0; i<mBlackWhiteImageHeight; i++){
		delete [] mFilteredCordinates[i];
		}
		delete [] mFilteredCordinates;

		mBlackWhiteImageWidth = std::max((int)pImage->width,scMinBlobSide);
		mBlackWhiteImageHeight = std::max((int)pImage->height,scMinBlobSide);
		mBlackWhiteImageWidth += mBlackWhiteImageWidth%2;
		mBlackWhiteImageHeight += mBlackWhiteImageHeight%2;

		mBlobIds = new int [mBlackWhiteImageHeight*(mBlackWhiteImageWidth/2)];
		mBlobLinesCounter = new int [mBlackWhiteImageHeight];
		mFilteredCordinatesPositions = new int[mBlackWhiteImageHeight];

		mBlobLines = new LineBlob* [mBlackWhiteImageHeight];
		for (int i=0; i<mBlackWhiteImageHeight; i++){
			mBlobLines[i] = new LineBlob[mBlackWhiteImageWidth/2];
		}

		mFilteredCordinates = new int * [mBlackWhiteImageHeight];
		for (int i=0; i<mBlackWhiteImageHeight; i++){
			mFilteredCordinates[i] = new int[(int)(0.5f+(float)mBlackWhiteImageWidth/(float)scMinBlobSide)];
		}
	}
}

template <bool scDraw>
void ConnectedComponents<scDraw>::run(){

	const IplImage *const tInputImage= *mBlackWhiteImage;	

	checkDataStructure(tInputImage);

	int tSquareSide = (int)std::max(1.0f,(0.5f+sqrtf((float)mMinBlobArea)));

	for (int i=0; i<mBlackWhiteImageHeight; i++){
		mBlobLinesCounter[i]=0;
	}
	mBlobs.clear();

	if (tSquareSide>=scMinBlobSide){
		SquaricalInspection(tInputImage,tSquareSide);
		ComputeLinesFromSquares(tInputImage,tSquareSide);
	}
	else{
		ComputeLinesFromPixels(tInputImage);
	}
	ComputeLinesMerge();
	ComputeLinesRemerge();
	SortGroupBlobs();
	StoreBlobs();
	if (scDraw) draw();


}

template <bool scDraw>
void ConnectedComponents<scDraw>::draw(){

	const IplImage *const tInputImage= *mBlackWhiteImage;
    Image::matchImageFormats(tInputImage, &mOutputImage, true);
	cvCopy(tInputImage,mOutputImage);

	for(int i=0; i<mNumComponent; ++i)
	{
		mComponents[i]->draw(mOutputImage, &mFont, cvScalar(100,100,255));
	}

//	cvShowImage("ConnectedComponents", mOutputImage);
//	cvWaitKey(5);
}

template <bool scDraw>
const int ConnectedComponents<scDraw>::scMinBlobSide = 2;

template class ConnectedComponents<true>;
template class ConnectedComponents<false>;
