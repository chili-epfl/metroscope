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
		return new decorators::ProportionView(pDecoratorManager,pDecoratorManager.loadMarker(pSetting["marker"]),
				(AngleModel *) pDecoratorManager.loadDecorator(pSetting["angle"]),
				(DenominatorsModel *)pDecoratorManager.loadDecorator(pSetting["equalparts"]),
				(Flipper *)pDecoratorManager.loadDecorator(pSetting["flipper"]),
				(TokenModel *)pDecoratorManager.loadDecorator(pSetting["tokens"]),
				(RectangleFractionModel *)pDecoratorManager.loadDecorator(pSetting["rectangle"]),
				(Carte *)pDecoratorManager.loadDecorator(pSetting["carte"]));
	} catch(libconfig::SettingNotFoundException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Marker parameter not found: " << e.getPath() << std::endl;
	} catch(libconfig::SettingTypeException &e) {
		std::cerr << "Failed to load " << scDecoratorName << ". Wrong type for marker parameter: " << e.getPath() << std::endl;
	}
	return 0;
}

decorators::ProportionView::ProportionView(DecoratorManager & pDecoratorManager,FiducialMarker * pMarker, AngleModel * pAngleModel, DenominatorsModel * pDenomModel, Flipper * pFlipper, TokenModel * pTokenModel, RectangleFractionModel *pRectangleModel, Carte *pCarte):
		FiducialDecorator(pDecoratorManager, pMarker),
		mAngleModel (pAngleModel),
		mDenomModel (pDenomModel),
		mFlipper(pFlipper),
		mTokenModel(pTokenModel),
		mRectangleModel(pRectangleModel),
		mCarte(pCarte),
		mOriginBox1X (700.0f),
		mOriginBox1Y (100.0f),
		mBoxWidth (500.0f),
		mBoxHeight (600.0f),
		mAngle(0.0f),
		mProportion(0.0f){
			mOriginBox1Y2 = mOriginBox1Y;
			mOriginBox1X4 = mOriginBox1X;
			mOriginBox1X2 = mOriginBox1X + mBoxWidth;
			mOriginBox1X3 = mOriginBox1X + mBoxWidth;
			mOriginBox1Y3 = mOriginBox1Y + mBoxHeight;
			mOriginBox1Y4 = mOriginBox1Y + mBoxHeight;

}

void decorators::ProportionView::update(){
	if(mMarker->isPresent()) {
		//DrawSaveRectangles();

		////
		//int mTextureId = mDecoratorManager.GetDisplay().LoadTexture("/home/daniela/Downloads/Bug.jpg");
		//float mHalfImageWidth = (float)(256/2);
		//float mHalfImageHeight = mHalfImageWidth;
		//mDecoratorManager.GetDisplay().PushTransformation();
		//mDecoratorManager.GetDisplay().RenderTexture(mTextureId,
		//		100.0f,200.0f,200.0f,200.0f,200.0f,400.0f,100.0f,400.0f);
		/*
		mDecoratorManager.GetDisplay().RenderTexture(mTextureId,
						-mHalfImageWidth,  mHalfImageHeight,
						 mHalfImageWidth,  mHalfImageHeight,
						 mHalfImageWidth, -mHalfImageHeight,
						-mHalfImageWidth, -mHalfImageHeight);
		*/
		//mDecoratorManager.GetDisplay().PopTransformation();
		////

		if(mAngleModel->isPresent()){
			//DrawRectangleProportion();
			DrawCircunference();
		}

		/*if(mDenomModel->isAnyCardActive()){
			int tParts = mDenomModel->GetActiveCard()->GetDenominator();
			DivideCircunference(tParts);
			DivideRectangleProportion(tParts);
			if(mFlipper->IsPresent()){
				SaveFraction();
			}
		}*/

		if(mTokenModel->isPresent()){
			float tNum =mTokenModel->getActiveTokens(0);
			float tDen = mTokenModel->getTotalTokens();
			float tProportion = (float)(tNum/tDen);
			ShowProportion(tProportion,mTokenModel->getMarker().getCenter());
		}

		if(mRectangleModel->isPresent()){
			ShowProportion(mRectangleModel->proportion(), mRectangleModel->getMarker().getCenter());
		}

		if(mCarte->isPresent()){
			DisplayMap();
		}
	}
}
void decorators::ProportionView::ShowProportion(float pProportion,wykobi::point2d<float> pPosition){
	mDecoratorManager.GetDisplay().PushTransformation();
	char proportion[10];
	sprintf(proportion, "%f", pProportion);
	if(pPosition.x + 20.0f < mDecoratorManager.GetDisplay().GetWidth() && pPosition.y + 20.0f < mDecoratorManager.GetDisplay().GetHeight()){
		mDecoratorManager.GetDisplay().RenderText(proportion, pPosition.x + 20.0f, pPosition.y + 20.0f, 1.0f,0.0f,0.0f,0.0f,1.0f);
	}
	mDecoratorManager.GetDisplay().PopTransformation();
}
void decorators::ProportionView::DisplayMap(){

	//DrawGrid
	//DrawWorkingArea
	int tDisplayWidth = mDecoratorManager.GetDisplay().GetWidth();
	int tDisplayHeight = mDecoratorManager.GetDisplay().GetHeight();
	float tRadius = tDisplayHeight/4;

	if(tDisplayWidth > tDisplayHeight){
		tDisplayWidth = tDisplayHeight;
	}else{
		tDisplayHeight = tDisplayWidth;
	}

	float tXCellDimension = (float)((tDisplayWidth-2*(tRadius/sqrt(2)))/mCarte->getSize());
	float tYCellDimension = (float)((tDisplayHeight-2*(tRadius/sqrt(2)))/mCarte->getSize());
	float tActualX = tRadius/sqrt(2);
	float tActualY = tRadius/sqrt(2);

	mDecoratorManager.GetDisplay().PushTransformation();
	for(int i = 0 ; i < mCarte->getSize() + 1 ; i++){
		mDecoratorManager.GetDisplay().RenderLine(tRadius/sqrt(2),tActualY,tDisplayWidth - tRadius/sqrt(2),tActualY,0.0f,0.0f,0.0f,1.0f); //horizontalLine
		mDecoratorManager.GetDisplay().RenderLine(tActualX,tRadius/sqrt(2),tActualX,tDisplayHeight - tRadius/sqrt(2),0.0f,0.0f,0.0f,1.0f); //verticalLine
		tActualX += tXCellDimension;
		tActualY += tYCellDimension;
	}
	mDecoratorManager.GetDisplay().PopTransformation();



	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderFilledEllipse(0.0f,0.0f,tRadius,tRadius,1.0f,1.0f,1.0f,1.0f,1);
	mDecoratorManager.GetDisplay().RenderFilledEllipse((float)tDisplayWidth,0.0f,tRadius,tRadius,1.0f,1.0f,1.0f,1.0f,1);
	mDecoratorManager.GetDisplay().RenderFilledEllipse((float)tDisplayWidth-1.0f,(float)tDisplayHeight-1.0f,tRadius,tRadius,1.0f,1.0f,1.0f,1.0f,5);
	mDecoratorManager.GetDisplay().RenderFilledEllipse(0.0f,(float)tDisplayHeight-1.0f,tRadius,tRadius,1.0f,1.0f,1.0f,1.0f,5);
	mDecoratorManager.GetDisplay().RenderEllipse(0.0f,0.0f,tRadius,tRadius,0.0f,0.0f,0.0f,1.0f,1);
	mDecoratorManager.GetDisplay().RenderEllipse((float)tDisplayWidth,0.0f,tRadius,tRadius,0.0f,0.0f,0.0f,1.0f,1);
	mDecoratorManager.GetDisplay().RenderEllipse((float)tDisplayWidth-1.0f,(float)tDisplayHeight-1.0f,tRadius,tRadius,0.0f,0.0f,0.0f,1.0f,1);
	mDecoratorManager.GetDisplay().RenderEllipse(0.0f,(float)tDisplayHeight-1.0f,tRadius,tRadius,0.0f,0.0f,0.0f,1.0f,1);

	mDecoratorManager.GetDisplay().PopTransformation();


	mDecoratorManager.GetDisplay().PushTransformation();
	wykobi::point2d<float> tOrigin = mCarte->getOriginPoint();
	float tBugPositionX = tRadius/sqrt(2) + (tOrigin.x)*(tXCellDimension)+(tXCellDimension/2);
	float tBugPositionY = tRadius/sqrt(2) + (tOrigin.y)*(tYCellDimension)+(tYCellDimension/2);
	mDecoratorManager.GetDisplay().RenderFilledEllipse(tBugPositionX, tBugPositionY, tXCellDimension/6,tYCellDimension/6,1.0f,0.0f,0.0f,1.0f,1);
	mDecoratorManager.GetDisplay().PopTransformation();

	mDecoratorManager.GetDisplay().PushTransformation();
	std::vector<wykobi::point2d<float>> tEnd = mCarte->getEndPoint();

	for(int i = 0 ; i < mCarte->getEndNumber() ; i++){
		float tEndPositionX =tRadius/sqrt(2)+ tEnd[i].x*(tXCellDimension)+(tXCellDimension/2);
		float tEndPositionY = tRadius/sqrt(2) + tEnd[i].y*(tYCellDimension)+(tYCellDimension/2);
		mDecoratorManager.GetDisplay().RenderFilledEllipse(tEndPositionX, tEndPositionY, tXCellDimension/2,tYCellDimension/2,0.0f,0.0f,0.0f,1.0f,1);

	}

	mDecoratorManager.GetDisplay().PopTransformation();

	mDecoratorManager.GetDisplay().PushTransformation();
	if(mCarte->getObstacleNumber() > 0){
		std::vector<wykobi::point2d<float>> tObstacle = mCarte->getObstaclesPoint();

			for(int i = 0 ; i < mCarte->getObstacleNumber() ; i++){
				float tEndPositionX =tRadius/sqrt(2)+ tObstacle[i].x*(tXCellDimension);
				float tEndPositionY = tRadius/sqrt(2) + tObstacle[i].y*(tYCellDimension);
				mDecoratorManager.GetDisplay().RenderQuadFilled(tEndPositionX,tEndPositionY,tEndPositionX + tXCellDimension,tEndPositionY,tEndPositionX+tXCellDimension, tEndPositionY + tYCellDimension, tEndPositionX, tEndPositionY + tYCellDimension,0.25f,0.25f,0.25f,1.0f);
			}
	}
	mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::ProportionView::ShowSquare(){
	wykobi::point2d<float> tPoint = mMarker->getCenter();
	mDecoratorManager.GetCam2World().InterpolatedMap(tPoint);

	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().TransformToMarkersLocalCoordinates(*mMarker,mDecoratorManager.GetCam2World(),mDecoratorManager.GetWorld2Proj());
	mDecoratorManager.GetDisplay().RenderQuad(10.0f,0.0f, 30.0f,0.0f, 30.0f, 20.0f, 10.0f, 20.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::ProportionView::ShowProportion(float pProportion){

	//mDecoratorManager.GetDisplay().PushTransformation();

	float mPartialY = mOriginBox1Y + (mOriginBox1Y4 - mOriginBox1Y)*pProportion;

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().RenderQuad(mOriginBox1X,mOriginBox1Y,mOriginBox1X2,mOriginBox1Y2,mOriginBox1X3,mOriginBox1Y3,mOriginBox1X4,mOriginBox1Y4,0.0f,0.0f,0.0,1.0f);
		mDecoratorManager.GetDisplay().RenderQuadFilled(mOriginBox1X,mOriginBox1Y,
				mOriginBox1X2,mOriginBox1Y2,
				mOriginBox1X3,mPartialY,
				mOriginBox1X4,mPartialY,
				1.0f, 1.0f, 0.392f,1.0f); //blue

		mDecoratorManager.GetDisplay().RenderQuadFilled(mOriginBox1X,mPartialY,
				mOriginBox1X2,mPartialY,
				mOriginBox1X3,mOriginBox1Y3,
				mOriginBox1X4,mOriginBox1Y4,
				0.341f, 0.459f, 0.937f,1.0f); //yellow

		mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::ProportionView::ShowProportion(int pNumerator, int pDenominator){
	float tProportion = (pNumerator/(float)pDenominator);
	mDecoratorManager.GetDisplay().PushTransformation();

	float mPartialY = mOriginBox1Y + (mOriginBox1Y4 - mOriginBox1Y)*tProportion;

		mDecoratorManager.GetDisplay().PushTransformation();
		mDecoratorManager.GetDisplay().RenderQuad(mOriginBox1X,mOriginBox1Y,mOriginBox1X2,mOriginBox1Y2,mOriginBox1X3,mOriginBox1Y3,mOriginBox1X4,mOriginBox1Y4,0.0f,0.0f,0.0,1.0f);
		mDecoratorManager.GetDisplay().RenderQuadFilled(mOriginBox1X,mOriginBox1Y,
				mOriginBox1X2,mOriginBox1Y2,
				mOriginBox1X3,mPartialY,
				mOriginBox1X4,mPartialY,
				1.0f, 1.0f, 0.392f,1.0f); //blue

		mDecoratorManager.GetDisplay().RenderQuadFilled(mOriginBox1X,mPartialY,
				mOriginBox1X2,mPartialY,
				mOriginBox1X3,mOriginBox1Y3,
				mOriginBox1X4,mOriginBox1Y4,
				0.341f, 0.459f, 0.937f,1.0f); //yellow

		mDecoratorManager.GetDisplay().PopTransformation();
}

void decorators::ProportionView::SaveFraction(){
	if(mFlipper->IsFlipped()){
		//int tDenominator = mDenomModel->GetActiveCard()->GetDenominator();
		//int tNumerator = (int)tDenominator*mProportion;

	}
}
/*
 * void decorators::FractionHint::drawPieChart(int pNumerator,int pDenominator,
												float pX, float pY, float pRadius,
												const color *pFillColor, const color *pOutlineColor){



	int tNumUnitBlocks = pNumerator/pDenominator + (pNumerator%pDenominator==0?0:1);
	if(tNumUnitBlocks == 0 && pNumerator == 0) tNumUnitBlocks = 1;
	float tOffset = .5-((float)tNumUnitBlocks)/2;
	for (int i= 0; i < tNumUnitBlocks; i++){
		mDecoratorManager.GetDisplay().RenderEllipse(pX + (tOffset+i)*scHINT_GRAPHICS_ICONS_WIDTH*2, pY, pRadius, pRadius, pOutlineColor->r, pOutlineColor->g, pOutlineColor->b, pOutlineColor->a);
		if(i == tNumUnitBlocks-1){
			int tNumerator = pNumerator%pDenominator == 0? pDenominator: pNumerator%pDenominator;
			mDecoratorManager.GetDisplay().RenderFilledSector(pX + (tOffset+i)*scHINT_GRAPHICS_ICONS_WIDTH*2, pY, pRadius, pRadius, scDEGREES_IN_CIRCLE*((float)tNumerator/pDenominator), 0.0f, pFillColor->r, pFillColor->g, pFillColor->b, pFillColor->a);
		}
		else{
			mDecoratorManager.GetDisplay().RenderFilledSector(pX + (tOffset+i)*scHINT_GRAPHICS_ICONS_WIDTH*2, pY, pRadius, pRadius, scDEGREES_IN_CIRCLE, 0.0f, pFillColor->r, pFillColor->g, pFillColor->b, pFillColor->a);
		}
	}
}
 */
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

	mProportion = mAngle/360.0f;

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

void decorators::ProportionView::DrawSaveRectangles(){
	mDecoratorManager.GetDisplay().PushTransformation();
	mDecoratorManager.GetDisplay().RenderQuad(100.0f,720.0f,1200.0f,720.0f,1200.0f,1000.0f,100.0f,1000.0f,0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(650.0f, 720.0f, 650.0f,1000.0f, 0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(375.0f, 720.0f, 375.0f,1000.0f, 0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().RenderLine(925.0f, 720.0f, 925.0f,1000.0f, 0.0f,0.0f,0.0f,1.0f);
	mDecoratorManager.GetDisplay().PopTransformation();
}
