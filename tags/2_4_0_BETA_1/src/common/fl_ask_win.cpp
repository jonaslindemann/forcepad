//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2003 Division of Structural Mecahnics, Lund University
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

#include "fl_ask_win.h"

#ifdef WIN32
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <FL/Fl.H>
#include <FL/x.H>

int fl_ask(const char *msg)
{
	static Fl_Window* topWindow = Fl::first_window();
	
	int result = MessageBox(
		fl_xid(topWindow),
		msg,     
		"Message",  
		MB_YESNO|MB_ICONQUESTION
		);

	if (result==IDYES)
		return 1;
	else
		return 0;
}
#endif
