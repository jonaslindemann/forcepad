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

#ifndef _CColor_h_
#define _CColor_h_

#include "DrawableBase.h"

IvfSmartPointer(CColor);

/**
 * Color class
 *
 * Implements a color value that can be used by all
 * drawable classes in ForcePAD.
 */
class CColor : public CDrawableBase {
private:
	float m_color[4];
public:
	/** Color class constructor. */
	CColor();

	/** Color class destructor. */
	virtual ~CColor();

	IvfClassInfo("CColor",CDrawableBase);

	// Get/set methods

	/** Sets color value. */
	void setColor(float red, float green, float blue, float alpha);

	/** Set color alpha value. */
	void setAlpha(float alpha);

	/** Assigns array the current color value. */
	void getColor(float* array);

	/** Returns color */
	void getColor(float &r, float &g, float &b, float &a);


	// Virtual overrides

	/** 
	 * Drawable base doMaterial() override.
	 * 
	 * The color class is rendered in OpenGL using the 
	 * DrawbleBase method render(), which in turn calls this 
	 * method for setting the current color.
	 */
	virtual void doMaterial();
};

#endif 

