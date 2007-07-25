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

#ifndef _CForce_H_
#define _CForce_H_

#include "Shape.h"

IvfSmartPointer(CForce);

/**
 * Force class
 *
 * Implements a node force.
 */
class CForce : public CShape {
public:
	enum TForceType {
		FT_VECTOR,
		FT_SCALAR
	};
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
	TForceType m_forceType;
public:
	/** Force class constructor. */
	CForce ();

	/** Force class destructor. */
	virtual ~CForce ();

	IvfClassInfo("CForce",CShape);

	// Methods

	void assignFrom(CForce* force);

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

	/** Read force from text stream. */
	virtual void readFromStream(istream &in);

	/** Store force to text stream. */
	virtual void saveToStream(ostream &out);
};
#endif
