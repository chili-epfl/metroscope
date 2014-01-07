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

#include "VideoSaver.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "../../utils/Time.hpp"

	#ifdef WIN32
template<bool scLiveStream>
const int VideoSaver<scLiveStream>::scDefaultCodec = CV_FOURCC_PROMPT;
	#else
template<bool scLiveStream>
const int VideoSaver<scLiveStream>::scDefaultCodec = CV_FOURCC_DEFAULT;
	#endif


template<bool scLiveStream>
VideoSaver<scLiveStream>::VideoSaver(
    const IplImage * const*pInputImage,
	const char *pFilename,
	double pFPS,
	int pCodec):
mInputImage(pInputImage),
mVideoWriter(cvCreateVideoWriter(pFilename, pCodec, pFPS, cvGetSize(*pInputImage))),
mStartTime(Time::MillisTimestamp()),
mFrameCount(0),
mMillisecondPerFrame(1000.0/pFPS)
{
}

template<bool scLiveStream>
VideoSaver<scLiveStream>::~VideoSaver(){
	cvReleaseVideoWriter(&mVideoWriter);
}

template<bool scLiveStream>
void VideoSaver<scLiveStream>::run()
{
	if (scLiveStream)
	{
		long tTimeToWait = (mStartTime+ (long) (mFrameCount*mMillisecondPerFrame+0.5))-Time::MillisTimestamp();

		if (tTimeToWait <= 0) std::cerr << -tTimeToWait << "ms delay at saving frame #" << mFrameCount << std::endl;
		else Time::Sleep(tTimeToWait);
	}
	cvWriteFrame(mVideoWriter,*mInputImage);
	++mFrameCount;
}

template class VideoSaver<true>;
template class VideoSaver<false>;
