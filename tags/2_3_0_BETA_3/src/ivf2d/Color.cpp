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

CColor::CColor()
{
	m_color[0] = 0.8f;
	m_color[1] = 0.8f;
	m_color[2] = 0.8f;
	m_color[3] = 1.0f;
}

CColor::~CColor()
{

}

void CColor::setColor(float red, float green, float blue, float alpha)
{
	m_color[0] = red;
	m_color[1] = green;
	m_color[2] = blue;
	m_color[3] = alpha;
}

void CColor::doMaterial()
{
	glColor4fv(m_color);
}

void CColor::setAlpha(float alpha)
{
	m_color[3] = alpha;
}

void CColor::getColor(float *array)
{
	array[0] = m_color[0];
	array[1] = m_color[1];
	array[2] = m_color[2];
}

void CColor::getColor(float &r, float &g, float &b, float &a)
{
	r = m_color[0];
	g = m_color[1];
	b = m_color[2];
	a = m_color[3];
}
