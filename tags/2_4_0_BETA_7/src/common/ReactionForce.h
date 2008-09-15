//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2007 Division of Structural Mecahnics, Lund University
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

#ifndef _CReactionForce_h_
#define _CReactionForce_h_

#include "Shape.h"

IvfSmartPointer(CReactionForce);

class CReactionForce : public CShape {
private:
	void initAngle();

	double m_value;
	double m_direction[2];
	double m_length;
	double m_angle;
	double m_arrowAngle;
	double m_arrowSize;
	double m_leftPos[2];
	double m_rightPos[2];
	double m_offset;
public:
	/** Force class constructor. */
	CReactionForce ();

	/** Force class destructor. */
	virtual ~CReactionForce ();

	IvfClassInfo("CReactionForce",CShape);

	// Get/set methods

	/** Set force value. */
	void setValue(double value);

	/** Return force value. */
	double getValue();

	/** 
	 * Set force direction.
	 *
	 * \c vx and \c vy directions determines
	 * the direction of the force. The direction
	 * values are normalized automatically.
	 */
	void setDirection(double vx, double vy);

	/** 
	 * Return force direction.
	 *
	 * Returns the normalized force directions.
	 */
	void getDirection(double &vx, double &vy);

	/** Set visual length of force arrow. */
	void setLength(double length);

	/** Return visual length of force arrow. */
	double getLength();

	/** Set visual arrow tip size. */
	void setArrowSize(double size);

	/** Return visual arrow tip size. */
	double getArrowSize();

	/** Set visual arrow tip angle. */
	void setArrowAngle(double angle);

	/** Return visual arrow tip angle. */
	double getArrowAngle();

	double getValueY();
	double getValueX();

	// Virtual overrides

	/** Draw force in OpenGL */
	virtual void doGeometry();
};
#endif
