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

#ifndef _CRectangle_h_
#define _CRectangle_h_

#include "Shape.h"
#include "Texture.h"
#include "Image.h"

IvfSmartPointer(CRectangle);

/**
 * Rectangle class
 *
 * Implements a textured rectangle.
 */
class CRectangle : public CShape {
public:
	enum TRectangleType {
		RT_SOLID,
		RT_OUTLINE,
		RT_SOLID_OUTLINE
	};
	enum TLineType {
		LT_SOLID,
		LT_DASHED,
		LT_DOTTED
	};
private:
	double m_size[2];
	double m_ratioX;
	double m_ratioY;
	double m_imageRatio;
	int m_textureSetup;
	TRectangleType m_rectType;
	TLineType m_lineType;
	double m_lineWidth;
    short m_lineStipple;
    int m_lineFactor;
	CColorPtr m_lineColor;
public:
	/** Rectangle class constructor. */
	CRectangle();

	/** Rectangle class destructor. */
	virtual ~CRectangle();

	IvfClassInfo("CRectangle",CShape);

	/** Set size of rectangle in world coordinates. */
	void setSize(double width, double height);

	/** Get size of rectangle in world coordinates. */ 
	void getSize(double &width, double &height);

	/** Set texture override. */
	void setTexture(CTexture* texture); // Override

	/** Draw rectangle in OpenGL. */
	virtual void doGeometry();

	/** Set type of rectangle */
	void setRectangleType(TRectangleType type);

	void setLineColor(CColor* color);
	void setLineWidth(double width);
	void setLineType(TLineType type);
};

#endif
