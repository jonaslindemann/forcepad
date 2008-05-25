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

#ifndef _CBase_H_
#define _CBase_H_

#include "CommonDefs.h"
#include "IvfPointer.h"

#ifdef WIN32
#pragma comment( lib, "opengl32" )
#pragma comment( lib, "glu32" )
//#pragma comment( lib, "ivf2d" )
#endif

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <vector>
#include <stack>
#include <deque>
#include <set>

using namespace std;

IvfSmartPointer(CBase);

/**
 * Base class
 * 
 * Base class used by most ForcePAD classes.
 * Contains code for reference counting, parent pointer and
 * stream handling.
 */
class CBase {
private:
	int m_ref;
	CBase* m_parent;
public:
	/** Base class constructor.*/
	CBase ();
	/** Base class destructor.*/
	virtual ~CBase ();

	IvfClassInfoTop("CBase");

	/** Increase reference count by 1.*/
	void addReference();

	/**
	 * Decrease reference count.
	 *
	 * Count is decreased only if > 0.
	 */
	void delReference();

	/** Returns true if reference count > 0.*/
	bool isReferenced();

	/** Returns reference count.*/
	int getReferenceCount();

	/** Sets parent object. */
	void setParent(CBase* parent);

	/** Returns parent object. */
	virtual CBase* getParent();

	/** Virtual method for retrieving object from a stream. */
	virtual void readFromStream(istream &in);

	/** Virtual method for storing object to a stream. */
	virtual void saveToStream(ostream &out);
};
#endif
