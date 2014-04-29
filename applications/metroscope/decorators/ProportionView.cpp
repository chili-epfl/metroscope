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
		return new decorators::ProportionView(pDecoratorManager,
				pDecoratorManager.loadMarker(pSetting["marker"]),
				(AngleModel *) pDecoratorManager.loadDecorator(pSetting["angle"]),
				(DenominatorsModel *)pDecoratorManager.loadDecorator(pSetting["equalparts"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::ProportionView::ProportionView(DecoratorManager & pDecoratorManager, FiducialMarker * pMarker, AngleModel * pAngleModel, DenominatorsModel * pDenomModel):
		FiducialDecorator(pDecoratorManager, pMarker),
		mAngleModel (pAngleModel),
		mDenomModel (pDenomModel),
		mOriginBox1X (700.0f),
		mOriginBox1Y (100.0f),
		mBoxWidth (500.0f),
		mBoxHeight (600.0f){
			mOriginBox1Y2 = mOriginBox1Y;
			mOriginBox1X4 = mOriginBox1X;
			mOriginBox1X2 = mOriginBox1X + mBoxWidth;
			mOriginBox1X3 = mOriginBox1X + mBoxWidth;
			mOriginBox1Y3 = mOriginBox1Y + mBoxHeight;
			mOriginBox1Y4 = mOriginBox1Y + mBoxHeight;

}

void decorators::ProportionView::update(){
	if(mMarker->isPresent() && mAngleModel->isPresent()){
		DrawRectangleProportion();
		DrawCircunference();

		if(mDenomModel->isAnyCardActive()){
			int tParts = mDenomModel->GetActiveCard()->GetDenominator();
			DivideCircunference(tParts);
			DivideRectangleProportion(tParts);
		}
	}
}

void decorators::ProportionView::DrawRectangleProportion(){
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

	mAngle = wykobi::oriented_vertex_angle(tWorldStartPoint, tWorldOriginPoint, tWorldEndPoint, wykobi::Clockwise);

	float mProportion = mAngle/360.0f;

	float mPartialY = mOriginBox1Y + (mOriginBox1Y4 - mOriginBox1Y)*mProportion;

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderQuad(mOriginBox1X,mOriginBox1Y,mOriginBox1X2,mOriginBox1Y2,mOriginBox1X3,mOriginBox1Y3,mOriginBox1X4,mOriginBox1Y4,0.0f,0.0f,0.0,1.0f);
	mDecoratorManager.GetDisplay().RenderQuadFilled(mOriginBox1X,mOriginBox1Y,
			mOriginBox1X2,mOriginBox1Y2,
			mOriginBox1X3,mPartialY,
			mOriginBox1X4,mPartialY,
			0.341f, 0.459f, 0.937f,1.0f); //blue

	mDecoratorManager.GetDisplay().RenderQuadFilled(mOriginBox1X,mPartialY,
			mOriginBox1X2,mPartialY,
			mOriginBox1X3,mOriginBox1Y3,
			mOriginBox1X4,mOriginBox1Y4,
			1.0f, 1.0f, 0.392f,1.0f); //yellow

	mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::ProportionView::DrawCircunference(){
	wykobi::quadix<float ,2> tMarkerCorners = mAngleModel->getMarker().getCorners();
	wykobi::point2d<float> mStartPoint = wykobi::centroid(tMarkerCorners[0],tMarkerCorners[1]);

	wykobi::point2d<float> tWorldStartPoint = mStartPoint;
	mDecoratorManager.GetCam2World().InterpolatedMap(tWorldStartPoint);

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(mAngleModel->getMarker(),mDecoratorManager.GetCam2World(),mDecoratorManager.GetWorld2Proj());
	mDecoratorManager.GetDisplay().RenderEllipse(0.0,0.0,5.1f,5.1f,0.0f,0.0f,0.0f,1.0f,1);
	mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::ProportionView::DivideCircunference(int pParts){
	float mRadio = 5.1f;
	double tAngle = (double)(360.0/pParts);
	double tPartialAngle = 0.0f;

		for(int i = 0 ; i < pParts ; i++){

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(mAngleModel->getMarker(),mDecoratorManager.GetCam2World(),mDecoratorManager.GetWorld2Proj());


		tPartialAngle += tAngle;

		float tPartialX = mRadio*wykobi::sin(tPartialAngle*wykobi::PI/180);
		float tPartialY = -mRadio*wykobi::cos(tPartialAngle*wykobi::PI/180);
		mDecoratorManager.GetDisplay().RenderLine(0.0f,0.0f,tPartialX,tPartialY,0.0f,0.0f,0.0f,1.0f);
		mDecoratorManager.GetDisplay().PopTransformation();
	}
}

void decorators::ProportionView::DivideRectangleProportion(int pParts){

	float tHeigth = (float)(mBoxHeight/pParts);
	float tPartialY = mOriginBox1Y;

	mDecoratorManager.GetDisplay().PushTransformation();

	for(int i = 1 ; i < pParts ; i++){
		tPartialY += tHeigth;
		mDecoratorManager.GetDisplay().RenderLine(mOriginBox1X, tPartialY, mOriginBox1X2,tPartialY, 0.0f,0.0f,0.0f,1.0f);
	}

	mDecoratorManager.GetDisplay().PopTransformation();
}

