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

#include "FoldedMirror.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>
#include <qa/utils/CvWykobiBridge.hpp>

const std::string decorators::FoldedMirror::scDecoratorName("FoldedMirror");
const  DecoratorManager::Registerer decorators::FoldedMirror::mRegisterer(decorators::FoldedMirror::scDecoratorName, &decorators::FoldedMirror::create);

decorators::FiducialDecorator *decorators::FoldedMirror::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try {
		return new decorators::FoldedMirror(pDecoratorManager,
				pDecoratorManager.loadMarker(pSetting["marker"]),
				*(QuadModel *)pDecoratorManager.loadDecorator(pSetting["model"]),
				*(AxisModel *)pDecoratorManager.loadDecorator(pSetting["orgAxis"]),
				*(AxisModel *)pDecoratorManager.loadDecorator(pSetting["dstAxis"]),
				*pDecoratorManager.loadMarker(pSetting["verso"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::FoldedMirror::FoldedMirror(DecoratorManager &pDecoratorManager, FiducialMarker *pMarker, const QuadModel &pQuadModel, const AxisModel &pOrgAxisModel, const AxisModel &pDstAxisModel, const FiducialMarker &pVerso):
FiducialDecorator(pDecoratorManager, pMarker),
mQuadModel(pQuadModel),
mOrgAxisModel(pOrgAxisModel),
mDstAxisModel(pDstAxisModel),
mVerso(pVerso)
{
}

void decorators::FoldedMirror::update() {
	if (mMarker->isPresent() && mQuadModel.getMarker().isPresent() && mOrgAxisModel.getMarker().isPresent() && mDstAxisModel.getMarker().isPresent())
	{
	if (wykobi::is_equal(mOrgAxisModel.getMarker().getCorners()[0], mDstAxisModel.getMarker().getCorners()[0])) std::cout << "sdfg" << std::endl;
		wykobi::quadix<float, 2> tOrgAxisMarkerCorners = mOrgAxisModel.getMarker().getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tOrgAxisMarkerCorners);
		wykobi::point2d<float> tAxisOrigin;
		wykobi::point2d<float> tAxisXUnit;
		wykobi::point2d<float> tAxisYUnit;
		FiducialMarker::ComputeBasisFromSquare(tOrgAxisMarkerCorners, tAxisOrigin, tAxisXUnit, tAxisYUnit);
		wykobi::line<float, 2> tAxisPoints;
		tAxisPoints[0] = tAxisOrigin + mOrgAxisModel.getAxis()[0].x*tAxisXUnit + mOrgAxisModel.getAxis()[0].y*tAxisYUnit;
		tAxisPoints[1] = tAxisOrigin + mOrgAxisModel.getAxis()[1].x*tAxisXUnit + mOrgAxisModel.getAxis()[1].y*tAxisYUnit;

		wykobi::quadix<float, 2> tModelMarkerCorners = mQuadModel.getMarker().getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tModelMarkerCorners);
		wykobi::point2d<float> tModelOrigin;
		wykobi::point2d<float> tModelXUnit;
		wykobi::point2d<float> tModelYUnit;
		FiducialMarker::ComputeBasisFromSquare(tModelMarkerCorners, tModelOrigin, tModelXUnit, tModelYUnit);
		wykobi::quadix<float, 2> tQuadCorners;
		for (int i = 0; i < 4; ++i) {
			tQuadCorners[i] = tModelOrigin + mQuadModel.getQuad()[i].x*tModelXUnit + mQuadModel.getQuad()[i].y*tModelYUnit;
		}

		wykobi::quadix<float, 2> tMirroredQuadCorners = wykobi::mirror(tQuadCorners, tAxisPoints);

		wykobi::quadix<float, 2> tDstAxisMarkerCorners = mDstAxisModel.getMarker().getCorners();
		mDecoratorManager.GetCam2World().InterpolatedMapOnQuad(tDstAxisMarkerCorners);
		float tHomography[9];
		std::cout << tOrgAxisMarkerCorners[0].x  << " " << tOrgAxisMarkerCorners[0].y << "->";
		CvWykobiBridge::findHomography(tOrgAxisMarkerCorners, tDstAxisMarkerCorners, tHomography);
		CvWykobiBridge::applyHomography(tHomography, tQuadCorners);
		CvWykobiBridge::applyHomography(tHomography, tMirroredQuadCorners);
		std::cout << tDstAxisMarkerCorners[0].x << " " << tDstAxisMarkerCorners[0].y << std::endl;

//		for (int i = 0; i < 4; ++i) {
//			tMirroredQuadCorners[i] = tDstAxisMarkerCorners[0] + tMirroredQuadCorners[i] - tOrgAxisMarkerCorners[0];
//		}


		mDecoratorManager.GetWorld2Proj().InterpolatedMapOnQuad(tQuadCorners);
		mDecoratorManager.GetWorld2Proj().InterpolatedMapOnQuad(tMirroredQuadCorners);
		for (int i = 0; i < 4; ++i) {
			mDecoratorManager.GetDisplay().RenderLine(
					tQuadCorners[i].x, tQuadCorners[i].y,
					tQuadCorners[(i+1)%4].x, tQuadCorners[(i+1)%4].y,
					0.0f, 0.0f, 0.0f);
			if (!mVerso.isPresent())
				mDecoratorManager.GetDisplay().RenderLine(
						tMirroredQuadCorners[i].x, tMirroredQuadCorners[i].y,
						tMirroredQuadCorners[(i+1)%4].x, tMirroredQuadCorners[(i+1)%4].y,
						0.0f, 0.0f, 0.0f);
		}
	}
}
