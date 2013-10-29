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

#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

CCGIndicator::CCGIndicator()
{
	m_gravityArrow = false;
	m_indicatorSize = 6.0;

	m_direction[0] = 0.0;
	m_direction[1] = -1.0;

	m_arrowLength = 50.0;
	m_arrowSize = 10.0;
	m_arrowAngle = M_PI/6.0;
	
	this->initAngle();
}

CCGIndicator::~CCGIndicator()
{

}

void CCGIndicator::doGeometry()
{
	glLineWidth(2.0);
	glBegin(GL_LINES);
	glVertex2d(0.0 - m_indicatorSize/2.0, 0.0);
	glVertex2d(0.0 + m_indicatorSize/2.0, 0.0);
	glVertex2d(0.0, 0.0 - m_indicatorSize/2.0);
	glVertex2d(0.0, 0.0 + m_indicatorSize/2.0);
	glEnd();

	if (m_gravityArrow)
	{
		glPushMatrix();
		glTranslated(m_direction[0]*(m_indicatorSize+m_arrowLength), m_direction[1]*(m_indicatorSize+m_arrowLength), 0.0);

		glBegin(GL_LINES);
		glLineWidth(2.0);
		glVertex2d(0, 0);
		glVertex2d(0 - m_direction[0]*m_arrowLength, 0 - m_direction[1]*m_arrowLength);
		glVertex2d(0, 0);
		glVertex2d(0 - m_leftPos[0], 0 - m_leftPos[1]);
		glVertex2d(0, 0);
		glVertex2d(0 - m_rightPos[0], 0 - m_rightPos[1]);
		glLineWidth(1.0);
		glEnd();

		glPopMatrix();
	}
}

void CCGIndicator::setArrowLength(double length)
{
	m_arrowLength = length;
}

double CCGIndicator::getArrowLength()
{
	return m_arrowLength;
}

void CCGIndicator::setShowGravityArrow(bool flag)
{
	m_gravityArrow = flag;
}

bool CCGIndicator::getShowGravityArrow()
{
	return m_gravityArrow; 
}

void CCGIndicator::setIndicatorSize(double size)
{
	m_indicatorSize = size;
}

double CCGIndicator::getIndicatorSize()
{
	return m_indicatorSize;
}

void CCGIndicator::initAngle()
{
	m_angle = atan2(m_direction[1], m_direction[0]);	

	m_leftPos[0] = m_arrowSize*cos(m_angle+m_arrowAngle);
	m_leftPos[1] = m_arrowSize*sin(m_angle+m_arrowAngle);
	m_rightPos[0] = m_arrowSize*cos(m_angle-m_arrowAngle);
	m_rightPos[1] = m_arrowSize*sin(m_angle-m_arrowAngle);
}

void CCGIndicator::setArrowSize(double size)
{
	m_arrowSize = size;
}

double CCGIndicator::getArrowSize()
{
	return m_arrowSize;
}
