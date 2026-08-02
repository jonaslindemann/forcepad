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

#include "Shape.h"

#include "Renderer2D.h"

namespace ivf2d {

Shape::Shape()
{
	m_pos[0] = 0.0;
	m_pos[1] = 0.0;
	m_rotation = 0.0;
	m_color = nullptr;
	m_useRasterPos = false;
}

Shape::~Shape()
{
}

void Shape::setPosition(double x, double y)
{
	m_pos[0] = x;
	m_pos[1] = y;
}

void Shape::getPosition(double &x, double &y) const
{
	x = m_pos[0];
	y = m_pos[1];
}

void Shape::setRotation(double angle)
{
	m_rotation = angle;
}

double Shape::getRotation() const
{
	return m_rotation;
}

void Shape::doBeginTransform()
{
	// Push this object's transform onto Renderer2D's stack. The caller
	// (PaintView) establishes the projection and identity base transform each
	// frame; nested shape renders compose on the parent. The old
	// raster-position branch is gone; ScreenImage (the only raster-position
	// user) positions its quad itself.
	Renderer2D &r = Renderer2D::instance();
	r.pushTransform();
	r.translate((float)m_pos[0], (float)m_pos[1]);
	r.rotateZ((float)m_rotation);
}

void Shape::doEndTransform()
{
	Renderer2D::instance().popTransform();
}


void Shape::doMaterial()
{
	if (m_color!=nullptr)
		m_color->render();
}

void Shape::setColor(ColorPtr color)
{
	m_color = color;
}

ColorPtr Shape::getColor() const
{
	return m_color;
}

void Shape::setUseRasterPosition(bool flag)
{
	m_useRasterPos = flag;
}

bool Shape::getUseRasterPosition() const
{
	return m_useRasterPos;
}

} // namespace ivf2d
