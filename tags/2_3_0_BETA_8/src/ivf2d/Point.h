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

#ifndef _CPoint_h_
#define _CPoint_h_

#include "Base.h"

/**
 * Point class
 *
 * The point class implements a simple 2D point.
 */
class CPoint : public CBase {
private:
	double m_pos[2];
public:
	/** Point class constructor. */
	CPoint();

	/** Point class destructor. */
	virtual ~CPoint();

	// Get/set methods

	/** Set point position. */
	void setPosition(double x, double y);

	/** Set point position. */
	void setPosition(const double* pos);

	/** Get Point position. */
	void getPosition(double &x, double &y);

	/** Return pointer to point position. (read-only) _*/
	const double* getPosition();

	/** Return point x position. */
	double x();

	/** Return point y position. */
	double y();
};

#endif 
