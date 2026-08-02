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

#include "CGIndicator.h"
#include "UiSettings.h"

#include "Renderer2D.h"
#include "Constants.h"
#include <cmath>

namespace fp {

CGIndicator::CGIndicator()
{
	m_gravityArrow = false;
	m_indicatorSize = 6.0;

	m_direction[0] = 0.0;
	m_direction[1] = -1.0;

	m_arrowLength = 50.0;
	m_arrowSize = 10.0;
	m_arrowAngle = ivf2d::PI/6.0;
	
	this->initAngle();
}

CGIndicator::~CGIndicator()
{

}

void CGIndicator::doGeometry()
{
	float dpr = (float)UiSettings::getInstance()->getDevicePixelRatio();
	ivf2d::Renderer2D &r = ivf2d::Renderer2D::instance();

	r.beginLines(2.0f * dpr);
	r.vertex((float)(0.0 - m_indicatorSize/2.0), 0.0f);
	r.vertex((float)(0.0 + m_indicatorSize/2.0), 0.0f);
	r.vertex(0.0f, (float)(0.0 - m_indicatorSize/2.0));
	r.vertex(0.0f, (float)(0.0 + m_indicatorSize/2.0));
	r.end();

	if (m_gravityArrow)
	{
		r.pushTransform();
		r.translate((float)(m_direction[0]*(m_indicatorSize+m_arrowLength)),
		            (float)(m_direction[1]*(m_indicatorSize+m_arrowLength)));

		r.beginLines(2.0f * dpr);
		r.vertex(0.0f, 0.0f);
		r.vertex((float)(0 - m_direction[0]*m_arrowLength), (float)(0 - m_direction[1]*m_arrowLength));
		r.vertex(0.0f, 0.0f);
		r.vertex((float)(0 - m_leftPos[0]), (float)(0 - m_leftPos[1]));
		r.vertex(0.0f, 0.0f);
		r.vertex((float)(0 - m_rightPos[0]), (float)(0 - m_rightPos[1]));
		r.end();

		r.popTransform();
	}
}

void CGIndicator::setArrowLength(double length)
{
	m_arrowLength = length;
}

double CGIndicator::getArrowLength()
{
	return m_arrowLength;
}

void CGIndicator::setShowGravityArrow(bool flag)
{
	m_gravityArrow = flag;
}

bool CGIndicator::getShowGravityArrow()
{
	return m_gravityArrow; 
}

void CGIndicator::setIndicatorSize(double size)
{
	m_indicatorSize = size;
}

double CGIndicator::getIndicatorSize()
{
	return m_indicatorSize;
}

void CGIndicator::initAngle()
{
	m_angle = atan2(m_direction[1], m_direction[0]);	

	m_leftPos[0] = m_arrowSize*cos(m_angle+m_arrowAngle);
	m_leftPos[1] = m_arrowSize*sin(m_angle+m_arrowAngle);
	m_rightPos[0] = m_arrowSize*cos(m_angle-m_arrowAngle);
	m_rightPos[1] = m_arrowSize*sin(m_angle-m_arrowAngle);
}

void CGIndicator::setArrowSize(double size)
{
	m_arrowSize = size;
}

double CGIndicator::getArrowSize()
{
	return m_arrowSize;
}

} // namespace fp
