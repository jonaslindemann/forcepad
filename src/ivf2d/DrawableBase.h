//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2009 Division of Structural Mecahnics, Lund University
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

#ifndef _CDrawableBase_h_
#define _CDrawableBase_h_

#include "Base.h"

/**
 * Drawable base class
 *
 * Base class for all classes in ForcePAD which have
 * a visual representation. (Drawable in OpenGL).
 */
class CDrawableBase : public CBase {
private:

public:
	/** DrawableBase class constructor. */
	CDrawableBase();

	/** DrawableBase class destructor. */
	virtual ~CDrawableBase();

	/** 
	 * virtual render method
	 *
	 * A drawable class is rendererd in OpenGL by calling the
	 * render() method. This method has the following default
	 * implementation:
	 *
	 * \code
	 * doMaterial();
	 * doBeginTransform();
	 * doGeometry();
	 * doEndTransform();
	 * \endcode
	 * 
	 * Note. It is often not neccesary to override this method.
	 */
	virtual void render();
protected:
	/** 
	 * Material definition virtual method.
	 *
	 * OpenGL color and material state changes
	 * should be placed in this method.
	 */
	virtual void doMaterial();

	/** 
	 * Shape transform virtual method.
	 *
	 * OpenGL transformations should be placed
	 * in this method. A typical doBeginTransform()
	 * method is shown below:
	 *
	 * \code 
	 * CMyShape::doBeginTransform()
	 * {
	 *   glPushMatrix();
	 *   glTranslated(...);
	 *   glRotatef(...);
	 * 
	 *   // Apply textures here
	 * 
	 * }
	 */
	virtual void doBeginTransform();

	/**
	 * Finalize transforms.
	 *
	 * Any OpenGL transforms started in doBeginTransform() should be 
	 * finished by placing a glPopMatrix() function in this method.
	 */
	virtual void doEndTransform();

	/**
	 * Create visible geometry.
	 *
	 * All geometry creation is placed in this method. Coordinates used
	 * should be assumed local to the generated object.
	 */
	virtual void doGeometry();
};

#endif 
