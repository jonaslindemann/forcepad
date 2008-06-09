//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2006 Division of Structural Mecahnics, Lund University
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

#ifndef Fl_HoverButton_H
#define Fl_HoverButton_H

#include <FL/Fl_Button.H>

/**
 * Hover button class
 *
 * Specialized FLTK Fl_Button class. Implementing
 * a hovering button. The button is flat when mouse 
 * is not over it. When mouse is over the button adds
 * an up frame.
 */
class Fl_HoverButton : public Fl_Button {
private:
	bool m_moreButton;
public:
	void setMoreButton(bool flag);
	/** Fl_HoverButton class constructor. */
	Fl_HoverButton(int,int,int,int,const char * = 0);

	/** Fl_HoverButton class constructor. */
	virtual ~Fl_HoverButton();
protected:
	/** Overridden FLTK handle method. */
	int handle(int event);

	/** Overridden FLTK draw method. */
	void draw();
};

#endif 
