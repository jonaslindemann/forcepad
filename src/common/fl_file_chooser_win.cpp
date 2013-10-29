// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2003 Division of Structural Mecahnics, Lund University
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

#include "fl_file_chooser_win.H"

#ifdef WIN32
#include <Windows.h>
#endif

#ifdef HAVE_CONFIG_H
#include <force_config.h>
#endif

#ifdef HAVE_CSTDIO
#include <cstdio>
#else
#include <stdio.h>
#endif

#include <FL/Fl.H>

#ifndef __APPLE__
#include <FL/x.H>
#endif

#ifdef WIN32
static char g_szFile[260];       // buffer for file name
#endif

char *fl_file_chooser_image(const char *message,const char *fname)
{
#ifdef WIN32

	OPENFILENAME ofn;       // common dialog box structure
	char szTitle[260]; 
	char szFilter[260];

	static Fl_Window* topWindow = Fl::first_window();

	strcpy(g_szFile, fname);
	strcpy(szTitle, message);
	//strncpy(szDefExt, pat+1, 3);
	ZeroMemory(szFilter, sizeof(szFilter));
	sprintf(szFilter, "Image files *.rgb;*.jpg;*.png\0\0");
	szFilter[11] = 0;

	
	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = fl_xid(topWindow) ;
	ofn.lpstrFile = g_szFile;
	ofn.nMaxFile = sizeof(g_szFile);
	ofn.lpstrFilter = szFilter;
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrTitle = szTitle;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrDefExt = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT;
	
	// Display the Open/save dialog box. 

	if (strlen(fname)==0)
	{
		if (GetOpenFileName(&ofn)==TRUE) 
		{
			return g_szFile;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		if (GetSaveFileName(&ofn)==TRUE) 
		{
			return g_szFile;
		}
		else
		{
			return NULL;
		}
	}
	return NULL;
#else
	return NULL;
#endif
}

char *fl_file_chooser(const char *message,const char *pat,const char *fname)
{
#ifdef WIN32

	OPENFILENAME ofn;       // common dialog box structure
	char szTitle[260]; 
	char szFilter[260];
	char szDefExt[3];

	static Fl_Window* topWindow = Fl::first_window();

	strcpy(g_szFile, fname);
	strcpy(szTitle, message);
	strncpy(szDefExt, pat+1, 3);
	// *.fpd files
	// 012345678901
	ZeroMemory(szFilter, sizeof(szFilter));
	sprintf(szFilter, "%s files %s\0\0", pat, pat);
	szFilter[11] = 0;

	
	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = fl_xid(topWindow) ;
	ofn.lpstrFile = g_szFile;
	ofn.nMaxFile = sizeof(g_szFile);
	ofn.lpstrFilter = szFilter;
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrTitle = szTitle;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrDefExt = szDefExt;
	ofn.Flags = OFN_PATHMUSTEXIST|OFN_OVERWRITEPROMPT;
	
	// Display the Open/save dialog box. 

	if (strlen(fname)==0)
	{
		if (GetOpenFileName(&ofn)==TRUE) 
		{
			return g_szFile;
		}
		else
		{
			return NULL;
		}
	}
	else
	{
		if (GetSaveFileName(&ofn)==TRUE) 
		{
			return g_szFile;
		}
		else
		{
			return NULL;
		}
	}
	return NULL;
#else
	return NULL;
#endif
}
