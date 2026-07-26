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

#include "Renderer2D.h"

namespace ivf2d {

Rectangle::Rectangle()
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
	m_lineColor = Color::create();
	m_lineColor->setColor(0.0f, 0.0f, 0.0f, 1.0f);
}

Rectangle::~Rectangle()
{

}

void Rectangle::setSize(double width, double height)
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

void Rectangle::getSize(double &width, double &height)
{
	width = m_size[0];
	height = m_size[1];
}

void Rectangle::doGeometry()
{
	Renderer2D &r = Renderer2D::instance();

	if ((m_rectType==RT_SOLID)||(m_rectType==RT_SOLID_OUTLINE))
	{
		r.beginQuads();
		r.texCoord(0.0f, 0.0f);
		r.vertex(0.0f, 0.0f);

		r.texCoord(1.0f, 0.0f);
		r.vertex((float)m_size[0], 0.0f);

		r.texCoord(1.0f, 1.0f);
		r.vertex((float)m_size[0], (float)m_size[1]);

		r.texCoord(0.0f, 1.0f);
		r.vertex(0.0f, (float)m_size[1]);

		r.end();
	}

	if ((m_rectType==RT_OUTLINE)||(m_rectType==RT_SOLID_OUTLINE))
	{
		m_lineColor->render();

		// The outline is drawn as four explicit segments (rather than a line
		// loop) so dashed rendering gets continuous per-segment distances.
		const float w = (float)m_size[0];
		const float h = (float)m_size[1];
		if (m_lineType != LT_SOLID)
		{
			// glLineStipple periods, mapped to shader dash periods (px):
			//   LT_DASHED 0x00FF -> 16 (8 on / 8 off)
			//   LT_DOTTED 0x0101 ->  4 (dotted approximation)
			const float period = (m_lineType == LT_DOTTED) ? 4.0f : 16.0f;
			r.beginDashedLines((float)m_lineWidth, period * (float)m_lineFactor);
		}
		else
		{
			r.beginLines((float)m_lineWidth);
		}
		r.vertex(0.0f, 0.0f); r.vertex(w, 0.0f);
		r.vertex(w, 0.0f);    r.vertex(w, h);
		r.vertex(w, h);       r.vertex(0.0f, h);
		r.vertex(0.0f, h);    r.vertex(0.0f, 0.0f);
		r.end();
	}
}

void Rectangle::setTexture(TexturePtr texture)
{
	//
	// Let's snatch us some image info from the texture
	//

	if (texture->getImage()!=nullptr)
	{
		Image* image = texture->getImage();
		m_imageRatio = image->getRatio();				
	}
	Shape::setTexture(texture);
}

void Rectangle::setRectangleType(TRectangleType type)
{
	m_rectType = type;
}

void Rectangle::setLineType(TLineType type)
{
	m_lineType = type;
}

void Rectangle::setLineWidth(double width)
{
	m_lineWidth = width;
}

void Rectangle::setLineColor(ColorPtr color)
{
	m_lineColor = color;
}

} // namespace ivf2d
