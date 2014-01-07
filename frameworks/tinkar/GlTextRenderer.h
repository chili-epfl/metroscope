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



#ifndef _GL_TEXT_RENDERER_H
#define _GL_TEXT_RENDERER_H

#ifdef WIN32
#include <windows.h>		// Header File For Windows
#endif

//#include <GL/gl.h>	// Header File For The OpenGL32 Library
#include <GL/glu.h>	// Header File For The GLu32 Library

#include <FTGL/ftgl.h>


class GlTextRenderer
{
public:
	static GlTextRenderer *GetInstance();

	bool Init(int nRenderWidth, int nRenderHeight);
	void SetRenderSize(int w, int h) { m_nRenderWidth=w; m_nRenderHeight=h; }

	/**
	 * Render text using a bitmap font. This means that the text can not be rotated nor scaled.
	 */
	void RenderBitmapText(int x, int y, int textSize, const char *s, ...);

	/**
	 *	Render text using a textured font. This is useful if the text has to be rotated/translated.
	 */
	void RenderTexturedText(int x, int y, const char *s, ...);
	void RenderTexturedText(const char *s, ...);

	/**
	 * WARNING: changing the size of the text takes INCREDIBLY much time -> use this method only if absolutely necessary
	 * (glScalef might be much faster but you loose quality)
	 */
	void RenderTexturedText(int x, int y, int textSize, const char *s, ...);

	void SetTextSize(int size);
	int GetTextSize() { return m_nDefaultTextSize; }

private:
	GlTextRenderer(void);
	~GlTextRenderer(void);

	void RenderTexturedText(FTFont *font, int x, int y, int textSize, const char *s);
	void RenderBitmapText(FTFont *font, int x, int y, int textSize, const char *s);

private:
	static GlTextRenderer *m_ptInstance;

	bool m_bInitialized;

	int m_nDefaultTextSize;
	int m_nRenderWidth, m_nRenderHeight;

	char m_textBuffer[1024];

	FTBitmapFont *m_fontBitmap;
	FTPolygonFont *m_fontPoly;
};

#endif
