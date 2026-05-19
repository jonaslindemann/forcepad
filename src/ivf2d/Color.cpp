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

#include "Color.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

namespace ivf2d {

Color::Color()
{
	m_color[0] = 0.8f;
	m_color[1] = 0.8f;
	m_color[2] = 0.8f;
	m_color[3] = 1.0f;
}

Color::~Color()
{

}

void Color::setColor(float red, float green, float blue, float alpha)
{
	m_color[0] = red;
	m_color[1] = green;
	m_color[2] = blue;
	m_color[3] = alpha;
}

void Color::doMaterial()
{
	glColor4fv(m_color);
}

void Color::setAlpha(float alpha)
{
	m_color[3] = alpha;
}

void Color::getColor(float *array)
{
	array[0] = m_color[0];
	array[1] = m_color[1];
	array[2] = m_color[2];
}

void Color::getColor(float &r, float &g, float &b, float &a)
{
	r = m_color[0];
	g = m_color[1];
	b = m_color[2];
	a = m_color[3];
}

void Color::setColor(const float* color)
{
	m_color[0] = color[0];
	m_color[1] = color[1];
	m_color[2] = color[2];
}

} // namespace ivf2d
