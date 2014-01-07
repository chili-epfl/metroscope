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

#include "GlTextRenderer.h"

#ifdef __STDC_SECURE_LIB__
#define sprintf vsprintf_s
#endif

GlTextRenderer *GlTextRenderer::m_ptInstance=NULL;

GlTextRenderer *GlTextRenderer::GetInstance()
{
	if (m_ptInstance==NULL)
		m_ptInstance = new GlTextRenderer();
	return m_ptInstance;
}

GlTextRenderer::GlTextRenderer(void)
{
	m_bInitialized = false;
	m_nDefaultTextSize = 72;
	m_fontPoly=NULL;
	m_fontBitmap=NULL;
}

GlTextRenderer::~GlTextRenderer(void)
{
	if (m_bInitialized) {
		delete m_fontBitmap;
		delete m_fontPoly;
	}
}

bool GlTextRenderer::Init(int nRenderWidth, int nRenderHeight)
{
	m_nRenderWidth = nRenderWidth;
	m_nRenderHeight = nRenderHeight;
	#ifdef WIN32
		m_fontPoly = new FTPolygonFont("c:/windows/fonts/arial.ttf");
		if(m_fontPoly->Error()) return false;
		m_fontBitmap = new FTBitmapFont("c:/windows/fonts/arial.ttf");
		if(m_fontBitmap->Error()) {
			delete m_fontPoly;
			return false;
		}
	#else
		//m_fontPoly = new FTTextureFont("/usr/share/fonts/truetype/ttf-dejavu/DejaVuSerif.ttf");
		m_fontPoly = new FTPolygonFont("/usr/share/fonts/truetype/ttf-dejavu/DejaVuSerif.ttf");
		if(m_fontPoly->Error()) return false;
		m_fontBitmap = new FTBitmapFont("/usr/share/fonts/truetype/ttf-dejavu/DejaVuSerif.ttf");
		if(m_fontBitmap->Error()) {
			delete m_fontPoly;
			return false;
		}
	#endif
	m_fontPoly->FaceSize(m_nDefaultTextSize);
	m_fontPoly->CharMap(ft_encoding_unicode);
	m_fontBitmap->FaceSize(m_nDefaultTextSize);
	m_bInitialized = true;
	return true;
}

void GlTextRenderer::SetTextSize(int size)
{
	m_nDefaultTextSize=size;
	if (m_bInitialized) {
		m_fontPoly->FaceSize(m_nDefaultTextSize);
		m_fontBitmap->FaceSize(m_nDefaultTextSize);
	}
}

void GlTextRenderer::RenderBitmapText(int x, int y, int textSize, const char *s, ...)
{
	if(s==NULL) return;
	va_list ap;
	va_start(ap, s);
	sprintf(m_textBuffer, s, ap);
	RenderBitmapText(m_fontBitmap, x, y, textSize, m_textBuffer);
	va_end(ap);
}

void GlTextRenderer::RenderTexturedText(int x, int y, int textSize, const char *s, ...)
{
	if(s==NULL) return;
	va_list ap;
	va_start(ap, s);
	sprintf(m_textBuffer, s, ap);
	m_fontPoly->FaceSize(textSize);
	RenderTexturedText(m_fontPoly, x, y, textSize, m_textBuffer);
	m_fontPoly->FaceSize(m_nDefaultTextSize);
	va_end(ap);
}

void GlTextRenderer::RenderTexturedText(int x, int y, const char *s, ...)
{
	if(s==NULL) return;
	va_list ap;
	va_start(ap, s);
	sprintf(m_textBuffer, s, ap);
	RenderTexturedText(m_fontPoly, x, y, m_nDefaultTextSize, m_textBuffer);
	va_end(ap);
}

void GlTextRenderer::RenderTexturedText(const char *s, ...)
{
	if(s==NULL) return;
	va_list ap;
	va_start(ap, s);
	sprintf(m_textBuffer, s, ap);
	RenderTexturedText(m_fontPoly, 0, 0, m_nDefaultTextSize, m_textBuffer);
	va_end(ap);
}

void GlTextRenderer::RenderBitmapText(FTFont *font, int x, int y, int textSize, const char *s)
{
	if (!m_bInitialized || s==NULL) return;


	// set opengl projection matrices
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	gluOrtho2D(0,m_nRenderWidth,0,m_nRenderHeight);
	// set raster position
	glRasterPos2i(x,y);
	glTranslatef((GLfloat)x,(GLfloat)y,0.0f);
	// set font size
	font->FaceSize(textSize);
	// render text
	font->Render(s);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void GlTextRenderer::RenderTexturedText(FTFont *font, int x, int y, int , const char *s)
{
	if (!m_bInitialized || s==NULL) return;

	// set opengl projection matrices
	glPushMatrix();
	glTranslatef((GLfloat)x,(GLfloat)y,0.0f);
	// render text
	font->Render(s);
	glPopMatrix();
}
