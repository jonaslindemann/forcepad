//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2026 Division of Structural Mecahnics, Lund University
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

#pragma once

#include "CommonDefs.h"

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

namespace ivf2d {

IvfSmartPointer(Base);

/**
 * Base class
 * 
 * Base class used by most ForcePAD classes.
 * Contains code for reference counting, parent pointer and
 * stream handling.
 */
class Base {
private:
	Base* m_parent;
public:
	/** Base class constructor.*/
	Base ();
	/** Base class destructor.*/
	virtual ~Base ();

	static BasePtr create() { return std::make_shared<Base>(); }

	IvfClassInfoTop("Base");

	/** Sets parent object. */
	void setParent(Base* parent);

	/** Returns parent object. */
	virtual Base* getParent();

	/** Virtual method for retrieving object from a stream. */
	virtual void readFromStream(std::istream &in);

	/** Virtual method for storing object to a stream. */
	virtual void saveToStream(std::ostream &out);
};

} // namespace ivf2d
