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

#include "Ellipse.h"

#include <cmath>

CEllipse::CEllipse()
{
	m_size[0] = 25;
	m_size[1] = 25;
	m_sectors = 12;
}

CEllipse::~CEllipse()
{

}

void CEllipse::setSize(int width, int height)
{
	m_size[0] = width;
	m_size[1] = height;
}

void CEllipse::doGeometry()
{
	int i;

	int x, y;
	double dr;

	dr = 2.0*M_PI/(double)m_sectors;

	glBegin(GL_TRIANGLE_FAN);
		glVertex2i(0,0);
		for (i=0; i<=m_sectors; i++)
		{
			x = (int)((float)m_size[0]*cos(i*dr));
			y = (int)((float)m_size[1]*sin(i*dr));
			glVertex2i(x, y);
		}
	glEnd();
}

void CEllipse::setSectors(int sectors)
{
	m_sectors = sectors;
}
