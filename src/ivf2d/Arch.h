//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2026 Division of Structural Mecahnics, Lund University
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

#pragma once

#include "Shape.h"
#include "Vec2d.h"
#include "Image.h"
#include "Ellipse.h"
#include <memory>

namespace ivf2d {

class Arch;
using ArchPtr = std::shared_ptr<Arch>;

/**
 * Rectangle class
 *
 * Implements a textured rectangle.
 */
class Arch : public Shape {
public:
private:
	double m_size[2];
	double m_lineWidth;
	ColorPtr m_lineColor;
    EllipsePtr m_ellipse;
    double f1(double x);
public:
	/** Rectangle class constructor. */
    Arch();

	/** Rectangle class destructor. */
    virtual ~Arch();

	static ArchPtr create() { return std::make_shared<Arch>(); }

	/** Set size of rectangle in world coordinates. */
	void setSize(double width, double height);

	/** Get size of rectangle in world coordinates. */ 
	Vec2d getSize() const;

	/** Draw rectangle in OpenGL. */
	void doGeometry() override;

	void setLineColor(ColorPtr color);
	void setLineWidth(double width);
    double getLineWidth() const;
};


} // namespace ivf2d
