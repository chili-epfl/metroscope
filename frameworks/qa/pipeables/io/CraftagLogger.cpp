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

#include "CraftagLogger.hpp"
#include <qa/utils/Time.hpp>
#include <qa/components/decorators/FiducialDecorator.hpp>
#include <iostream>


CraftagLogger::CraftagLogger(
    const char *pFilename,
	DecoratorManager &pDecoratorManager,
	CraftagRegistrar &pCraftagRegistrar):
mLogfile(pFilename),
mDecoratorManager(pDecoratorManager),
mCraftagRegistrar(pCraftagRegistrar)
{
	if (!mLogfile) std::cout << "CraftagLogger: Unable to write in file: " << pFilename << std::endl;
	mLogfile << "taglog = (" << std::endl;
}

CraftagLogger::~CraftagLogger()
{
	mLogfile << "    {" << std::endl;
	mLogfile << "        timestamp = " << Time::MillisTimestamp() << "L;" << std::endl;
	mLogfile << "        tags = ();" << std::endl;
	mLogfile << "    }" << std::endl;

	mLogfile << ");" << std::endl;
	mLogfile.close();
}

void CraftagLogger::run()
{
	std::ios::fmtflags tPreviousFlags = std::cout.flags();
	std::cout << std::showpoint;

	mLogfile << "    {" << std::endl;
	mLogfile << "        timestamp = " << Time::MillisTimestamp() << "L;" << std::endl;
	mLogfile << "        tags = (";
	const int cNTotalTagsTracked = mCraftagRegistrar.GetTranscoder().getNTotalTagsTracked();
	const int cCurrentFrameId = mCraftagRegistrar.getFrameId();
	bool tFirstLine = true;
	for (int i = 0; i < cNTotalTagsTracked; ++i) {
		if (cCurrentFrameId == mCraftagRegistrar.getLastDetectedFrameFromInternalId(i))
		{
			if (tFirstLine) tFirstLine = false;
			else mLogfile << ",";
			mLogfile << std::endl << "            { id = " << mCraftagRegistrar.GetTranscoder().getTagId(i) << "; corners = [ ";
			const CvPoint2D32f * tCorners = mCraftagRegistrar.getCornersFromInternalId(i);
			for (int j = 0; j < 4; ++j) {
				if (j) mLogfile << ", ";
				mLogfile << tCorners[j].x << ", " << tCorners[j].y;
			}
			mLogfile << " ]; }";
		}
	}
	mLogfile << std::endl << "        );" << std::endl;

	//We add the ladybug game stats, if applicable
	if(&mDecoratorManager!=NULL){
		//We try to find the object for the ladybug game
		decorators::FiducialDecorator *gameDec = mDecoratorManager.getDecorator("activities/proportions.cfggame_activity");
		if(gameDec){
				//If the game decorator is present, we insert the stats
				mLogfile << "        game = (";
				mLogfile << gameDec->GetStringRepresentation();
				mLogfile << ");" << std::endl;
		}
	}

	mLogfile << "    }," << std::endl;

	std::cout.flags(tPreviousFlags);
}
