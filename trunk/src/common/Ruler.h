//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2008 Division of Structural Mecahnics, Lund University
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

#ifndef _CRuler_H_
#define _CRuler_H_

#include "Shape.h"

IvfSmartPointer(CRuler);

/**
 * Constraint class
 *
 * Implemenets a two-dimensional node constraint. The constraint
 * is visualised with vertical and horisontal lines for x and y 
 * constraints.
 */
class CRuler : public CShape {
private:
	int m_startPos[2];
	int m_endPos[2];
	double m_actualLength;

	void initRuler();
public:
	/** Ruler class constructor. */
	CRuler();

	/** Ruler class destructor. */
	virtual ~CRuler();

	IvfClassInfo("CRuler",CShape);

	void doGeometry();

	void setEndPos(int x, int y);
	void setStartPos(int x, int y);
	void setActualLength(double length);
	double getActualLength();
	double getPixelLength();
};

#endif


