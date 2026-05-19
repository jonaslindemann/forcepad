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

#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

namespace ivf2d {

Shape::Shape()
{
	m_pos[0] = 0.0;
	m_pos[1] = 0.0;
	m_rotation = 0.0;
	m_color = nullptr;
	m_texture = nullptr;
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

void Shape::getPosition(double &x, double &y)
{
	x = m_pos[0];
	y = m_pos[1];
}

void Shape::setRotation(double angle)
{
	m_rotation = angle;
}

double Shape::getRotation()
{
	return m_rotation;
}

void Shape::doBeginTransform()
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

	if (m_texture!=nullptr)
	{
		glEnable(GL_TEXTURE_2D);
		if (!m_texture->isBound())
			m_texture->bind();
		else
			m_texture->apply();
	}
}

void Shape::doEndTransform()
{
	if (!m_useRasterPos)
		glPopMatrix();

	if (m_texture!=nullptr)
	{
		glDisable(GL_TEXTURE_2D);
	}
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

ColorPtr Shape::getColor()
{
	return m_color;
}

void Shape::setTexture(TexturePtr texture)
{
	m_texture = texture;
}

Texture* Shape::getTexture()
{
	return m_texture.get();
}

void Shape::setUseRasterPosition(bool flag)
{
	m_useRasterPos = flag;
}

bool Shape::getUseRasterPosition()
{
	return m_useRasterPos;
}

} // namespace ivf2d
