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

#include "Line.h"

#include "Renderer2D.h"

namespace ivf2d {

Line::Line()
{
	m_width = 1;
	m_startPos[0] = 0;
	m_startPos[1] = 0;
	m_endPos[0] = 50;
	m_endPos[1] = 50;
}

Line::~Line()
{

}

void Line::setWidth(int width)
{
	m_width = width;
	initLine();
}

void Line::setStartPos(int x, int y)
{
	m_startPos[0] = x;
	m_startPos[1] = y;
	initLine();
}

void Line::setEndPos(int x, int y)
{
	m_endPos[0] = x;
	m_endPos[1] = y;
	initLine();
}

void Line::doGeometry()
{
	Renderer2D &r = Renderer2D::instance();
	r.beginQuads();
		r.vertex((float)m_p1.getX(), (float)m_p1.getY());
		r.vertex((float)m_p2.getX(), (float)m_p2.getY());
		r.vertex((float)m_p3.getX(), (float)m_p3.getY());
		r.vertex((float)m_p4.getX(), (float)m_p4.getY());
	r.end();
}

void Line::initLine()
{
	Vec3d forward;
	Vec3d right;
	Vec3d p1;
	Vec3d p2;


	double dx, dy, dz;

	p1.setComponents((double)m_startPos[0], (double)m_startPos[1], 0.0);
	p2.setComponents((double)m_endPos[0], (double)m_endPos[1], 0.0);

	forward.setFromPoints(p1,p2);
	forward.normalize();

	forward.getComponents(dx, dy, dz);
	right.setComponents(-dy, dx, 0.0);

	m_p1 = p1 + (double)m_width*0.5*right;
	m_p2 = p1 - (double)m_width*0.5*right;
	m_p3 = p2 - (double)m_width*0.5*right;
	m_p4 = p2 + (double)m_width*0.5*right;
}

int Line::getWidth() const
{
	return m_width;
}

} // namespace ivf2d
