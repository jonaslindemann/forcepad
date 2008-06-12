//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2008 Division of Structural Mecahnics, Lund University
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

#include "forcepad_config.h"

#include <FL/Fl.H>
#include "fl_message_win.h"

#ifdef FORCEPAD_NEW_UI
#include "MainFrame2.h"
#else
#include "MainFrame.h"
#endif

#include "SplashFrame.h"
//#include "StatusOutput.h"
#include "LogWindow.h"

int
main(int argc, char **argv)
{
	//
	// Setup window visuals
	//

	so_print("main","Setting visual to FL_SINGLE|FL_RGB|FL_ALPHA");

	Fl::visual(FL_DOUBLE|FL_RGB|FL_ALPHA);
	Fl::get_system_colors();
	//Fl::background(236, 233, 216);
#ifdef __APPLE__
	Fl::scheme("default");
#else
	Fl::scheme("GTK+");
#endif

	//
	// Create main window
	//

	so_print("main","Creating main window.");

	CMainFrame* frame = new CMainFrame();
	frame->setCommandLine(argc, argv);
	frame->show();

	//
	// Show a splash screen
	//

	so_print("main","Creating splash screen.");

	CSplashFrame* splash = new CSplashFrame();
	splash->setTimer(true);
	splash->centerWindow(frame->getMainWindow());
	splash->show();

	//
	// FLTK main loop
	//

	int result = Fl::run();

	// 
	// Cleanup
	//

	delete frame;

	so_hide();

	return result;
}
