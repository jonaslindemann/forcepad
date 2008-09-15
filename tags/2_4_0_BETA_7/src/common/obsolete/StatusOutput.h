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

#ifndef _CStatusOutput_h_
#define _CStatusOutput_h_

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Multi_Browser.H>
#include <FL/Fl_Button.H>
#include <stdarg.h>

#define so_show() CStatusOutput::getInstance()->show();
#define so_print(a,b) CStatusOutput::getInstance()->print(a,b);
#define so_println() CStatusOutput::getInstance()->print("");
#define so_hide() CStatusOutput::getInstance()->hide();
#define so_env(a) CStatusOutput::getInstance()->setEnv(a);

const char* so_format(char* format, ...);

class CStatusOutput : public Fl_Window {
private:
    static CStatusOutput* m_instance;
	Fl_Browser* m_output;
	char* m_env;
protected:
	CStatusOutput(int x=0, int y=0, int w=410, int h=160, char *l="Status");
public:
	static CStatusOutput* getInstance();
	virtual ~CStatusOutput();

	void show();
	void print(const char* txt);
	void print(const char* context, const char* txt);
	void setEnv(const char* env);
};

#endif 
