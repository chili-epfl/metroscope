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

#include "OpenGl2DDisplay.hpp"
#include <tinkar/GlTextRenderer.h>
#include <iostream>
#include <GL/glut.h>
#include <SDL/SDL.h>
#include <math.h>
#include <opencv2/highgui/highgui.hpp>

#ifndef GL_BGR
#define GL_BGR GL_BGR_EXT
#endif

OpenGl2DDisplay::OpenGl2DDisplay(int pDisplayX, int pDisplayY, int pDisplayWidth, int pDisplayHeight):
mKeyPressed(0),
mWidth(pDisplayWidth),
mHeight(pDisplayHeight)
{
	char tWindowPosition[21+4+1+4+1];
#ifdef __STDC_SECURE_LIB__
	sprintf_s(tWindowPosition, "SDL_VIDEO_WINDOW_POS=%d,%d", pDisplayX, pDisplayY);
	_putenv(tWindowPosition);
#else
	sprintf(tWindowPosition, "SDL_VIDEO_WINDOW_POS=%d,%d", pDisplayX, pDisplayY);
	putenv(tWindowPosition);
#endif
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) SDL_Quit();
	if ( !SDL_SetVideoMode(mWidth, mHeight, 0, SDL_OPENGL | SDL_NOFRAME) ) SDL_Quit();
	SDL_WM_SetCaption("OpenGLDisplay", NULL);

	//Init OpenGL parameters
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, mWidth, mHeight, 0, 0, 1);
//	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//Displacement trick for exact pixelization
	glTranslatef(0.5f, 0.5f, 0.0f);

//	glDepthFunc(GL_LESS);									// The Type Of Depth Test To Do
//	glEnable(GL_DEPTH_TEST);								// Enables Depth Testing
	glShadeModel(GL_SMOOTH);								// Enables Smooth Color Shading
	glClearDepth(1.0);										// Enables Clearing Of The Depth Buffer
	glEnable(GL_BLEND);										// Turn Blending On
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);								// Set Line Antialiasing
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	int argc = 0;
	glutInit (&argc, 0);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);

	GlTextRenderer::GetInstance()->Init(mWidth, mHeight);
	GlTextRenderer::GetInstance()->SetTextSize(10);
}

OpenGl2DDisplay *OpenGl2DDisplay::BackgroundColor(float pR, float pG, float pB, float pA)
{
	glClearColor(pR, pG, pB, pA);
	return this;
}


void OpenGl2DDisplay::run()
{
	SDL_GL_SwapBuffers();
	SDL_Event tEvent;
	mKeyPressed = 0;
	while (SDL_PollEvent(&tEvent))
	{
		switch (tEvent.type) {
		case SDL_KEYDOWN:
			mKeyPressed = (char) tEvent.key.keysym.sym;
			break;
		}
	}
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGl2DDisplay::PushTransformation()
{
	glPushMatrix();
}

void OpenGl2DDisplay::Translate(float pX, float pY)
{
	glTranslatef(pX, pY, 0.0f);
}

void OpenGl2DDisplay::Rotate(float pAngle, float pCenterX, float pCenterY)
{
	glTranslatef(pCenterX, pCenterY, 0.0f);
	glRotatef(-pAngle, 0.0f, 0.0f, 1.0f);
	glTranslatef(-pCenterX, -pCenterY, 0.0f);
}

void OpenGl2DDisplay::Scale(float pX, float pY)
{
	glScalef(pX, pY, 1.0f);
}

void OpenGl2DDisplay::TransformToMarkersLocalCoordinates(const FiducialMarker &pMarker,
		const PlaneCoordinateMapper & pCam2World,
		const PlaneCoordinateMapper & pWorld2Proj)
{
	wykobi::quadix<float, 2> tCorners = pMarker.getCorners();
	std::cout << "Transform Corners " << tCorners.size() << std::endl;
	if(tCorners.size()<4) return;//The markers had disappeared, we just do nothing
	pCam2World.InterpolatedMapOnQuad(tCorners);
	wykobi::point2d<float> tCenter;
	wykobi::vector2d<float> tXUnit;
	wykobi::vector2d<float> tYunit;
	FiducialMarker::ComputeBasisFromSquare(tCorners, tCenter, tXUnit, tYunit);
	wykobi::point2d<float> tRight = tCenter+tXUnit;
	wykobi::point2d<float> tTop = tCenter+tYunit;
	pWorld2Proj.InterpolatedMap(tCenter);
	pWorld2Proj.InterpolatedMap(tRight);
	pWorld2Proj.InterpolatedMap(tTop);
	glTranslatef(tCenter.x, tCenter.y, 0.0f);
	glRotatef(wykobi::cartesian_angle(tRight-tCenter), 0.0f, 0.0f, 1.0f);
	glScalef(wykobi::distance(tCenter, tRight), wykobi::orientation(tTop, tRight, tCenter)*wykobi::distance(tCenter, tTop), 1.0f);
}

//The scale is adjusted to be based on the size of the marker. If the marker is flat on the table, the scale will be approx. 1.0.
void OpenGl2DDisplay::TransformToMarkersLocalCoordinatesFixed(const FiducialMarker &pMarker, const float pMarkerWidth, const float pMarkerHeight,
               		const PlaneCoordinateMapper & pCam2World,
               		const PlaneCoordinateMapper & pWorld2Proj){
	wykobi::quadix<float, 2> tCorners = pMarker.getCorners();
		pCam2World.InterpolatedMapOnQuad(tCorners);
		wykobi::point2d<float> tCenter;
		wykobi::vector2d<float> tXUnit;
		wykobi::vector2d<float> tYunit;
		FiducialMarker::ComputeBasisFromSquare(tCorners, tCenter, tXUnit, tYunit);
		float  tActualXUnit;
		float tActualYUnit;
		calculateLocalProjectorDimensionsFromRealWorldSize(pMarkerWidth, pMarkerHeight, tCenter, tActualXUnit, tActualYUnit, pWorld2Proj);
		wykobi::point2d<float> tRight = tCenter+tXUnit;
		wykobi::point2d<float> tTop = tCenter+tYunit;
		pWorld2Proj.InterpolatedMap(tCenter);
		pWorld2Proj.InterpolatedMap(tRight);
		pWorld2Proj.InterpolatedMap(tTop);
		glTranslatef(tCenter.x, tCenter.y, 0.0f);
		glRotatef(wykobi::cartesian_angle(tRight-tCenter), 0.0f, 0.0f, 1.0f);
		glScalef(wykobi::distance(tCenter, tRight)/tActualXUnit, wykobi::orientation(tTop, tRight, tCenter)*wykobi::distance(tCenter, tTop)/tActualYUnit, 1.0f);


}



void OpenGl2DDisplay::calculateLocalProjectorDimensionsFromRealWorldSize(float pRealWorldMarkerWidth, float pRealWorldMarkerHeight,
																				wykobi::point2d<float> pCenter, float &tActualXUnit, float &tActualYUnit,
																				const PlaneCoordinateMapper & pWorld2Proj){
		wykobi::quadix<float,2> tMarkerSquare;

		tMarkerSquare[0].x = pCenter.x - pRealWorldMarkerWidth/2;
		tMarkerSquare[0].y = pCenter.y + pRealWorldMarkerHeight/2;

		tMarkerSquare[1].x = pCenter.x + pRealWorldMarkerWidth/2;
		tMarkerSquare[1].y = pCenter.y + pRealWorldMarkerHeight/2;

		tMarkerSquare[2].x = pCenter.x + pRealWorldMarkerWidth/2;
		tMarkerSquare[2].y = pCenter.y - pRealWorldMarkerHeight/2;

		tMarkerSquare[3].x = pCenter.x - pRealWorldMarkerWidth/2;
		tMarkerSquare[3].y = pCenter.y - pRealWorldMarkerHeight/2;

		pWorld2Proj.InterpolatedMapOnQuad(tMarkerSquare);

		wykobi::vector2d<float> tXUnit;
		wykobi::vector2d<float> tYunit;
		FiducialMarker::ComputeBasisFromSquare(tMarkerSquare, pCenter, tXUnit, tYunit);
		wykobi::point2d<float> tRight = pCenter+tXUnit;
		wykobi::point2d<float> tTop = pCenter+tYunit;
		tActualXUnit = distance(pCenter, tRight);
		tActualYUnit = distance(pCenter, tTop);
}


void OpenGl2DDisplay::PopTransformation()
{
	glPopMatrix();
}


void OpenGl2DDisplay::RenderText(
		const char *pText,
		float x, float y,
		float scale,
		float r, float g, float b, float a)
{
	glColor4f(r,g,b,a);
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glScalef(2.0f*scale, -2.0f*scale, 1.0f); // text is nicer if scaled up
	GlTextRenderer::GetInstance()->RenderTexturedText(0, 0, 12, pText);
	glPopMatrix();
}


static const char scFIRST_PRINTABLE_CHAR = ' ';
static const int scNUM_PRINTABLE_CHARS = 95;

static const float scPRINTABLE_CHARACTER_DIMENSIONS[95][2] =
	{

	{7.60f, 0.00f},   {9.60f, 17.50f}, {11.00f, 17.50f}, {20.10f, 17.30f},	 //SPACE, !, ", #
	{15.22f, 18.20f}, {22.76f, 17.8f}, {21.34f, 17.70f}, {6.56f, 17.50f},	 //$, %, &, '
	{9.34f, 18.30f},  {9.34f, 18.30f}, {12.00f, 17.80f}, {20.10f, 15.00f},	 // (, ), *, +
	{7.60f, 2.50f},   {8.10f, 7.40f},  {7.60f, 2.70f},   {8.10f, 17.50f},	 // (comma), - , . , /

	{15.17f, 17.70f}, {15.17f, 17.70f}, {15.17f, 17.70f}, {15.17f, 17.70f},  //0, 1, 2, 3
	{15.17f, 17.70f}, {15.17f, 17.70f}, {15.17f, 17.70f}, {15.17f, 17.70f},  //4, 5, 6, 7
	{15.17f, 17.70f}, {15.17f, 17.70f},										 //8, 9

	{8.04f, 10.50f},  {8.04f, 10.50f},  {20.10f, 14.00f}, {20.10f, 10.80f},  // : , ; , <, =
	{20.10f, 14.00f}, {12.76f, 17.80f}, {23.98f, 16.9f}, 					 // >, ?, @

	{17.34f, 17.50f}, {17.60f, 17.50f}, {19.04f, 17.50f}, {19.22f, 17.50f},  //A, B, C, D
	{17.50f, 17.50f}, {16.62f, 17.50f}, {18.14f, 17.50f}, {19.50f, 17.50f},  //E, F, G, H
	{9.46f, 17.50f},  {9.60f, 17.50f},  {17.96f, 17.50f}, {15.92f, 17.50f},  //I, J, K, L
	{24.57f, 17.50f}, {20.96f, 17.50f}, {19.74f, 17.50f}, {16.14f, 17.50f},  //M, N, O, P
	{19.68f, 17.50f}, {18.10f, 17.50f}, {16.40f, 17.50f}, {16.00f, 17.50f},	 //Q, R, S, T
	{20.20f, 17.50f}, {17.34f, 17.50f}, {24.68f, 17.50f}, {17.10f, 17.50f},  //U, V, W, X
	{15.86f, 17.50f}, {16.68f, 17.50f},  									 //Y, Z

	{9.32f, 18.30f},  {8.10f, 17.50f},  {9.32f, 18.30f},  {20.10f, 17.50f},	 //[, \, ], ^
	{12.00f, 0.00f},  {11.90f, 19.10f},										 // _, '

	{14.28f, 12.75f}, {15.34f, 18.25f}, {13.40f, 12.75f}, {15.34f, 18.25f},	 //a, b, c, d
	{14.20f, 12.75f}, {8.90f, 18.25f},  {15.32f, 12.75f}, {15.46f, 18.25f},  //e, f, g, h
	{7.68f, 12.75f},  {7.40f, 12.75f},  {14.52f, 18.25f}, {7.68f, 18.25f},	 //i, j, k, l
	{22.70f, 12.75f}, {15.40f, 12.75f}, {14.40f, 12.75f}, {15.34f, 12.75f},	 //m, n, o, p
	{15.34f, 12.75f}, {11.48f, 12.75f}, {12.40f, 12.75f}, {9.64f, 18.25f},	 //q, r, s, t
	{15.46f, 12.75f}, {13.58f, 12.75f}, {20.50f, 12.75f}, {13.52f, 12.75f},  //u, v, w, x
	{13.58f, 12.75f}, {12.62f, 12.75f},

	{15.22f, 18.20f}, {8.04f, 18.40f},  {15.22f, 18.20f}, {20.10f, 9.40f}    // {, |, }, ~


	};


//Works great as long as you don't use weird formatting characters (%, \) or charaters other than the first 127 ASCII chars
void OpenGl2DDisplay::RenderCenteredText(
						const char *pText,
						float x, float y,
						bool centerY,					//choose true if text should be centered in the Y-direction
						float scale,
						float r, float g, float b, float a)
{

	float tShiftedX = x - GetTextWidth(pText, scale, strlen(pText))/2;
	float tShiftedY = y + (centerY ? GetTextHeight(pText, scale)/2:0);

	RenderText(pText, tShiftedX, tShiftedY, scale, r, g, b, a);
}

//Trims spaces off the end of a string
void trim(char *&str, size_t strlen){
	char *tLastChar = str + strlen-1;
	while(tLastChar >=str && *tLastChar == ' ') tLastChar--;
	*(tLastChar + 1) = '\0';
}


void OpenGl2DDisplay::RenderCenteredTextFixedWidth(
						const char * const pText, const char *pDelimiters,
						float x, float y,
						float width,
						bool centerY,
						float scale,
						float r, float g, float b, float a)
{
	char *buffer = new char[strlen(pText)+1];   //Change this to on the heap later?
	buffer[0] = '\0';
	const char *tNextWordStart = pText; //points to the char at the start of the next word
	int tNumLines = 0;
	int tNumCharsInLine = 0; //number of chars in the current line
	float tLineWidth = 0;
	const float tWidthOfSpace = GetTextWidth(" ", scale, 1);
	float tLineHeight = GetTextHeight(pText, scale)*1.25;

	while (strlen(tNextWordStart) != 0){
		size_t tWordLength = strcspn(tNextWordStart, pDelimiters) + 1;  //number of chars in the word plus the delimiting char
		float tWordWidth = GetTextWidth(tNextWordStart, scale, tWordLength);   //find the width of this new word
		bool tEndsWithSpace = *(tNextWordStart + tWordLength - 1) == ' ';
		if (tEndsWithSpace? tLineWidth + tWordWidth - tWidthOfSpace > width : tLineWidth + tWordWidth > width){
			trim(buffer, strlen(buffer));
			if(strlen(buffer) != 0){
				RenderCenteredText(buffer, x, y + tNumLines*tLineHeight, centerY, scale, r, g, b, a);
				tNumLines++;
			}
			tNumCharsInLine = 0;
			tLineWidth = 0;
		}
		snprintf((char *)buffer + tNumCharsInLine, tWordLength + 1, "%s", tNextWordStart);
		tLineWidth += tWordWidth;//
		tNumCharsInLine += tWordLength;
		tNextWordStart += ((strlen(tNextWordStart) < tWordLength)? tWordLength - 1: tWordLength);

	}
	trim(buffer, strlen(buffer));
	RenderCenteredText(buffer, x, y + tNumLines*tLineHeight, centerY, scale, r, g, b, a);
	delete [] buffer;
}



float OpenGl2DDisplay::GetTextWidth(const char *pText, float scale, size_t pNChars){
	//int tNChars = strlen(pText);
	float tTextWidth = 0.0f;
	for (unsigned int i = 0; i < pNChars; i++){
		unsigned char tCh = pText[i];
		if (tCh < scFIRST_PRINTABLE_CHAR || tCh > scFIRST_PRINTABLE_CHAR + scNUM_PRINTABLE_CHARS) break;
		tTextWidth += scPRINTABLE_CHARACTER_DIMENSIONS[tCh - scFIRST_PRINTABLE_CHAR][0];
	}
	return tTextWidth*scale;
}

float OpenGl2DDisplay::GetTextHeight(const char *pText, float scale){
	int tNChars = strlen(pText);
	float tTextHeight = 0.0f;
	float tTempTextHeight;
	for (int i = 0; i < tNChars; i++){
		unsigned char tCh = pText[i];
		if (tCh < scFIRST_PRINTABLE_CHAR || tCh > scFIRST_PRINTABLE_CHAR + scNUM_PRINTABLE_CHARS) break;
		tTempTextHeight = scPRINTABLE_CHARACTER_DIMENSIONS[tCh-scFIRST_PRINTABLE_CHAR][1];
		if (tTempTextHeight > tTextHeight) tTextHeight = tTempTextHeight;
	}
	return tTextHeight*scale;
}


void OpenGl2DDisplay::RenderEllipse(
		float x, float y,
		float rx, float ry,
		float r, float g, float b, float a,
		int step)
{
	glPushMatrix();
	glTranslatef(x, y, 0.0f);

	glColor4f(r,g,b,a);
	glBegin(GL_LINE_LOOP);
	for (int i=0; i<360; i+=step)
	{
		static const float scDeg2Rad = 3.14159f/180.0f;
		float tDegInRad = i*scDeg2Rad;
		glVertex2f(cos(tDegInRad)*rx, sin(tDegInRad)*ry);
	}
	glEnd();

	glPopMatrix();
}

void OpenGl2DDisplay::RenderFilledEllipse(
		float x, float y,
		float rx, float ry,
		float r, float g, float b, float a,
		int step)
{
	glPushMatrix();
	glTranslatef(x, y, 0.0f);

	glColor4f(r,g,b,a);
	glBegin(GL_POLYGON);
	for (int i=0; i<360; i+=step)
	{
		static const float scDeg2Rad = 3.14159f/180.0f;
		float tDegInRad = i*scDeg2Rad;
		glVertex2f(cos(tDegInRad)*rx, sin(tDegInRad)*ry);
	}
	glEnd();

	glPopMatrix();
}

void OpenGl2DDisplay::RenderFilledSector(
		float x, float y,
		float rx, float ry, float degrees, float rotation,
		float r, float g, float b, float a,
		int step)
{
	glPushMatrix();
	glTranslatef(x, y, 0.0f);

	glColor4f(r,g,b,a);
	glBegin(GL_POLYGON);
	glVertex2f(0.0f, 0.0f);
	static const float scDeg2Rad = 3.14159265f/180.0f;
	for (int i=rotation; i<(rotation+degrees); i+=step)
	{
		float tDegInRad = i*scDeg2Rad;
		glVertex2f(cos(tDegInRad)*rx, -sin(tDegInRad)*ry);
	}
	glVertex2f(cos((rotation+degrees)*scDeg2Rad)*rx, -sin((rotation+degrees)*scDeg2Rad)*ry);

	glEnd();

	glPopMatrix();
}


void OpenGl2DDisplay::RenderBracket(
		float x, float y,
		float width, float height,
		int rotation,
		float r, float g, float b, float a,
		int step)
{
	Rotate(rotation, x, y);
	RenderArc(x + width, y - height/4, width/2, height/4, 180, 270, r, g, b, a, step);
	RenderArc(x, y - height/4, width/2, height/4, 0, 90, r, g, b, a, step);
	RenderArc(x, y + height/4, width/2, height/4, 270, 360, r, g, b, a, step);
	RenderArc(x + width, y + height/4, width/2, height/4, 90, 180, r, g, b, a, step);
	Rotate(-rotation, x, y);
}



void OpenGl2DDisplay::RenderArc(
		float x, float y,
		float rx, float ry,
		int startAngle, int endAngle,
		float r, float g, float b, float a,
		int step)
{
	if (endAngle < startAngle) endAngle += 360;

	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glColor4f(r,g,b,a);
	glBegin(GL_LINES);
	static const float scDeg2Rad = 3.14159f/180.0f;
	float tDegInRad = (startAngle%360)*scDeg2Rad;
	glVertex2f(cos(tDegInRad)*rx, sin(tDegInRad)*ry);
	for (int i = startAngle+step; i < endAngle; i += step)
	{
		tDegInRad = (i%360)*scDeg2Rad;
		float tX = cos(tDegInRad)*rx;
		float tY = sin(tDegInRad)*ry;
		glVertex2f(tX,tY);
		glVertex2f(tX,tY);
	}
	tDegInRad =(endAngle%360)*scDeg2Rad;
	glVertex2f(cos(tDegInRad)*rx, sin(tDegInRad)*ry);
	glEnd();
	glPopMatrix();
}

void OpenGl2DDisplay::RenderLine(
		float x1, float y1,
		float x2, float y2,
		float r, float g, float b, float a)
{
	glColor4f(r,g,b,a);
	glBegin(GL_LINES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glEnd();
}

void OpenGl2DDisplay::RenderCross(float x, float y, float r, float g, float b, float a, float l)
{
	this->RenderLine(x-l,y,x+l,y,r,g,b,a);
	this->RenderLine(x,y-l,x,y+l,r,g,b,a);
}

void OpenGl2DDisplay::RenderQuad(
		float x1, float y1,
		float x2, float y2,
		float x3, float y3,
		float x4, float y4,
		float r, float g, float b, float a)
{
	glColor4f(r,g,b,a);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glVertex2f(x3, y3);
	glVertex2f(x4, y4);
	glEnd();
}

void OpenGl2DDisplay::RenderQuadFilled(
		float x1, float y1,
		float x2, float y2,
		float x3, float y3,
		float x4, float y4,
		float r, float g, float b, float a)
{
	glColor4f(r,g,b,a);
	glBegin(GL_QUADS);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glVertex2f(x3, y3);
	glVertex2f(x4, y4);
	glEnd();
}


int OpenGl2DDisplay::LoadTexture(const char *pImagePath)
{
	IplImage *tImage = cvLoadImage(pImagePath);
	//FIXME: width is assumed to be a multiple of whatever opencv aligns the
	//data against (most likely 4)...
	GLuint tTextureId;
	glGenTextures(1, &tTextureId);
	glBindTexture(GL_TEXTURE_2D, tTextureId);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	//..or this won't work
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, tImage->width, tImage->height,
			0, GL_BGR, GL_UNSIGNED_BYTE, tImage->imageData);
	cvReleaseImage(&tImage);
	return tTextureId;
}

void OpenGl2DDisplay::ReleaseTexture(unsigned int pTextureId)
{
	glDeleteTextures(1, &pTextureId);
}

void OpenGl2DDisplay::RenderTexture(unsigned int pTextureId,
		float pTLx, float pTLy,
		float pTRx, float pTRy,
		float pBRx, float pBRy,
		float pBLx, float pBLy,
		int pSubtextureIndex, int pNSubtextures)
{
		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glBindTexture(GL_TEXTURE_2D, pTextureId);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //GL_CLAMP
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f); //reset the colors to white and opaque .

		glBegin (GL_QUADS);
			glTexCoord2f(0.0f,     pSubtextureIndex/(float)pNSubtextures); glVertex2f(pTLx, pTLy); //glTexCoord2f(0.0f, 0.0f);
			glTexCoord2f(0.0f, (pSubtextureIndex+1)/(float)pNSubtextures); glVertex2f(pBLx, pBLy); //glTexCoord2f(0.0f, 1.0f);
			glTexCoord2f(1.0f, (pSubtextureIndex+1)/(float)pNSubtextures); glVertex2f(pBRx, pBRy); //glTexCoord2f(1.0f, 1.0f);
			glTexCoord2f(1.0f,     pSubtextureIndex/(float)pNSubtextures); glVertex2f(pTRx, pTRy); //glTexCoord2f(1.0f, 0.0f);
		glEnd();
		glDisable (GL_TEXTURE_2D);
}

void OpenGl2DDisplay::RenderTexture( unsigned int pTextureId,
		const wykobi::polygon<float, 2> & pMask,
		const wykobi::polygon<float, 2> & pDestination)
{
		glEnable(GL_TEXTURE_2D);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glBindTexture(GL_TEXTURE_2D, pTextureId);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //GL_CLAMP
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f); //reset the colors to white and opaque .

		glBegin (GL_POLYGON);
		for(size_t i = 0; i < pMask.size(); ++i) {
			glTexCoord2f(pMask[i].x, pMask[i].y); glVertex2f(pDestination[i].x, pDestination[i].y);
		}
		glEnd();
		glDisable (GL_TEXTURE_2D);
}
