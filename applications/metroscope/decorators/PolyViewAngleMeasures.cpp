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

#include "PolyViewAngleMeasures.hpp"
#include <iostream>

const std::string decorators::PolyViewAngleMeasures::scDecoratorName("PolyViewAngleMeasures");
const  DecoratorManager::Registerer decorators::PolyViewAngleMeasures::mRegisterer(decorators::PolyViewAngleMeasures::scDecoratorName, &decorators::PolyViewAngleMeasures::create);

namespace {
	static const int scRed = 0;
	static const int scYellow = 3;
	static const int scBlue = 6;
	static const int scMagenta = 9;
	static const int scGreen = 12;
	static const int scCyan = 15;
	static const float scColors[3*6] = {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f};
}//TODO: We only do until 6 different colors! Should be enough for now...

decorators::FiducialDecorator *decorators::PolyViewAngleMeasures::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::PolyViewAngleMeasures(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), *(PolyModel *)pDecoratorManager.loadDecorator(pSetting["model"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::PolyViewAngleMeasures::PolyViewAngleMeasures(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const PolyModel &pPolyModel):
FiducialDecorator(pDecoratorManager, pMarker),
mPolyModel(pPolyModel)
{
}

void decorators::PolyViewAngleMeasures::update() {

	if (mMarker->isPresent())
	{
		wykobi::quadix<float ,2> tMarkerCorners = mPolyModel.getMarker().getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tMarkerCorners);
		wykobi::point2d<float> tOrigin;
		wykobi::point2d<float> tXUnit;
		wykobi::point2d<float> tYUnit;
		FiducialMarker::ComputeBasisFromSquare(tMarkerCorners, tOrigin, tXUnit, tYUnit);

		int tNumAngles = mPolyModel.getSize();

		//float tMeasures[tNumAngles];
		float* tMeasures = new float[tNumAngles];
		for (int i = 0; i < tNumAngles; ++i) {
			tMeasures[i] = wykobi::oriented_vertex_angle(mPolyModel.getPolygon()[(i+(tNumAngles-1))%tNumAngles], mPolyModel.getPolygon()[i], mPolyModel.getPolygon()[(i+1)%tNumAngles]);
		}

		wykobi::polygon<float ,2> tPolyCorners;
		for (int i = 0; i < tNumAngles; ++i) {
			tPolyCorners.push_back(tOrigin + mPolyModel.getPolygon()[i].x*tXUnit + mPolyModel.getPolygon()[i].y*tYUnit);
		}

		mDecoratorManager.GetWorld2Proj().InterpolatedMapOnPolygon(tPolyCorners, tNumAngles);


		int tInitialColorIndices[6] = { scRed, scYellow, scBlue, scMagenta, scGreen, scCyan};
		int* tColorIndices = new int[tNumAngles];
		for (int i = 0; i < tNumAngles; ++i) tColorIndices[i] = tInitialColorIndices[i%6];

		for (int i = 0; i < tNumAngles; ++i) {
			for (int j = 0; j < i; ++j) {
				if (wykobi::abs(tMeasures[i] - tMeasures[j]) < wykobi::Epsilon)
					tColorIndices[i] = tColorIndices[j];
			}
		}

		mDecoratorManager.GetWorld2Proj().InterpolatedMap(tOrigin);
		for (int i = 0; i < tNumAngles; ++i) {
			if (wykobi::abs(tMeasures[i] - 90.0f) < wykobi::Epsilon)
			{
				wykobi::point2d<float> tSide1 = 10.0f*wykobi::normalize(tPolyCorners[(i+(tNumAngles))%tNumAngles]-tPolyCorners[i]);
				wykobi::point2d<float> tSide2 = 10.0f*wykobi::normalize(tPolyCorners[(i+1)%tNumAngles]-tPolyCorners[i]);
				wykobi::point2d<float> tOppositeCorner = tSide1+tSide2;
				tSide1 = tPolyCorners[i]+tSide1;
				tSide2 = tPolyCorners[i]+tSide2;
				tOppositeCorner = tPolyCorners[i]+tOppositeCorner;//Not sure about this... does it still make sense?
				mDecoratorManager.GetDisplay().RenderLine(tSide1.x, tSide1.y, tOppositeCorner.x, tOppositeCorner.y, scColors[tColorIndices[i]], scColors[tColorIndices[i]+1], scColors[tColorIndices[i]+2]);
				mDecoratorManager.GetDisplay().RenderLine(tSide2.x, tSide2.y, tOppositeCorner.x, tOppositeCorner.y, scColors[tColorIndices[i]], scColors[tColorIndices[i]+1], scColors[tColorIndices[i]+2]);
			}
			else
			{
				float tOrientation1 = wykobi::cartesian_angle(tPolyCorners[(i+(tNumAngles-1))%tNumAngles]-tPolyCorners[i]);
				float tOrientation2 = wykobi::cartesian_angle(tPolyCorners[(i+1)%tNumAngles]-tPolyCorners[i]);
				mDecoratorManager.GetDisplay().RenderArc(
						tPolyCorners[i].x, tPolyCorners[i].y,
						20.0f, 20.0f,
						tOrientation2, tOrientation1,
						scColors[tColorIndices[i]], scColors[tColorIndices[i]+1], scColors[tColorIndices[i]+2]);
			}

			wykobi::point2d<float> tCorner = tPolyCorners[i]-tOrigin;
			wykobi::point2d<float> tTextPosition = tPolyCorners[i]+75.0f*wykobi::normalize(wykobi::make_vector(tCorner));
			char tText[20];
			sprintf(tText, "%.0f°", tMeasures[i]);
			mDecoratorManager.GetDisplay().RenderText(tText, tTextPosition.x, tTextPosition.y, 1.0f, scColors[tColorIndices[i]], scColors[tColorIndices[i]+1], scColors[tColorIndices[i]+2]);
		}
	}


}
