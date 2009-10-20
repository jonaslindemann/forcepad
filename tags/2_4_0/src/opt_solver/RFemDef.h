//
// Copyright 1999-2007 by Structural Mechanics, Lund University.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "RFem@byggmek.lth.se".
//
//
// Written by Jonas Lindemann
//

#ifndef RFEMDEF_H
#define RFEMDEF_H

#define RFEM_VERSION_MAJOR	0
#define RFEM_VERSION_MINOR	1
#define RFEM_VERSION_RELEASE 0

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <string>
#include <vector>
#include <map>

#include <include.h>
#include <newmat.h>
#include <newmatio.h>
#include <newmatap.h>

#ifdef RFEM_DEBUG_PRINT
	#define RFemDbg(txt) \
		std::cout << txt << std::endl;

	#ifdef RFEM_DEBUG_LEVEL_1
		#define RFemDbg1(txt) \
			std::cout << txt << std::endl;
		#define RFemDbg2(txt)
		#define RFemDbg3(txt)
	#endif

	#ifdef RFEM_DEBUG_LEVEL_2
		#define RFemDbg1(txt) \
			std::cout << txt << std::endl;
		#define RFemDbg2(txt) \
			std::cout << txt << std::endl;
		#define RFemDbg3(txt)
	#endif

	#ifdef RFEM_DEBUG_LEVEL_3
		#define RFemDbg1(txt) \
			std::cout << txt << std::endl;
		#define RFemDbg2(txt) \
			std::cout << txt << std::endl;
		#define RFemDbg3(txt) \
			std::cout << txt << std::endl;
	#endif
#else
	#define RFemDbg(txt)
	#define RFemDbg1(txt)
	#define RFemDbg2(txt)
	#define RFemDbg3(txt)
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
	#ifdef RFEM_DLL
		#ifdef RFEMMATH_LIB
			#define RFemMATH_API __declspec(dllexport)
		#else
			#define RFemMATH_API __declspec(dllimport)
		#endif

		#ifdef RFEM_LIB
			#define RFEM_API __declspec(dllexport)
		#else
			#define RFEM_API __declspec(dllimport)
		#endif

		#ifdef RFemIMAGE_LIB
			#define RFemIMAGE_API __declspec(dllexport)
		#else
			#define RFemIMAGE_API __declspec(dllimport)
		#endif

		#ifdef RFemFILE_LIB
			#define RFemFILE_API __declspec(dllexport)
		#else
			#define RFemFILE_API __declspec(dllimport)
		#endif

		#ifdef RFemFLTK_LIB
			#define RFemFLTK_API __declspec(dllexport)
		#else
			#define RFemFLTK_API __declspec(dllimport)
		#endif

		#ifdef RFemUI_LIB
			#define RFemUI_API __declspec(dllexport)
		#else
			#define RFemUI_API __declspec(dllimport)
		#endif

		#ifdef RFemWIDGET_LIB
			#define RFemWIDGET_API __declspec(dllexport)
		#else
			#define RFemWIDGET_API __declspec(dllimport)
		#endif

		#ifdef RFemCTL_LIB
			#define RFemCTL_API __declspec(dllexport)
		#else
			#define RFemCTL_API __declspec(dllimport)
		#endif

		#ifdef RFem3DUI_LIB
			#define RFem3DUI_API __declspec(dllexport)
		#else
			#define RFem3DUI_API __declspec(dllimport)
		#endif

		#ifdef RFemEXT_LIB
			#define RFemEXT_API __declspec(dllexport)
		#else
			#define RFemEXT_API __declspec(dllimport)
		#endif

		#ifdef RFemWIN32_LIB
			#define RFemWIN32_API __declspec(dllexport)
		#else
			#define RFemWIN32_API __declspec(dllimport)
		#endif

		#ifdef RFemFONT_LIB
			#define RFemFONT_API __declspec(dllexport)
		#else
			#define RFemFONT_API __declspec(dllimport)
		#endif

		#ifdef RFemGLE_LIB
			#define RFemGLE_API __declspec(dllexport)
		#else
			#define RFemGLE_API __declspec(dllimport)
		#endif

		#pragma warning(disable:4251)
	#else
		#define RFEMMATH_API
	#endif
#else
		#define RFEMMATH_API
#endif 

#define StdPointer(classname) \
	class classname; \
	typedef classname* classname##Ptr

#define SmartPointer(classname) \
	class classname; \
	typedef CPointer<classname> classname##Ptr; \
	typedef classname* classname##StdPtr;

#define SmartPointerRefBase(classname,refbase) \
	class classname; \
	typedef CPointerRefBase<classname,refbase> classname##Ptr; \
	typedef classname* classname##StdPtr;

#define ClassInfo(classname,parent) \
	const char* getClassNameThis() { return classname; } \
	virtual const char* getClassName() { return classname; } \
	virtual bool isClass(char* name) { \
		std::string className; \
		std::string queryClass = name; \
		className = getClassNameThis(); \
		if (!className.empty()) { \
			if (className == queryClass) \
				return true; \
			else \
				return parent::isClass(name); \
		}\
		else \
			return false; \
	} 

#define ClassInfoTop(classname) \
	const char* getClassNameThis() { return classname; } \
	virtual const char* getClassName() { return classname; } \
	virtual bool isClass(const char* name) { \
		std::string className = getClassNameThis(); \
		std::string queryClassname = name; \
		if (className == queryClassname) \
			return true; \
		else \
			return false; \
	} 


#endif
