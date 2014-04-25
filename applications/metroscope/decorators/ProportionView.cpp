/*
 * ProportionView.cpp
 *
 *  Created on: Apr 23, 2014
 *      Author: daniela
 */

#include "ProportionView.hpp"
#include <iostream>
#include <qa/utils/Time.hpp>
#include <wykobi/wykobi_utilities.hpp>

const std::string decorators::ProportionView::scDecoratorName("ProportionView");
const DecoratorManager::Registerer decorators::ProportionView::mRegisterer(decorators::ProportionView::scDecoratorName, &decorators::ProportionView::create);

decorators::FiducialDecorator *decorators::ProportionView::create(libconfig::Setting &pSetting, DecoratorManager &pDecoratorManager)
{
	try{
		return new decorators::ProportionView(pDecoratorManager, pDecoratorManager.loadMarker(pSetting["marker"]), (AngleModel *) pDecoratorManager.loadDecorator(pSetting["angle"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::ProportionView::ProportionView(DecoratorManager & pDecoratorManager, FiducialMarker * pMarker, AngleModel * pAngleModel):
		FiducialDecorator(pDecoratorManager, pMarker),
		mAngleModel(pAngleModel){

}

void decorators::ProportionView::update(){

	if(mMarker->isPresent() && mAngleModel->isPresent()){

		wykobi::point2d<float> mOriginPoint = mAngleModel->OriginPoint();
		wykobi::point2d<float> mEndPoint = mAngleModel->EndPoint();

		wykobi::quadix<float ,2> tMarkerCorners = mAngleModel->getMarker().getCorners();
		wykobi::point2d<float> mStartPoint = wykobi::centroid(tMarkerCorners[0],tMarkerCorners[1]);

		wykobi::point2d<float> tWorldOriginPoint = mOriginPoint;
		wykobi::point2d<float> tWorldStartPoint = mStartPoint;
		wykobi::point2d<float> tWorldEndPoint = mEndPoint;

		mDecoratorManager.GetCam2World().InterpolatedMap(tWorldOriginPoint);
		mDecoratorManager.GetCam2World().InterpolatedMap(tWorldStartPoint);
		mDecoratorManager.GetCam2World().InterpolatedMap(tWorldEndPoint);

		float mAngle = wykobi::oriented_vertex_angle(tWorldStartPoint, tWorldOriginPoint, tWorldEndPoint, wykobi::Clockwise);

		float mProportion = mAngle/360.0f;
		float mX1 = 700.0f;
		float mY1 = 100.0f;

		float mX2 = 1200.0f;
		float mY2 = 100.0f;

		float mX3 = 1200.0f;
		float mY3 = 700.0f;

		float mX4 = 700.0f;
		float mY4 = 700.0f;

		float mPartialY = mY1 + (mY4 - mY1)*mProportion;

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().RenderQuad(700.0f,100.0f,1000.0f,100.0f,1000.0f,700.0f,700.0f,700.0f,0.0f,0.0f,0.0,1.0f);
		mDecoratorManager.GetDisplay().RenderQuadFilled(mX1,mY1,
				mX2,mY2,
				mX3,mPartialY,
				mX4,mPartialY,
				0.3410f, 0.4590f, 0.9370f,1.0f); //blue

		mDecoratorManager.GetDisplay().RenderQuadFilled(mX1,mPartialY,
				mX2,mPartialY,
				mX3,mY3,
				mX4,mY4,
				1.0000f, 1.0000f, 0.3920f,1.0f); //yellow

		mDecoratorManager.GetDisplay().PopTransformation();

	}
}


