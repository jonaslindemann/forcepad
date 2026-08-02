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

// OpenGL is linked via CMake (Qt6::OpenGL / OpenGL::GL), not #pragma comment.

// Base only needs stream forward declarations and shared_ptr. It used to pull
// in <iostream> <cmath> <cstdlib> <cstring> <cstdio> <vector> <stack> <deque>
// <set> for the benefit of everything downstream; those now belong to the files
// that actually use them.
#include <iosfwd>
#include <memory>

namespace ivf2d {

class Base;
using BasePtr = std::shared_ptr<Base>;

/**
 * Base class
 *
 * Minimal polymorphic base for most ForcePAD classes: it provides the virtual
 * destructor and the stream serialisation hooks that Force/Constraint and the
 * FemGrid classes override.
 *
 * The former reference counting is gone -- ownership is std::shared_ptr via the
 * static create() factories -- and so is the parent pointer, which had no users.
 */
class Base {
public:
	Base() = default;
	virtual ~Base() = default;

	static BasePtr create() { return std::make_shared<Base>(); }

	/** Virtual method for retrieving object from a stream. */
	virtual void readFromStream(std::istream &in);

	/** Virtual method for storing object to a stream. */
	virtual void saveToStream(std::ostream &out);
};

} // namespace ivf2d
