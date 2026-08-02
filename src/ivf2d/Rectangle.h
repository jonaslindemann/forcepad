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
#include <memory>

namespace ivf2d {

class Rectangle;
using RectanglePtr = std::shared_ptr<Rectangle>;

/**
 * Rectangle class
 *
 * Implements a solid and/or outlined rectangle.
 */
class Rectangle : public Shape {
public:
	enum class TRectangleType {
		RT_SOLID,
		RT_OUTLINE,
		RT_SOLID_OUTLINE
	};
	enum class TLineType {
		LT_SOLID,
		LT_DASHED,
		LT_DOTTED
	};
private:
	double m_size[2];
	TRectangleType m_rectType;
	TLineType m_lineType;
	double m_lineWidth;
    short m_lineStipple;
    int m_lineFactor;
	ColorPtr m_lineColor;
public:
	/** Rectangle class constructor. */
	Rectangle();

	/** Rectangle class destructor. */
	virtual ~Rectangle();

	static RectanglePtr create() { return std::make_shared<Rectangle>(); }

	/** Set size of rectangle in world coordinates. */
	void setSize(double width, double height);

	/** Get size of rectangle in world coordinates. */
	Vec2d getSize() const;

	/** Draw rectangle in OpenGL. */
	virtual void doGeometry() override;

	/** Set type of rectangle */
	void setRectangleType(TRectangleType type);

	void setLineColor(ColorPtr color);
	void setLineWidth(double width);
	void setLineType(TLineType type);
};


} // namespace ivf2d

