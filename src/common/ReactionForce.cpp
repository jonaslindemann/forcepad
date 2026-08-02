//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2008 Division of Structural Mecahnics, Lund University
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

#include "ReactionForce.h"

#include "Renderer2D.h"
#include "UiSettings.h"
#include "Constants.h"
#include <cmath>

namespace fp {

// ------------------------------------------------------------
ReactionForce::ReactionForce ()
{
	m_value = 1.0;
	m_direction[0] = 0.0;
	m_direction[1] = 1.0;
	m_length = 50.0;
	m_offset = 40.0;
	m_arrowSize = 10.0;
	m_arrowAngle = ivf2d::PI/6.0;
	this->initAngle();
}

// ------------------------------------------------------------
ReactionForce::~ReactionForce ()
{

}

// ------------------------------------------------------------
void ReactionForce::setValue(double value)
{
	m_value = value;
}

// ------------------------------------------------------------
double ReactionForce::getValue()
{
	return m_value;
}

// ------------------------------------------------------------
void ReactionForce::setDirection(double vx, double vy)
{
	double l = sqrt(pow(vx,2)+pow(vy,2));
	m_direction[0] = vx/l;
	m_direction[1] = vy/l;
	this->initAngle();
}

// ------------------------------------------------------------
void ReactionForce::getDirection(double &vx, double &vy)
{
	vx = m_direction[0];
	vy = m_direction[1];
	this->initAngle();
}

// ------------------------------------------------------------
void ReactionForce::setLength(double length)
{
	m_length = length;
	this->initAngle();
}

// ------------------------------------------------------------
double ReactionForce::getLength()
{
	return m_length;
}

// ------------------------------------------------------------
void ReactionForce::doGeometry()
{
	double x;
	double y;

    double oldArrowSize = m_arrowSize;
	
	this->getPosition(x, y);

	float lineWidth;
	{
        float dpr = (float)UiSettings::getInstance()->getDevicePixelRatio();
        if (UiSettings::getInstance()->getLineThickness()>0.0)
            lineWidth = (float)(UiSettings::getInstance()->getLineThickness() * dpr);
        else
            lineWidth = 2.0f * dpr;
	}

	if (UiSettings::getInstance()->getSymbolLength()>0.0)
	{
		this->setArrowSize(UiSettings::getInstance()->getSymbolLength() * 0.25);
	}

	ivf2d::Renderer2D &r = ivf2d::Renderer2D::instance();
	r.color(1.0f, 0.5f, 0.0f, 1.0f);

	if (m_length>=0.0)
	{
		r.beginLines(lineWidth);
		r.vertex((float)(-m_direction[0]*m_offset), (float)(-m_direction[1]*m_offset));
		r.vertex((float)(0 -m_direction[0]*m_offset - m_direction[0]*m_length), (float)(0 -m_direction[1]*m_offset - m_direction[1]*m_length));
		r.end();

		r.pushTransform();
		r.translate((float)(0 -m_direction[0]*m_offset), (float)(0 -m_direction[1]*m_offset));
		r.beginLines(lineWidth);
		r.vertex(0.0f, 0.0f);
		r.vertex((float)(0 - m_leftPos[0]), (float)(0 - m_leftPos[1]));
		r.vertex(0.0f, 0.0f);
		r.vertex((float)(0 - m_rightPos[0]), (float)(0 - m_rightPos[1]));
		r.end();
		r.popTransform();
	}
	else
	{
		r.beginLines(lineWidth);
		r.vertex((float)(-m_direction[0]*m_offset), (float)(-m_direction[1]*m_offset));
		r.vertex((float)(-m_direction[0]*m_offset + m_direction[0]*m_length), (float)(-m_direction[1]*m_offset + m_direction[1]*m_length));
		r.end();

		r.pushTransform();
		r.translate((float)(-m_direction[0]*m_offset + m_direction[0]*m_length), (float)(-m_direction[1]*m_offset + m_direction[1]*m_length));
		r.beginLines(lineWidth);
		r.vertex(0.0f, 0.0f);
		r.vertex((float)(0 + m_leftPos[0]), (float)(0 + m_leftPos[1]));
		r.vertex(0.0f, 0.0f);
		r.vertex((float)(0 + m_rightPos[0]), (float)(0 + m_rightPos[1]));
		r.end();
		r.popTransform();
	}

	if (UiSettings::getInstance()->getSymbolLength()>0.0)
		this->setArrowSize(oldArrowSize);
}

// ------------------------------------------------------------
void ReactionForce::setArrowAngle(double angle)
{
	m_arrowAngle = angle*2.0*ivf2d::PI/360.0;
	this->initAngle();
}

// ------------------------------------------------------------
double ReactionForce::getArrowAngle()
{
	return m_arrowAngle*360.0/ivf2d::PI/2.0;
}

// ------------------------------------------------------------
void ReactionForce::initAngle()
{
	m_angle = atan2(m_direction[1], m_direction[0]);	

	m_leftPos[0] = m_arrowSize*cos(m_angle+m_arrowAngle);
	m_leftPos[1] = m_arrowSize*sin(m_angle+m_arrowAngle);
	m_rightPos[0] = m_arrowSize*cos(m_angle-m_arrowAngle);
	m_rightPos[1] = m_arrowSize*sin(m_angle-m_arrowAngle);
}

// ------------------------------------------------------------
void ReactionForce::setArrowSize(double size)
{
	m_arrowSize = size;
	this->initAngle();
}

// ------------------------------------------------------------
double ReactionForce::getArrowSize()
{
	return m_arrowSize;
}

// ------------------------------------------------------------
double ReactionForce::getValueX()
{
	return m_direction[0]*m_value;
}

// ------------------------------------------------------------
double ReactionForce::getValueY()
{
	return m_direction[1]*m_value;
}

} // namespace fp
