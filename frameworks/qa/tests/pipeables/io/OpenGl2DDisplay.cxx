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

#include <qa/pipeables/io/OpenGl2DDisplay.hpp>
#include <qa/pipeables/misc/Stopper.hpp>

#include <GL/glut.h>
class QuickTest: public Pipeable
{
    public:
        void run(){
        	glLineWidth(1.0f);
        	glColor3f(1.0f, 0.0f, 0.0f);
			glBegin(GL_LINES);
			glVertex2f(1.0f,  1.0f);
			glVertex2f(3.0f,  3.0f);
			glVertex2f(197.0f,  237.0f);
			glVertex2f(199.0f,  239.0f);

			glVertex2f( 99.0f, 119.0f);
			glVertex2f(101.0f, 121.0f);
			glVertex2f(101.0f, 119.0f);
			glVertex2f( 99.0f, 121.0f);

			glVertex2f(  0.0f, 100.0f);
			glVertex2f(  0.0f, 140.0f);
			glVertex2f(199.0f, 100.0f);
			glVertex2f(199.0f, 140.0f);
			glVertex2f( 80.0f,   0.0f);
			glVertex2f(120.0f,   0.0f);
			glVertex2f( 80.0f, 239.0f);
			glVertex2f(120.0f, 239.0f);


			glVertex2f(50.25f,  80.0f);
			glVertex2f(50.25f,  100.0f);
			glVertex2f(50.00f,  100.0f);
			glVertex2f(50.00f, 120.0f);
			glVertex2f(50.50f, 120.0f);
			glVertex2f(50.50f, 140.0f);
			glVertex2f(50.25f, 140.0f);
			glVertex2f(50.25f, 160.0f);

			glLineWidth(20.0f);
			glVertex2f(60.00f, 100.0f);
			glVertex2f(60.00f, 120.0f);

			glEnd();

			mDisplay->RenderEllipse(100.0f, 120.0f, 20.0f, 30.0f,          0.0f, 1.0f, 0.0f);
			mDisplay->RenderArc    (100.0f, 120.0f, 10.0f, 15.0f, 45, 315, 0.0f, 0.0f, 1.0f);
			mDisplay->RenderArc    (100.0f, 120.0f, 10.0f, 15.0f, 315, 45, 1.0f, 0.0f, 1.0f);
			mDisplay->PushTransformation();
			mDisplay->Rotate(30.0f, 10.0f, 80.0f);
			mDisplay->RenderText("kikoo", 10.0f, 80.0f);
			mDisplay->PopTransformation();
        }

        OpenGl2DDisplay *mDisplay;
};

int main(int , char *[])
{
	OpenGl2DDisplay *tDisplay = new OpenGl2DDisplay(0, 0, 200, 240);
	QuickTest *tQuicktest = new QuickTest();
	tQuicktest->mDisplay = tDisplay;
	tDisplay->BackgroundColor(1.0f, 1.0f, 1.0f);
	Stopper *tStopper = new Stopper(tDisplay->KeyPressed());

	*tDisplay
	| *tQuicktest
	| *tStopper
	| *tDisplay;

	tDisplay->start();

	delete tStopper;
	delete tQuicktest;
	delete tDisplay;
	return 0;
}
