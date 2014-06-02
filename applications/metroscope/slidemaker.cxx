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

#include <iostream>
#include <libconfig.h++>
#include <boost/format.hpp>
#include <fstream>
#include <qa/pipeables/misc/DecoratorManager.hpp>

//NetworkedStateManager* stateManager;


int main(int argc, char* argv[])
{
	//TODO: printing size?
	if (argc < 2) return 1;

	bool tPortrait = true;
	const std::size_t tNCol = 2;
	const std::size_t tNRow = 4;

	float tPageWidth = 29.7f;
	float tPageHeight = 21.0f;
	if (tPortrait) { 
		tPageWidth = 21.0f;
		tPageHeight = 29.7f;
	}
	const std::size_t tSlidesPerPage = tNCol*tNRow;
	const float tSlideWidth = tPageWidth/ (float) tNCol;
	const float tSlideHeight = tPageHeight/ (float) tNRow;
	const float tThumbnailRatio = 0.8f;
	const float tTagPadding = 10.0f/14.0f;
	const float tTagSize = std::min(tTagPadding*tSlideWidth, tTagPadding*tSlideHeight);
	const float tTagXMargin = (tSlideWidth - tTagSize)/2.0f;
	const float tTagYMargin = (tSlideHeight - tTagSize)/2.0f;
	const float tTextSize = 5.0f;

	libconfig::Config tConfig;
	try {
		tConfig.readFile( argv[1] );
	} catch(libconfig::FileIOException &) {
		std::cerr << "Unagle to load the config file" << std::endl;
	} catch(libconfig::ParseException &e) {
		std::cerr << "Line "<< e.getLine() << ": " << e.getError() << std::endl;
	}

	auto scHeader = boost::format(
"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\
<svg xmlns:svg=\"http://www.w3.org/2000/svg\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" width=\"%1%cm\" height=\"%2%cm\">") % tPageWidth % tPageHeight;
	boost::format scImage("\t<image xlink:href=\"%1%\" width=\"%2%cm\" height=\"%3%cm\" x=\"%4%cm\" y=\"%5%cm\" />");
	boost::format scRectangle("\t<rect style=\"fill:none;stroke:#000000\" width=\"%3%cm\" height=\"%4%cm\" x=\"%1%cm\" y=\"%2%cm\" />");
	boost::format scTagFilename("./tags/400px/%1%.png");
	boost::format scText("<text x=\"%2%cm\" y=\"%3%cm\" font-size=\"%4%\" fill=\"white\" stroke=\"black\" stroke-width=\"1\" font-weight=\"bold\" >%1%</text>");
	boost::format scFooter("</svg>");

	std::size_t tPage = 1;
	boost::format tNotesFilename("./notes%|03|.svg");
	std::ofstream tRecto((tNotesFilename % (tPage++)).str());
	std::ofstream tVerso((tNotesFilename % (tPage++)).str());

	tRecto << scHeader << std::endl;
	tVerso << scHeader << std::endl;
	try {
		libconfig::Setting &tSlides = tConfig.lookup("slides");
		std::size_t scNSlides = (std::size_t) tSlides.getLength();
		for (
			std::size_t i = 0, tNSlideOnPage = 0, tCol = 0, tRow = 0;
			i< scNSlides ;
			++i, ++tNSlideOnPage, tCol = ((tCol+1) % tNCol), tRow += (tCol == 0))
		{
			if (tNSlideOnPage >= tSlidesPerPage) {
				tRecto << scFooter << '\n';
				tVerso << scFooter << '\n';
				tRecto.close();
				tVerso.close();

				tRecto.open((tNotesFilename % (tPage++)).str());
				tVerso.open((tNotesFilename % (tPage++)).str());
				tRecto << scHeader << '\n';
				tVerso << scHeader << '\n';
				tNSlideOnPage = 0;
				tRow = 0;
			}

			std::string tPath = tSlides[i];
			tRecto << (
				scRectangle
				% (tSlideWidth*tCol)
				% (tSlideHeight*tRow)
				% (tSlideWidth)
				% (tSlideHeight)
			) << '\n';
			tRecto << (
				scImage
				% tPath
				% (tSlideWidth*tThumbnailRatio)
				% (tSlideHeight*tThumbnailRatio)
				% (tSlideWidth*tCol)
				% (tSlideHeight*tRow)
			) << '\n';
			tRecto << (
				scText
				% (i+1)
				% (tSlideWidth*tCol)
				% (tSlideHeight*(tRow+1))
				% (tSlideHeight*tTextSize)
			) << '\n';
			tVerso << (
				scImage
				% (scTagFilename % (i+1))
				% tTagSize
				% tTagSize
				% ((1-tCol)*tSlideWidth+tTagXMargin)
				% (tRow*tSlideHeight+tTagYMargin)
			) << '\n';
			tVerso << (
				scText
				% (i+1)
				% (tSlideWidth*(1-tCol))
				% (tSlideHeight*(tRow+1))
				% (tSlideHeight*tTextSize)
			) << '\n';
		}
	}
	catch (libconfig::SettingNotFoundException &e) {
		std::cerr << "Setting not found: " << e.getPath() << std::endl;
	}
	tRecto << scFooter << std::endl;
	tVerso << scFooter << std::endl;
	tRecto.close();
	tVerso.close();
}
