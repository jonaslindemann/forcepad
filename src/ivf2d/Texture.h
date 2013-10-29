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

#ifndef _CTexture_h_
#define _CTexture_h_

#include "Base.h"
#include "DrawableBase.h" // Get OpenGL stuff
#include "Image.h"

IvfSmartPointer(CTexture);

/**
 * Texture class.
 *
 * Implements an OpenGL texture.
 */
class CTexture : public CBase {
private:
    unsigned int m_wrapT;
    unsigned int m_wrapS;
    unsigned int m_textureMode;
    unsigned int m_magFilter;
    unsigned int m_minFilter;
    unsigned int m_textureName;
	float m_textureEnvColor[4];
	bool m_bound;
	CImagePtr m_image;
	double m_texScaleX;
	double m_texScaleY;
	double m_texRotate;
public:
	/** Texture class constructor. */
	CTexture();

	/** Texture class destructor. */
	virtual ~CTexture();

	IvfClassInfo("CTexture",CBase);

	/** Applies a bound texture. */
	void apply();

	/** Binds a texture to a texture name. */
	void bind();

	/** Returns true if texture is bound. */
	bool isBound();

	// Get/set methods

	/** Returns current texture image. */
	CImage* getImage();

	/** 
	 * Sets current texture image. 
	 *
	 * Replaces existing image. If existing image 
	 * is unreferenced it will be deleted.
	 */
	void setImage(CImage* image);
};

#endif 
