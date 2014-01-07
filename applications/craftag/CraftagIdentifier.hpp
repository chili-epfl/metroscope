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

#ifndef tCraftagIdentifier_HPP
#define tCraftagIdentifier_HPP

#include <qa/pipeables/Pipeable.hpp>
#include <qa/components/vision/Craftag.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/legacy/compat.hpp>
#include <qa/utils/Draw.hpp>
#include <qa/utils/CvWykobiBridge.hpp>

class CraftagIdentifier: public Pipeable
{
    public:

        CraftagIdentifier(
        		const IplImage *const *pInputImage,
        		int pFromId = 0,
        		int pToId = sMaxNTags):
        mInputImage(pInputImage),
        mFromId(pFromId),
        mToId(pToId),
        mOutputImage(cvCloneImage(*pInputImage)),
        mTags(new Craftag*[pToId-pFromId])
        {
        	for (int i = mFromId; i < mToId; ++i) mTags[i-mFromId] = new Craftag(i);
			cvInitFont(&mFont,CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5);
        	cvNamedWindow("CraftagIdentifier");
        }

		virtual ~CraftagIdentifier()
		{
			for (int i = mFromId; i < mToId; ++i) delete mTags[i-mFromId];
        	delete [] mTags;
		}

    protected:
        void run()
        {
        	cvCopyImage(*mInputImage, mOutputImage);
        	for (int i = mFromId; i < mToId; ++i) {
        		Craftag *tTag = mTags[i-mFromId];
				if (tTag->isPresent()){
					wykobi::point2d<float> tCenter = tTag->getCenter();
					wykobi::point2d<float> tTop = tCenter+tTag->getYUnit();
					wykobi::point2d<float> tRight = tCenter+tTag->getXUnit();

					static const int scShift = 2;
					static const float scPrecision = 1<<scShift;
					cvLine(mOutputImage, cvPoint(tCenter.x*scPrecision, tCenter.y*scPrecision), cvPoint(tTop.x*scPrecision, tTop.y*scPrecision), sColor, 1, CV_AA, scShift);
					cvLine(mOutputImage, cvPoint(tCenter.x*scPrecision, tCenter.y*scPrecision), cvPoint(tRight.x*scPrecision, tRight.y*scPrecision), sColor, 1, CV_AA, scShift);
					//char tText[256];
					//sprintf(tText, "%d:(%.2f,%.2f),(%.2f,%.2f),(%.2f,%.2f)", i, tCenter.x, tCenter.y, tTop.x, tTop.y, tRight.x, tRight.y);
					char tText[4+1];
					sprintf(tText, "%d", i);
					cvPutText(mOutputImage, tText, cvPoint(tCenter.x, tCenter.y), &mFont, sColor);

					CvPoint2D32f tCvCorners[4];
					CvWykobiBridge::convertQuad(wykobi::scale(scPrecision, scPrecision, tTag->getCorners()), tCvCorners);
					for (int i = 0; i < 4; ++i) {
						cvLine(mOutputImage, cvPointFrom32f(tCvCorners[i]), cvPointFrom32f(tCvCorners[(i+1)%4]), sColor, 1, CV_AA, scShift);
					}
				}
			}
        	cvShowImage("CraftagIdentifier", mOutputImage);
        	char tKeyPressed = cvWaitKey(1);
        	if (tKeyPressed == 'q') stop();
        }

        const IplImage *const *mInputImage;
        int mFromId;
        int mToId;
        IplImage *mOutputImage;
        Craftag **mTags;
        const static CvScalar sColor;
		CvFont mFont;
        const static int sMaxNTags = 1024;

    private:
        CraftagIdentifier();
        CraftagIdentifier(const CraftagIdentifier& pNaiveCalib);
        CraftagIdentifier& operator=(const CraftagIdentifier& pNaiveCalib);
};

const CvScalar CraftagIdentifier::sColor = CV_RGB(255, 255, 255);

#endif
