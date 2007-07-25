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

#ifndef _Ivf2dDef_h_
#define _Ivf2dDef_h_

#pragma warning( disable : 4786 )  // Disable template generation warning

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef NULL
	#ifdef __cplusplus
		#define NULL 0
	#else
		#define NULL ((void*) 0)
	#endif
#endif

//////////////////////////////////////////////////
// This is ripped from the GLUT 3.7 header file.
//////////////////////////////////////////////////
#if defined(_WIN32)

/* GLUT 3.7 now tries to avoid including <windows.h>
   to avoid name space pollution, but Win32's <GL/gl.h> 
   needs APIENTRY and WINGDIAPI defined properly. */
# if 0
#  define  WIN32_LEAN_AND_MEAN
#  include <windows.h>
# else
   /* XXX This is from Win32's <windef.h> */
#  ifndef APIENTRY
#   define GLUT_APIENTRY_DEFINED
#   if (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#    define APIENTRY    __stdcall
#   else
#    define APIENTRY
#   endif
#  endif
   /* XXX This is from Win32's <winnt.h> */
#  ifndef CALLBACK
#   if (defined(_M_MRX000) || defined(_M_IX86) || defined(_M_ALPHA) || defined(_M_PPC)) && !defined(MIDL_PASS)
#    define CALLBACK __stdcall
#   else
#    define CALLBACK
#   endif
#  endif
   /* XXX This is from Win32's <wingdi.h> and <winnt.h> */
#  ifndef WINGDIAPI
#   define GLUT_WINGDIAPI_DEFINED
#   define WINGDIAPI __declspec(dllimport)
#  endif
   /* XXX This is from Win32's <ctype.h> */
#  ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#   define _WCHAR_T_DEFINED
#  endif
# endif
#pragma warning (disable:4244)  /* Disable bogus conversion warnings. */
#pragma warning (disable:4305)  /* VC++ 5.0 version of above warning. */
#pragma warning (disable:4800)  
#endif

//////////////////////////////////////////////////
// End rip...
//////////////////////////////////////////////////

#define IvfStdPointer(classname) \
	class classname; \
	typedef classname* classname##Ptr

#define IvfSmartPointer(classname) \
	class classname; \
	typedef CIvfPointer<classname> classname##Ptr; \
	typedef classname* classname##StdPtr;

#ifdef __APPLE__
#define IvfClassInfo(ivfclassname,ivfparent) \
	void getClassNameThis(char* name) { strcpy(name, ivfclassname); } \
	const char* getClassNameThis() { return ivfclassname; } \
	virtual void getClassName(char* name) { strcpy(name, ivfclassname); } \
	virtual const char* getClassName() { return ivfclassname; } \
	virtual bool isClass(char* name) { \
		char className[30] = ""; \
		getClassNameThis(className); \
		if (className!=NULL) { \
			if (strcmp(className, name)==0) \
				return true; \
			else \
				return ivfparent::isClass(name); \
		}\
		else \
			return false; \
	}
#define IvfClassInfoTop(ivfclassname) \
	void getClassNameThis(char* name) { strcpy(name, ivfclassname); } \
	const char* getClassNameThis() { return ivfclassname; } \
	virtual void getClassName(char* name) { strcpy(name, ivfclassname); } \
	virtual const char* getClassName() { return ivfclassname; } \
	virtual bool isClass(char* name) { \
		char className[30] = ""; \
		getClassNameThis(className); \
		if (className!=NULL) { \
			if (strcmp(className, name)==0) \
				return true; \
			else \
				return false; \
		}\
		else \
			return false; \
	}

#else

#define IvfClassInfo(ivfclassname,ivfparent) \
	void getClassNameThis(char* name) { strcpy_s(name, 30, ivfclassname); } \
	const char* getClassNameThis() { return ivfclassname; } \
	virtual void getClassName(char* name) { strcpy_s(name, 30, ivfclassname); } \
	virtual const char* getClassName() { return ivfclassname; } \
	virtual bool isClass(char* name) { \
		char className[30] = ""; \
		getClassNameThis(className); \
		if (className!=NULL) { \
			if (strcmp(className, name)==0) \
				return true; \
			else \
				return ivfparent::isClass(name); \
		}\
		else \
			return false; \
	}

#define IvfClassInfoTop(ivfclassname) \
	void getClassNameThis(char* name) { strcpy_s(name, 30, ivfclassname); } \
	const char* getClassNameThis() { return ivfclassname; } \
	virtual void getClassName(char* name) { strcpy_s(name, 30, ivfclassname); } \
	virtual const char* getClassName() { return ivfclassname; } \
	virtual bool isClass(char* name) { \
		char className[30] = ""; \
		getClassNameThis(className); \
		if (className!=NULL) { \
			if (strcmp(className, name)==0) \
				return true; \
			else \
				return false; \
		}\
		else \
			return false; \
	}
#endif

#endif
