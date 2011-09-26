//|
//| Mouse cursor shapes for the Fast Light Tool Kit (FLTK).
//|
//| Copyright 1998-1999 by Vivality llc, Venice, CA, U.S.A. 
//|  and Matthias Melcher.
//|
//| This library is free software; you can redistribute it and/or
//| modify it under the terms of the GNU Library General Public
//| License as published by the Free Software Foundation; either
//| version 2 of the License, or (at your option) any later version.
//|
//| This library is distributed in the hope that it will be useful,
//| but WITHOUT ANY WARRANTY; without even the implied warranty of
//| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//| Library General Public License for more details.
//|
//| You should have received a copy of the GNU Library General Public
//| License along with this library; if not, write to the Free Software
//| Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
//| USA.
//|
//| Please report all bugs and problems to "matthias@mediaone.net".
//|


#ifndef FL_CURSOR_SHAPE_H
#define FL_CURSOR_SHAPE_H

#ifndef __APPLE__

#define FL_INTERNALS

#include <FL/Enumerations.H>
#include <FL/Fl.h>

#ifdef WIN32
# include <windows.h>
# define HCursor HCURSOR
#elif defined(__APPLE__)
# include <Carbon/Carbon.h>
# define HCursor CursPtr
#else
# include <X11/Xlib.h>
# include <X11/cursorfont.h> 
# define HCursor Cursor
#endif


class Fl_Cursor_Shape;


extern void fl_cursor_ex(Fl_Cursor_Shape *c);


class Fl_Cursor_Shape {
	static int hgt_, wdt_;
	Fl_Color ol_, fl_;
	HCursor handle_;
	void init();
public:
	Fl_Cursor_Shape() { init(); }
	Fl_Cursor_Shape(int hotx, int hoty, unsigned short *andPattern, unsigned short *xorPattern, Fl_Color=FL_BLACK, Fl_Color=FL_WHITE);
	Fl_Cursor_Shape(int hotx, int hoty, unsigned int *andPattern, unsigned int *xorPattern, Fl_Color=FL_BLACK, Fl_Color=FL_WHITE);
	~Fl_Cursor_Shape();
	void shape(int hotx, int hoty, unsigned short *andPattern, unsigned short *xorPattern, Fl_Color=FL_BLACK, Fl_Color=FL_WHITE);
	void shape(int hotx, int hoty, unsigned int *andPattern, unsigned int *xorPattern, Fl_Color=FL_BLACK, Fl_Color=FL_WHITE);
	void color(Fl_Color ol, Fl_Color fl);
	Fl_Color outlineColor() { return ol_; }
	Fl_Color fillColor() { return fl_; }
	HCursor handle() { return handle_; }
};



#endif
#endif

