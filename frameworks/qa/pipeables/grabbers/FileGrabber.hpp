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

#ifndef FileGrabber_HPP
#define FileGrabber_HPP

#include "../Pipeable.hpp"
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <boost/filesystem/operations.hpp>

class FileGrabber : public Pipeable{
    public:
        FileGrabber(const char *pDirectoryPath);

        const IplImage *const *Image() const {return &mImage;};
        const std::string *FilePath() const {return &mFilePath;};
		const std::string *FileName() const {return &mFileName;};
		const std::string *FileSuffix() const {return &mFileSuffix;};
        const std::string *FileNameAndSuffix() const {return &mFileNameAndSuffix;};

        ~FileGrabber();
		void rescanFolder();

		bool IsEndFolder() const {return mCurrentFile == cEndOfDirectory;}

    protected:
		void run();

		void findNextFile();
		void loadImage();

		bool isExtensionAccepted();

		const char *mDirectoryPath;
        boost::filesystem::path mDirectory;
        boost::filesystem::directory_iterator mCurrentFile;
        boost::filesystem::directory_iterator cEndOfDirectory;
        bool mFirstImage;
        IplImage *mImage;

		std::string mFilePath;
        std::string mFileNameAndSuffix;
		std::string mFileName;
		std::string mFileSuffix;
};
#endif


