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

#include "LogBufferDisplay.hpp"
#include <iostream>
#include <GL/glut.h>

const std::string decorators::LogBufferDisplay::scDecoratorName("LogBufferDisplay");
const  DecoratorManager::Registerer decorators::LogBufferDisplay::mRegisterer(decorators::LogBufferDisplay::scDecoratorName, &decorators::LogBufferDisplay::create);

decorators::FiducialDecorator *decorators::LogBufferDisplay::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::LogBufferDisplay(pDecoratorManager,
			pDecoratorManager.loadMarker(pSetting["marker"]),
			pSetting["vistype"],
			pSetting["scale"],
			* (LogBuffer *) pDecoratorManager.loadDecorator(pSetting["logbuffer"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::LogBufferDisplay::LogBufferDisplay(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, int pType, float pScale, const LogBuffer &pLogBuffer):
FiducialDecorator(pDecoratorManager, pMarker),
mType(pType),
mScale(1.0f/pScale),
mLogBuffer(pLogBuffer)
{
}

void decorators::LogBufferDisplay::update() {
	if (mMarker->isPresent())
	{
		glLineWidth(3.0f);
		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(*mMarker, mDecoratorManager.GetCam2World(), mDecoratorManager.GetWorld2Proj());
		mDecoratorManager.GetDisplay().Scale(mScale, mScale);
		static const float scScreeW = mDecoratorManager.GetDisplay().GetWidth();
		static const float scScreeH = mDecoratorManager.GetDisplay().GetHeight();
		float tLogSize = (* mLogBuffer.GetLog().begin()).second.size();
		switch (mType)
		{
			default:
			case 0:
				mDecoratorManager.GetDisplay().Translate(-scScreeW/2.0f, -scScreeH/2.0f);
				mDecoratorManager.GetDisplay().RenderQuadFilled(
					0, 0,
					scScreeW, 0,
					scScreeW, scScreeH,
					0, scScreeH,
					1.0f, 1.0f, 1.0f 
					);
				for (auto logIt = mLogBuffer.GetLog().begin(); logIt != mLogBuffer.GetLog().end(); ++logIt)
				{
					auto tName = (*logIt).first;
					float tR = 0.9019607843f;
					float tG = 0.3803921569f;
					float tB = 0.0039215686f;
 					if (tName == "sheet")
					{
						tR = 0.0f;
						tG = 0.0f;
						tB = 0.0f;
					}
					else if (tName.substr(0,5) == "shape")
					{
					 	tR = 0.368627451f;
						tG = 0.2352941176f;
						tB = 0.6f;
					}

					auto tPrevious = (*logIt).second.front();
					mDecoratorManager.GetCam2World().InterpolatedMap(tPrevious.x, tPrevious.y);
					for (auto entryIt = (*logIt).second.begin(); entryIt != (*logIt).second.end(); ++entryIt)
					{
						auto tCurrent = *entryIt;
						if (tPrevious.present && tCurrent.present)
						{
							mDecoratorManager.GetCam2World().InterpolatedMap(tCurrent.x, tCurrent.y);
							mDecoratorManager.GetDisplay().RenderLine(
								tPrevious.x, tPrevious.y,
								tCurrent.x, tCurrent.y,
								tR, tG, tB);
							tPrevious = tCurrent;
						}
						else if (tCurrent.present)
						{
							tPrevious = tCurrent;
						}
					}
				}
				break;
			case 1:
				mDecoratorManager.GetDisplay().Translate(-scScreeW/2.0f, -tLogSize/2.0f);
				mDecoratorManager.GetDisplay().RenderQuadFilled(
					0, 0,
					scScreeW, 0,
					scScreeW, tLogSize,
					0, tLogSize,
					1.0f, 1.0f, 1.0f 
					);
				for (auto logIt = mLogBuffer.GetLog().begin(); logIt != mLogBuffer.GetLog().end(); ++logIt)
				{
					auto tName = (*logIt).first;
					
					float tR = 0.1725490196;
					float tG = 0.6274509804;
					float tB = 0.1725490196;
					if (tName[0] == 't')
					{
						tR = 0.1215686275;
						tG = 0.4666666667;
						tB = 0.7058823529;
					}
					else if (tName.substr(0,5) == "shape")
					{
						tR = 0.1725490196;
						tG = 0.6274509804;
						tB = 0.1725490196;
					}
					if (tName == "sheet")
					{
						tR = 0.0f;
						tG = 0.0f;
						tB = 0.0f;
					}

					auto tPrevious = (*logIt).second.front();
					mDecoratorManager.GetCam2World().InterpolatedMap(tPrevious.x, tPrevious.y);

					float tCurrentT=tLogSize;
					float tPreviousT=tLogSize;
					for (auto entryIt = (*logIt).second.begin(); entryIt != (*logIt).second.end(); ++entryIt)
					{
						auto tCurrent = *entryIt;
						tCurrentT -= 1.0f;
						if (tPrevious.present && tCurrent.present)
						{
							mDecoratorManager.GetCam2World().InterpolatedMap(tCurrent.x, tCurrent.y);
							if (tName == "sheet")
							{
								static const float scShift = 10.5f*17.0f;
								mDecoratorManager.GetDisplay().RenderLine(
									tPrevious.x+scShift , tPreviousT,
									tCurrent.x+scShift , tCurrentT,
									tR, tG, tB);
								mDecoratorManager.GetDisplay().RenderLine(
									tPrevious.x-scShift , tPreviousT,
									tCurrent.x-scShift , tCurrentT,
									tR, tG, tB);
							}
							else
							{
								mDecoratorManager.GetDisplay().RenderLine(
									tPrevious.x, tPreviousT,
									tCurrent.x, tCurrentT,
									tR, tG, tB);
							}
							tPrevious = tCurrent;
							tPreviousT = tCurrentT;
						}
						else if (tCurrent.present)
						{
							tPrevious = tCurrent;
							tPreviousT = tCurrentT;
						}
					}
				}
				break;
			case 2:
				mDecoratorManager.GetDisplay().Translate(-tLogSize/2.0f, -scScreeH/2.0f);
				mDecoratorManager.GetDisplay().RenderQuadFilled(
					0, 0,
					0, scScreeH, 
					tLogSize, scScreeH, 
					tLogSize, 0, 
					1.0f, 1.0f, 1.0f 
					);
				for (auto logIt = mLogBuffer.GetLog().begin(); logIt != mLogBuffer.GetLog().end(); ++logIt)
				{
					auto tName = (*logIt).first;

					if (tName.substr(1,4) == "card") continue;

					float tR = 0.9960784314;
					float tG = 0.4980392157;
					float tB = 0.0549019608;
					auto tId = tName[5];
					if (tId == '1' || tId == '2' || tId == '3')
					{
						tR = 0.1215686275;
						tG = 0.4666666667;
						tB = 0.7058823529;
					}
					else if (tName == "sheet")
					{
						tR = 0.0f;
						tG = 0.0f;
						tB = 0.0f;
					}

					auto tPrevious = (*logIt).second.front();
					mDecoratorManager.GetCam2World().InterpolatedMap(tPrevious.x, tPrevious.y);

					float tCurrentT=0.0f;
					float tPreviousT=0.0f;
					for (auto entryIt = (*logIt).second.begin(); entryIt != (*logIt).second.end(); ++entryIt)
					{
						auto tCurrent = *entryIt;
						tCurrentT += 1.0f;
						if (tPrevious.present && tCurrent.present)
						{
							mDecoratorManager.GetCam2World().InterpolatedMap(tCurrent.x, tCurrent.y);
							if (tName == "sheet")
							{
								
								mDecoratorManager.GetDisplay().RenderLine(
									tPreviousT, tPrevious.y+7.0f*17.0, 
									tCurrentT, tCurrent.y+7.0f*17.0, 
									tR, tG, tB);
								mDecoratorManager.GetDisplay().RenderLine(
									tPreviousT, tPrevious.y+18.0f*17.0, 
									tCurrentT, tCurrent.y+18.0f*17.0, 
									tR, tG, tB);
								mDecoratorManager.GetDisplay().RenderLine(
									tPreviousT, tPrevious.y+31.0f*17.0, 
									tCurrentT, tCurrent.y+31.0f*17.0, 
									tR, tG, tB);
							}
							else
							{
								mDecoratorManager.GetDisplay().RenderLine(
									tPreviousT, tPrevious.y, 
									tCurrentT, tCurrent.y, 
									tR, tG, tB);
							}
							tPrevious = tCurrent;
							tPreviousT = tCurrentT;
						}
						else if (tCurrent.present)
						{
							tPrevious = tCurrent;
							tPreviousT = tCurrentT;
						}
					}
				}
				break;
		}
		mDecoratorManager.GetDisplay().PopTransformation();
		glLineWidth(1.0f);
	}
}
