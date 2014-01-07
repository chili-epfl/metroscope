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

#include "FileGrabber.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <iostream>

FileGrabber::FileGrabber(
    const char *pDirectoryPath):
mDirectoryPath(pDirectoryPath),
mDirectory(pDirectoryPath),
mCurrentFile(mDirectory),
mFirstImage(true),
mImage(cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3))
{
    if (mCurrentFile == cEndOfDirectory)
    	std::cout << "there was no image in your directory" << std::endl;

	while (mCurrentFile != cEndOfDirectory && (!isExtensionAccepted() || !is_regular(*mCurrentFile))) {
		++mCurrentFile;
	}
	loadImage();
}

FileGrabber::~FileGrabber()
{
    cvReleaseImage(&mImage);
}

void FileGrabber::run()
{
    if (mCurrentFile != cEndOfDirectory)
    {
    	if (mFirstImage){
    		mFirstImage = false;
    	} else {
    		findNextFile();
			loadImage();

			while (!mImage && mCurrentFile != cEndOfDirectory){
				std::cout << "Error: " << mCurrentFile->path().string() << " is corrupted" << std::endl;
    			findNextFile();
				loadImage();
			}
    	}
    } else {
    	stop();
    }

    if (mCurrentFile == cEndOfDirectory) {
    	std::cout << "no Image left" << std::endl;
		cvReleaseImage(&mImage);
		mImage = cvCreateImage(cvSize(640,480),IPL_DEPTH_8U,3);
		stop();
    }
}

void FileGrabber::rescanFolder(){
	//mCurrentFile = boost::filesystem::directory_iterator(mDirectoryPath);
	mDirectory = boost::filesystem::path(mDirectoryPath);
	mCurrentFile = boost::filesystem::directory_iterator(mDirectory);
	//cEndOfDirectory = boost::filesystem::directory_iterator();
	mFirstImage = true;
	if (isExtensionAccepted() && is_regular(*mCurrentFile))
		loadImage();
}

void FileGrabber::findNextFile()
{
	bool first = true;
	while (mCurrentFile != cEndOfDirectory && (!isExtensionAccepted() || !is_regular(*mCurrentFile) || first)) {
		++mCurrentFile;
		first = false;
	}
}

bool FileGrabber::isExtensionAccepted() {
	return extension((*mCurrentFile).path()) == ".jpg"
		|| extension((*mCurrentFile).path()) == ".JPG"
		|| extension((*mCurrentFile).path()) == ".png";
}

void FileGrabber::loadImage()
{
	if (mCurrentFile != cEndOfDirectory)
	{
		cvReleaseImage(&mImage);
		std::string tPath = mCurrentFile->path().string();
		int tFilenamePosition = tPath.find_last_of('/')+1;
		mImage=cvLoadImage(tPath.c_str());
		mFilePath = tPath.substr(0, tPath.find_last_of('/')+1);
		mFileName = tPath.substr(tFilenamePosition, tPath.find_last_of('.')-tFilenamePosition);
		mFileSuffix = tPath.substr(tPath.find_last_of('.')+1, tPath.size()-tPath.find_last_of('.')-1);
		mFileNameAndSuffix = tPath.substr(tFilenamePosition, tPath.size()-tFilenamePosition);
	}
}
