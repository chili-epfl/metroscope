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

#ifndef OpenGl2DDisplay_HPP
#define OpenGl2DDisplay_HPP

#include "../Pipeable.hpp"
#include "../../components/vision/FiducialMarker.hpp"
#include "../../components/vision/PlaneCoordinateMapper.hpp"
#include <vector>

class OpenGl2DDisplay: public Pipeable
{
    public:
        OpenGl2DDisplay(int pDisplayX, int pDisplayY, int pDisplayWidth, int pDisplayHeight);
        OpenGl2DDisplay *BackgroundColor(float pR, float pG, float pB, float pA = 1.0f);

        const char *KeyPressed(){return &mKeyPressed;}

    protected:
        void run();
        char mKeyPressed;

		const int mWidth;
		const int mHeight;

    public:
		int GetWidth() {return mWidth;}
		int GetHeight() {return mHeight;}

        void PushTransformation();
        void Translate(float pX, float pY);
        void Rotate(float pAngle, float pCenterX = 0.0f, float pCenterY = 0.0f);
        void Scale(float pX, float pY);
        void TransformToMarkersLocalCoordinates(const FiducialMarker &pMarker,
        		const PlaneCoordinateMapper & pCam2World,
        		const PlaneCoordinateMapper & pWorld2Proj);

        void TransformToMarkersLocalCoordinatesFixed(const FiducialMarker &pMarker, const float pMarkerWidth, const float pMarkerHeight,
        		const PlaneCoordinateMapper & pCam2World,
        		const PlaneCoordinateMapper & pWorld2Proj);

        void PopTransformation();

        void calculateLocalProjectorDimensionsFromRealWorldSize(float pRealWorldMarkerWidth, float pRealWorldMarkerHeight,
       																	wykobi::point2d<float> pCenter, float &tActualXUnit, float &tActualYUnit,
       																	const PlaneCoordinateMapper & pWorld2Proj);


		void RenderText(
				const char *pText,
				float x, float y,
				float scale = 1.0f,
				float r=0.0f, float g=0.0f, float b=0.0f, float a=1.0f);

		void RenderCenteredText(
						const char *pText,
						float x, float y,
						bool centerY,
						float scale,
						float r, float g, float b, float a);

		void RenderCenteredTextFixedWidth(
						const char *pText, const char *pDelimiters,
						float x, float y,
						float width,
						bool centerY,
						float scale,
						float r, float g, float b, float a);

		void RenderEllipse(
				float x, float y,
				float rx, float ry,
				float r, float g, float b, float a=1.0f,
				int step = 5);
		void RenderFilledEllipse(
				float x, float y,
				float rx, float ry,
				float r, float g, float b, float a=1.0f,
				int step = 5);

		/*Fills a sector of a given proportion of a circle in degrees, starting from the positive x-axis and
			rotating counter-clockwise. The rotation parameter will rotate the sector a given number of degrees
			in the counter-clockwise direction*/
		void RenderFilledSector(
				float x, float y,
				float rx, float ry, float degrees, float rotation,
				float r, float g, float b, float a,
				int step = 5);

		void RenderBracket(
				float x, float y,
				float width, float height,
				int rotation,
				float r, float g, float b, float a,
				int step = 5);
		void RenderArc(
				float x, float y,
				float rx, float ry,
				int startAngle, int endAngle,
				float r, float g, float b, float a=1.0f,
				int step = 5);
		void RenderArc(
				float x, float y,
				float rx, float ry,
				float startAngle, float endAngle,
				float r, float g, float b, float a=1.0f,
				int step = 5)
		{RenderArc(x, y, rx, ry, (int) (startAngle+0.5f), (int) (endAngle+0.5f), r, g, b, a, step);}

		void RenderLine(
				float x1, float y1,
				float x2, float y2,
				float r, float g, float b, float a = 1.0f);

		void RenderCross(float x, float y,
				float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f,
				float l = 10.0f);

		void RenderQuad(
				float x1, float y1,
				float x2, float y2,
				float x3, float y3,
				float x4, float y4,
				float r, float g, float b, float a = 1.0f);

		void RenderQuadFilled(
				float x1, float y1,
				float x2, float y2,
				float x3, float y3,
				float x4, float y4,
				float r, float g, float b, float a = 1.0f);

		void RenderPolygon(std::vector<float> vertices,
				float r, float g, float b, float a);
		void RenderPolygonFilled(std::vector<float> vertices,
				float r, float g, float b, float a);

		int LoadTexture(const char *pImagePath);
		void ReleaseTexture(unsigned int pTextureId);
		void RenderTexture(unsigned int pTextureId,
				float pTLx, float pTLy,
				float pTRx, float pTRy,
				float pBRx, float pBRy,
				float pBLx, float pBLy,
				int pSubtextureIndex = 0, int pNSubtextures = 1);
		void RenderTexture(unsigned int pTextureId,
				const wykobi::polygon<float, 2> & pMask,
				const wykobi::polygon<float, 2> & pDestination);

		float GetTextWidth(const char *pText, float scale, size_t pNChars);
		float GetTextHeight(const char *pText, float scale);
};

#endif
