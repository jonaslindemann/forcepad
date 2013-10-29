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

#include "Rectangle.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

CRectangle::CRectangle()
{
	m_size[0] = 1.0;
	m_size[1] = 1.0;
	m_ratioX = 1.0;
	m_ratioY = 1.0;
	m_imageRatio = 1.0;
	m_textureSetup = 1;

	m_lineFactor = 1;
	m_lineType = LT_SOLID;
	m_rectType = RT_SOLID;
	m_lineWidth = 1.0;
	m_lineColor = new CColor();
	m_lineColor->setColor(0.0f, 0.0f, 0.0f, 1.0f);
}

CRectangle::~CRectangle()
{

}

void CRectangle::setSize(double width, double height)
{
	m_size[0] = width;
	m_size[1] = height;

	//
	// Calculate ratios
	//

	m_ratioX = m_size[0]/m_size[1];
	m_ratioY = m_size[1]/m_size[0];

	//
	// Determine texture setup
	//

	if (m_ratioX>1.0)
	{
		if (m_imageRatio>m_ratioX)
			m_textureSetup = 1;
		else
			m_textureSetup = 2;
	}
	else
	{
		if (m_imageRatio<m_ratioX)
			m_textureSetup = 3;
		else
			m_textureSetup = 4;
	}
}

void CRectangle::getSize(double &width, double &height)
{
	width = m_size[0];
	height = m_size[1];
}

void CRectangle::doGeometry()
{
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	if ((m_rectType==RT_SOLID)||(m_rectType==RT_SOLID_OUTLINE))
	{
		glBegin(GL_QUADS);
		glTexCoord2d(0.0, 0.0);
		glVertex2d(0.0, 0.0);
		
		glTexCoord2d(1.0, 0.0);
		glVertex2d(m_size[0], 0.0);
		
		glTexCoord2d(1.0, 1.0);
		glVertex2d(m_size[0], m_size[1]);
		
		glTexCoord2d(0.0, 1.0);
		glVertex2d(0.0, m_size[1]);
		
		glEnd();
	}

	if ((m_rectType==RT_OUTLINE)||(m_rectType==RT_SOLID_OUTLINE))
	{
		glPushAttrib(GL_LINE_BIT);
		if (m_lineType!=LT_SOLID)
		{
			if (m_lineType==LT_DASHED)
				m_lineStipple = 0x00FF;

			if (m_lineType==LT_DOTTED)
				m_lineStipple = 0x0101;

			glLineWidth((GLfloat)m_lineWidth);
			glLineStipple(m_lineFactor, m_lineStipple);
			glEnable(GL_LINE_STIPPLE);
		}
		m_lineColor->render();
		glBegin(GL_LINE_LOOP);		
		glVertex2d(0.0, 0.0);
		glVertex2d(m_size[0], 0.0);
		glVertex2d(m_size[0], m_size[1]);
		glVertex2d(0.0, m_size[1]);
		glEnd();
		glPopAttrib();
	}
	/*

	switch (m_textureSetup) {
	case 1:
		glTexCoord2d(0.0, 0.0);
		glVertex2d(0.0, 0.0);

		glTexCoord2d(1.0, 0.0);
		glVertex2d(m_size[0], 0.0);

		glTexCoord2d(1.0, 1.0*m_ratioX);
		glVertex2d(m_size[0], m_size[1]);

		glTexCoord2d(0.0, 1.0*m_ratioX);
		glVertex2d(0.0, m_size[1]);
		break;
	case 2:

		break;
	case 3:

		break;
	case 4:

		break;
	default:

		break;
	}

	*/
}

void CRectangle::setTexture(CTexture *texture)
{
	//
	// Let's snatch us some image info from the texture
	//

	if (texture->getImage()!=NULL)
	{
		CImage* image = texture->getImage();
		m_imageRatio = image->getRatio();				
	}
	CShape::setTexture(texture);
}

void CRectangle::setRectangleType(TRectangleType type)
{
	m_rectType = type;
}

void CRectangle::setLineType(TLineType type)
{
	m_lineType = type;
}

void CRectangle::setLineWidth(double width)
{
	m_lineWidth = width;
}

void CRectangle::setLineColor(CColor *color)
{
	m_lineColor = color;
}
