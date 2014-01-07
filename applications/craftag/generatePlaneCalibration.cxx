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

#include <qa/pipeables/grabbers/FlyCapture2Grabber.hpp>
#include <qa/pipeables/grabbers/VideoGrabber.hpp>
#include <qa/pipeables/io/OpenGl2DDisplay.hpp>
#include <qa/pipeables/io/VideoSaver.hpp>
#include <qa/pipeables/vision/Flip.hpp>
#include <qa/pipeables/vision/Inverter.hpp>
#include <qa/pipeables/vision/Gray2Color.hpp>
#include <qa/pipeables/misc/DelayedStopper.hpp>
#include "Chessboard.hpp"
#include "PlaneCalib.hpp"
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{
	static const double scFramerate = 3.75f;
	static const int scNFrames = 20;

	bool tRecord = false;
	bool tReplay = false;
	bool tCamOnly = false;
	bool tProjOnly = false;
	for (int i = 1; i < argc; ++i)
	{
		std::string tArgument(argv[i]);
		if (tArgument == "--record") tRecord = true;
		else if (tArgument == "--replay") tReplay = true;
		else if (tArgument == "--cam-only") tCamOnly = true;
		else if (tArgument == "--proj-only") tProjOnly = true;
	}

	int tXRes = 1280;
	int tYRes = 768;
	FlyCapture2Grabber *tCameraGrabber = 0;
	OpenGl2DDisplay *tDisplay = 0;
	if (!tReplay)
	{
		tCameraGrabber = new FlyCapture2Grabber(1280, 960, scFramerate);
		tDisplay = new OpenGl2DDisplay(0, 0, tXRes, tYRes);
	}

	if (!tProjOnly)
	{
		if (tReplay)
		{
			if (!tRecord)
			{
				VideoGrabber *tVideoGrabber = new VideoGrabber("camCalibVideo.avi");
				PlaneCalibration *tCam2World = new PlaneCalibration(76, 47, 1, tVideoGrabber->Image());

				*tVideoGrabber
				| *tCam2World
				| *tVideoGrabber;

				tVideoGrabber->start();
				tCam2World->Write("cam2world.map", 10.0f);

				delete tCam2World;
				delete tVideoGrabber;
			}
		}
		else
		{
			Flip *tFlip = new Flip(Flip::scHorizontally, tCameraGrabber->Image());
			tDisplay->BackgroundColor(0.0f, 0.0f, 0.0f);
			if (tRecord)
			{
				Gray2Color *tGray2Color = new Gray2Color(tFlip->GetOutputImage());
				VideoSaver<false> *tVideoSaver = new VideoSaver<false>(tGray2Color->GetOutputImage(),"camCalibVideo.avi", scFramerate, VideoSaver<false>::scDefaultCodec);
				DelayedStopper *tDelayedStopper = new DelayedStopper(scNFrames);

				*tCameraGrabber
				| *tFlip
				| *tDisplay
				| *tGray2Color
				| *tVideoSaver
				| *tDelayedStopper
				| *tCameraGrabber;

				tCameraGrabber->start();

				delete tDelayedStopper;
				delete tGray2Color;
				delete tVideoSaver;
			}
			else
			{
				PlaneCalibration *tCam2World = new PlaneCalibration(76, 47, 1, tFlip->GetOutputImage());

				*tCameraGrabber
				| *tFlip
				| *tDisplay
				| *tCam2World
				| *tCameraGrabber;

				tCameraGrabber->start();
				tCam2World->Write("cam2world.map", 10.0f);

				delete tCam2World;
			}
			delete tFlip;
		}
	}


	if (!tCamOnly)
	{
		int tSquareSize = 32;
		int tNSquareCols = tYRes/tSquareSize;
		int tNSquareRows = tXRes/tSquareSize;
		if (tReplay)
		{
			if (!tRecord)
			{
				VideoGrabber *tVideoGrabber = new VideoGrabber("projCalibVideo.avi");
				PlaneCalibration *tWorld2Proj = new PlaneCalibration(tNSquareRows, tNSquareCols, 1, tVideoGrabber->Image());

				*tVideoGrabber
				| *tWorld2Proj
				| *tVideoGrabber;

				tVideoGrabber->start();

				PlaneCoordinateMapper *tCamPx2WorldMm = new PlaneCoordinateMapper("cam2world.map");
				tWorld2Proj->Write("world2proj.map", tSquareSize, 2.0f, tCamPx2WorldMm);
				delete tCamPx2WorldMm;
				delete tWorld2Proj;
			}
		}
		else
		{
			Flip *tFlip = new Flip(Flip::scHorizontally, tCameraGrabber->Image());
			Chessboard *tChessboard = new Chessboard(0, 0, tNSquareCols, tNSquareRows, tSquareSize, tSquareSize);
			tDisplay->BackgroundColor(1.0f, 1.0f, 1.0f);
			*tDisplay | *tChessboard;
			tDisplay->start();
			Inverter *tInverter = new Inverter(tFlip->GetOutputImage());

			if (tRecord)
			{
				Gray2Color *tGray2Color = new Gray2Color(tInverter->GetOutputImage());
				VideoSaver<false> *tVideoSaver = new VideoSaver<false>(tGray2Color->GetOutputImage(), "projCalibVideo.avi", scFramerate, VideoSaver<false>::scDefaultCodec);
				DelayedStopper *tDelayedStopper = new DelayedStopper(scNFrames);

				*tChessboard
				| *tDisplay
				| *tCameraGrabber
				| *tFlip
				| *tInverter
				| *tGray2Color
				| *tVideoSaver
				| *tDelayedStopper
				| *tChessboard;

				tChessboard->start();

				delete tDelayedStopper;
				delete tGray2Color;
				delete tVideoSaver;
			}
			else
			{
				PlaneCalibration *tWorld2Proj = new PlaneCalibration(tNSquareRows, tNSquareCols, 1, tInverter->GetOutputImage());

				*tChessboard
				| *tDisplay
				| *tCameraGrabber
				| *tFlip
				| *tInverter
				| *tWorld2Proj
				| *tChessboard;

				tChessboard->start();

				PlaneCoordinateMapper *tCamPx2WorldMm = new PlaneCoordinateMapper("cam2world.map");
				tWorld2Proj->Write("world2proj.map", tSquareSize, 2.0f, tCamPx2WorldMm);
				delete tCamPx2WorldMm;

				delete tWorld2Proj;
			}
			delete tInverter;
			delete tChessboard;
			delete tFlip;
		}
	}

	if (!tReplay)
	{
		delete tDisplay;
		delete tCameraGrabber;
	}

	return 0;
}
