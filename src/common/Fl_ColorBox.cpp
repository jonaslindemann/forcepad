//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2007 Division of Structural Mecahnics, Lund University
//
// Written by Jonas Lindemann
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
// Comments and suggestions to jonas.lindemann@byggmek.lth.se
//

// Fl_Gl_ColorBox.cpp: implementation of the Fl_Gl_ColorBox class.
//
//////////////////////////////////////////////////////////////////////

#include "Fl_ColorBox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Fl_ColorBox::Fl_ColorBox(int x,int y,int w,int h, const char *l)
: Fl_Widget(x,y,w,h,l)
{

}

Fl_ColorBox::~Fl_ColorBox()
{

}

int Fl_ColorBox::handle(int event)
{
	return Fl_Widget::handle(event);
/*
	switch (event) {
	default:
		break;
	}
*/
}

void Fl_ColorBox::draw()
{
	uchar r, g, b;

	r = (uchar)(m_color[0]*255.0);
	g = (uchar)(m_color[1]*255.0);
	b = (uchar)(m_color[2]*255.0);
	
	fl_rectf(x(), y(), w(), h(), r, g, b);
}

void Fl_ColorBox::setColor(double red, double green, double blue)
{
	m_color[0] = red;
	m_color[1] = green;
	m_color[2] = blue;
	redraw();
}
