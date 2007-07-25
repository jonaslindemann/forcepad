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

#ifndef _Fl_Gl_ColorBox_h_
#define _Fl_Gl_ColorBox_h_

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <FL/fl_draw.H>

class Fl_ColorBox : public Fl_Widget {
private:
	double m_color[3];
public:
	Fl_ColorBox(int,int,int,int,const char * = 0);
	/** Fl_HoverButton class constructor. */
	virtual ~Fl_ColorBox();

	void setColor(double red, double green, double blue);

protected:
	/** Overridden FLTK handle method. */
	int handle(int event);

	/** Overridden FLTK draw method. */
	void draw();
};
#endif 
