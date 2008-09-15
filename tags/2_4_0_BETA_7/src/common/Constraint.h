//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2006 Division of Structural Mecahnics, Lund University
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

#ifndef _CConstraint_H_
#define _CConstraint_H_

#include "Shape.h"
#include "ReactionForce.h"

IvfSmartPointer(CConstraint);

/**
 * Constraint class
 *
 * Implemenets a two-dimensional node constraint. The constraint
 * is visualised with vertical and horisontal lines for x and y 
 * constraints.
 */
class CConstraint : public CShape {
public:
	/** Constraint type */
	enum TConstraintType {
		CT_X, /**< Dof constrained in the x-direction. */
		CT_Y, /**< Dof constrained in the y-direction. */
		CT_XY, /**< Dof constrained both x- and y-directions. */
		CT_SCALAR, /**< Scalar dof constraint. */
		CT_VECTOR,  /**< Vector dof constraint. */
		CT_HINGE
	};
private:
	TConstraintType m_constraintType;
	CReactionForcePtr m_reactionForce;
	double m_size;
	double m_value;
	double m_direction[2];
	double m_vectorSize;
	double m_vectorRadius;
	double m_hingeLength;
	bool m_visibleReaction;
public:
	/** Constraint class constructor. */
	CConstraint ();

	/** Constraint class destructor. */
	virtual ~CConstraint ();

	IvfClassInfo("CConstraint",CShape);

	// Methods

	void assignFrom(CConstraint* constraint);

	// Get/set methods

	/** Sets the visible size of the constraint. */
	void setSize(double size);

	/** Returns the visible size. */
	double getSize();

	/** Sets the constraint type. */
	void setConstraintType(TConstraintType type);

	/** Returns constraint type. */
	TConstraintType getConstraintType();

	void setValue(double value);
	double getValue();

	void getDirection(double &ex, double &ey);
	void setDirection(double ex, double ey);

	void getHingeStart(double &x, double &y);
	void getHingeEnd(double &x, double &y);

	double getVectorSize();
	void setVectorSize(double size);

	void setShowReactionForce(bool flag);
	bool getShowReactionForce();

	CReactionForce* getReactionForce();

	// Virtual overrides

	/** Renders constraint in OpenGL (2D). */
	virtual void doGeometry();

	/** Reads constraint from a text stream. */
	virtual void readFromStream(istream &in);

	/** Stores constraint to a text stream */
	virtual void saveToStream(ostream &out);
};
#endif
