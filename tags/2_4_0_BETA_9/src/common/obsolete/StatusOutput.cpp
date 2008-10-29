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

#include "StatusOutput.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

CStatusOutput* CStatusOutput::m_instance=0;


#include <iostream>

static char g_buff[512];

const char* so_format(char* format, ...)
{
	va_list args; 
    va_start(args, format); 
    vsprintf (g_buff, format, args); /* Format the string */

	return g_buff;
}

CStatusOutput::CStatusOutput(int x, int y, int w, int h, char *l):
        Fl_Window(x, y, w, h, l)
{
    box(FL_FLAT_BOX);
    user_data((void*)(this));
	m_output = new Fl_Multi_Browser(5, 5, 400,150);
	m_output->color(FL_WHITE);
	Fl_Group::current()->resizable(m_output);
    end();

	m_env = NULL;
}

CStatusOutput::~CStatusOutput()
{
	if (m_env!=NULL)
		delete [] m_env;
}

CStatusOutput* CStatusOutput::getInstance()
{
    if (m_instance==0)
        m_instance = new CStatusOutput();
    
    return m_instance;
}


void CStatusOutput::print(const char* txt)
{
	char* buffer = new char[255];
	if (m_env==NULL)
		sprintf(buffer, "@s@f@B%d@C%d@.%s", FL_WHITE, FL_BLACK, txt);
	else
		sprintf(buffer, "@s@f@B%d@C%d@.%s:\t%s", FL_WHITE, FL_BLACK, m_env, txt);
    m_output->add(buffer);
	if (m_output->size()>100)
		m_output->remove(1);
	m_output->bottomline(m_output->size());
	delete [] buffer;
}

void CStatusOutput::print(const char* context, const char* txt)
{
	char* buffer = new char[255];
	int pos = 35;
	sprintf(buffer, "@s@f@B%d@C%d@.%s:                                                                              ", 
		FL_WHITE, FL_BLACK, context);
	sprintf(buffer+pos, "%s", txt);
    m_output->add(buffer);
	if (m_output->size()>100)
		m_output->remove(1);
	m_output->bottomline(m_output->size());
	delete [] buffer;
}

void CStatusOutput::show()
{
	Fl_Window::show();
}

void CStatusOutput::setEnv(const char *env)
{
	if (m_env!=NULL)
		delete [] m_env;

	m_env = new char[strlen(env)+1];
	strcpy(m_env, env);
}
