// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2006 Division of Structural Mecahnics, Lund University
//
// Written by Jonas Lindemann
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Comments and suggestions to jonas.lindemann@byggmek.lth.se

#include "Fl_HoverButton.h"
#include <FL/Fl.H>
#include <FL/fl_draw.H>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Fl_HoverButton::Fl_HoverButton(int x,int y,int w,int h, const char *l)
: Fl_Button(x,y,w,h,l)
{
	m_moreButton = false;
}

Fl_HoverButton::~Fl_HoverButton()
{

}

int Fl_HoverButton::handle(int event)
{
	switch (event) {
	case FL_ENTER:

		redraw();
		return 1;
		break;
	case FL_LEAVE:

		redraw();
		return 1;
		break;
	default:
		return Fl_Button::handle(event);
		break;
	}
}

void Fl_HoverButton::draw()
{
	if (type() == FL_HIDDEN_BUTTON || box() == FL_NO_BOX) return;
	Fl_Color col = value() ? selection_color() : color();
	if (col == FL_GRAY && Fl::belowmouse()==this) col = FL_LIGHT1;
	
	if (value())
	{
		// Pressed

		if (down_box())
			draw_box(down_box(), col);
		else
			draw_box(fl_down(box()), col);
	}
	else
	{
		// Unpressed

		if ((Fl::belowmouse()==this)&&(this->active()))
			draw_box(box(), col);
		else
			draw_box(FL_FLAT_BOX, col);
	}
	
	//draw_box(value() ? (down_box()?down_box():down(box())) : box(), col);
	draw_label();

	if (m_moreButton)
	{
		fl_color(FL_DARK3); 
		
		int xx = x()+w()-7;
		int yy = y()+w()-7;
		
		fl_polygon(xx,yy,xx+5,yy+2,xx,yy+4);
	}
}

void Fl_HoverButton::setMoreButton(bool flag)
{
	m_moreButton = true;
	redraw();
}
