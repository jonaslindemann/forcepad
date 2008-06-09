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

#ifndef IVFDEF_H
#define IVFDEF_H

#include <ivf/ivfconfig.h>

#define IvfStdPointer(classname) \
	class classname; \
	typedef classname* classname##Ptr

#define IvfSmartPointer(classname) \
	class classname; \
	typedef CIvfPointer<classname> classname##Ptr; \
	typedef classname* classname##StdPtr;

#define IvfSmartPointerRefBase(classname,refbase) \
	class classname; \
	typedef CIvfPointerRefBase<classname,refbase> classname##Ptr; \
	typedef classname* classname##StdPtr;

#define IvfClassInfo(ivfclassname,ivfparent) \
	void getClassNameThis(char* name) { strcpy(name, ivfclassname); } \
	const char* getClassNameThis() { return ivfclassname; } \
	virtual void getClassName(char* name) { strcpy_s(name, ivfclassname); } \
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

#ifdef IVF_DEBUG_PRINT
	#define IvfDbg(txt) \
		cout << txt << endl;

	#ifdef IVF_DEBUG_LEVEL_1
		#define IvfDbg1(txt) \
			cout << txt << endl;
		#define IvfDbg2(txt)
		#define IvfDbg3(txt)
	#endif

	#ifdef IVF_DEBUG_LEVEL_2
		#define IvfDbg1(txt) \
			cout << txt << endl;
		#define IvfDbg2(txt) \
			cout << txt << endl;
		#define IvfDbg3(txt)
	#endif

	#ifdef IVF_DEBUG_LEVEL_3
		#define IvfDbg1(txt) \
			cout << txt << endl;
		#define IvfDbg2(txt) \
			cout << txt << endl;
		#define IvfDbg3(txt) \
			cout << txt << endl;
	#endif
#else
	#define IvfDbg(txt)
	#define IvfDbg1(txt)
	#define IvfDbg2(txt)
	#define IvfDbg3(txt)
#endif

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

// DLL-support on Win32

#ifdef WIN32
	#ifdef IVF_DLL
		#ifdef IVF_EXPORTS
			#define IVF_API __declspec(dllexport)
		#else
			#define IVF_API __declspec(dllimport)
		#endif
		#pragma warning(disable:4251)
	#else
		#define IVF_API
	#endif

	#ifdef IVFMATH_DLL
		#ifdef IVFMATH_EXPORTS
			#define IVFMATH_API __declspec(dllexport)
		#else
			#define IVFMATH_API __declspec(dllimport)
		#endif
		#pragma warning(disable:4251)
	#else
		#define IVFMATH_API
	#endif

	#ifdef IVFIMAGE_DLL
		#ifdef IVFIMAGE_EXPORTS
			#define IVFIMAGE_API __declspec(dllexport)
		#else
			#define IVFIMAGE_API __declspec(dllimport)
		#endif
		#pragma warning(disable:4251)
	#else
		#define IVFIMAGE_API
	#endif

	#ifdef IVFFILE_DLL
		#ifdef IVFFILE_EXPORTS
			#define IVFFILE_API __declspec(dllexport)
		#else
			#define IVFFILE_API __declspec(dllimport)
		#endif
		#pragma warning(disable:4251)
	#else
		#define IVFFILE_API
	#endif

	#ifdef IVFCTL_DLL
		#ifdef IVFCTL_EXPORTS
			#define IVFCTL_API __declspec(dllexport)
		#else
			#define IVFCTL_API __declspec(dllimport)
		#endif
		#pragma warning(disable:4251)
	#else
		#define IVFCTL_API
	#endif
	
	#ifdef IVFEXT_DLL
		#ifdef IVFEXT_EXPORTS
			#define IVFEXT_API __declspec(dllexport)
		#else
			#define IVFEXT_API __declspec(dllimport)
		#endif
		#pragma warning(disable:4251)
	#else
		#define IVFEXT_API
	#endif	

	#define IVFWIDGET_API
	#define IVFUI_API
	#define IVFFLTKWIDGET_API

#else
	#define IVF_API
	#define IVFMATH_API
	#define IVFIMAGE_API
	#define IVFFILE_API
	#define IVFCTL_API
	#define IVFWIDGET_API
	#define IVFUI_API
	#define IVFFLTKWIDGET_API
	#define IVFEXT_API
#endif

#endif
