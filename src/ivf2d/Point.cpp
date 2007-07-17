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

CPoint::CPoint()
{
	m_pos[0] = 0.0;
	m_pos[1] = 0.0;
}

CPoint::~CPoint()
{

}

void CPoint::setPosition(double x, double y)
{
	m_pos[0] = x;
	m_pos[1] = y;
}

void CPoint::getPosition(double &x, double &y)
{
	x = m_pos[0];
	y = m_pos[1];
}

const double* CPoint::getPosition()
{
	return m_pos;
}

void CPoint::setPosition(const double *pos)
{
	m_pos[0] = pos[0];
	m_pos[1] = pos[1];
}

double CPoint::x()
{
	return m_pos[0];
}

double CPoint::y()
{
	return m_pos[1];
}
