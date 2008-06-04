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

#ifndef _CShape_h_
#define _CShape_h_

#include "DrawableBase.h"
#include "Color.h"
#include "Texture.h"

IvfSmartPointer(CShape);

/**
 * Shape base class
 *
 * Base class for all classes in ForcePAD which have
 * a visual representation with a position, rotation,
 * color and a texture. (Drawable in OpenGL).
 */
class CShape : public CDrawableBase {
private:
	double m_pos[2];
	double m_rotation;
	CColorPtr m_color;
	CTexturePtr m_texture;
	bool m_useRasterPos;
public:
	bool getUseRasterPosition();
	void setUseRasterPosition(bool flag);
	/** Shape class constructor. */
	CShape();

	/** Shape class destructor. */
	virtual ~CShape();

	IvfClassInfo("CShape",CDrawableBase);

	// Get/set methods 
	
	/** Set position of shape. */
	void setPosition(double x, double y);

	/** Get position of shape. */
	void getPosition(double &x, double &y);

	/** Set shape rotation. (degrees)*/
	void setRotation(double angle);

	/** Get shape rotation. (degrees)*/
	double getRotation();

	/** Set shape color. */
	void setColor(CColor* color);

	/** Return shape color. */
	CColor* getColor();

	/** Set shape texture. */
	void setTexture(CTexture* texture);

	/** Get shape texture. */
	CTexture* getTexture();

	// Implemented virtual methods

	/** Defined shape material. */
	virtual void doMaterial();

	/** Transforms shape in OpenGL. */
	virtual void doBeginTransform();

	/** Ends transform in OpenGL. */
	virtual void doEndTransform();
};

#endif 
