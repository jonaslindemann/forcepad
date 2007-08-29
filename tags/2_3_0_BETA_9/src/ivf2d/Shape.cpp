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

CShape::CShape()
{
	m_pos[0] = 0.0;
	m_pos[1] = 0.0;
	m_rotation = 0.0;
	m_color = NULL;
	m_texture = NULL;
	m_useRasterPos = false;
}

CShape::~CShape()
{
}

void CShape::setPosition(double x, double y)
{
	m_pos[0] = x;
	m_pos[1] = y;
}

void CShape::getPosition(double &x, double &y)
{
	x = m_pos[0];
	y = m_pos[1];
}

void CShape::setRotation(double angle)
{
	m_rotation = angle;
}

double CShape::getRotation()
{
	return m_rotation;
}

void CShape::doBeginTransform()
{
	if (!m_useRasterPos)
	{
		glPushMatrix();
		glTranslated(m_pos[0], m_pos[1], 0.0);
		glRotatef(m_rotation, 0.0, 0.0, 1.0);
	}
	else
	{
		glRasterPos2i((int)m_pos[0], (int)m_pos[1]);
	}

	//
	// Apply texture, if any.
	//

	if (m_texture!=NULL)
	{
		glEnable(GL_TEXTURE_2D);
		if (!m_texture->isBound())
			m_texture->bind();
		else
			m_texture->apply();
	}
}

void CShape::doEndTransform()
{
	if (!m_useRasterPos)
		glPopMatrix();

	if (m_texture!=NULL)
	{
		glDisable(GL_TEXTURE_2D);
	}
}


void CShape::doMaterial()
{
	if (m_color!=NULL)
		m_color->render();
}

void CShape::setColor(CColor *color)
{
	m_color = color;
}

CColor* CShape::getColor()
{
	return m_color;
}

void CShape::setTexture(CTexture *texture)
{
	m_texture = texture;
}

CTexture* CShape::getTexture()
{
	return m_texture;
}

void CShape::setUseRasterPosition(bool flag)
{
	m_useRasterPos = flag;
}

bool CShape::getUseRasterPosition()
{
	return m_useRasterPos;
}
