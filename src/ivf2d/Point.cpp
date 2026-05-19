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

#include "Point.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

namespace ivf2d {

Point::Point()
{
	m_pos[0] = 0.0;
	m_pos[1] = 0.0;
}

Point::~Point()
{

}

void Point::setPosition(double x, double y)
{
	m_pos[0] = x;
	m_pos[1] = y;
}

void Point::getPosition(double &x, double &y)
{
	x = m_pos[0];
	y = m_pos[1];
}

const double* Point::getPosition()
{
	return m_pos;
}

void Point::setPosition(const double *pos)
{
	m_pos[0] = pos[0];
	m_pos[1] = pos[1];
}

double Point::x()
{
	return m_pos[0];
}

double Point::y()
{
	return m_pos[1];
}

} // namespace ivf2d
