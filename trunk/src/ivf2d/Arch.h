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

#ifndef _CArch_h_
#define _CArch_h_

#include "Shape.h"
#include "Texture.h"
#include "Image.h"
#include "Ellipse.h"

IvfSmartPointer(CArch);

/**
 * Rectangle class
 *
 * Implements a textured rectangle.
 */
class CArch : public CShape {
public:
private:
	double m_size[2];
	double m_lineWidth;
	CColorPtr m_lineColor;
    CEllipsePtr m_ellipse;
    double f1(double x);
public:
	/** Rectangle class constructor. */
    CArch();

	/** Rectangle class destructor. */
    virtual ~CArch();

    IvfClassInfo("CArch",CShape);

	/** Set size of rectangle in world coordinates. */
	void setSize(double width, double height);

	/** Get size of rectangle in world coordinates. */ 
	void getSize(double &width, double &height);

	/** Draw rectangle in OpenGL. */
	virtual void doGeometry();

	void setLineColor(CColor* color);
	void setLineWidth(double width);
    double getLineWidth();
};

#endif
