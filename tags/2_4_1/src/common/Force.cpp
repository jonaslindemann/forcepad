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

#include "Force.h"

#include "UiSettings.h"

// ------------------------------------------------------------
CForce::CForce ()
{
	m_value = 1.0;
	m_direction[0] = 0.0;
	m_direction[1] = 1.0;
	m_length = 50.0;
	m_arrowSize = 10.0;
	m_arrowAngle = M_PI/6.0;
	m_forceType = FT_VECTOR;
	this->initAngle();
}

// ------------------------------------------------------------
CForce::~CForce ()
{

}

// ------------------------------------------------------------
void CForce::setValue(double value)
{
	m_value = value;
}

// ------------------------------------------------------------
double CForce::getValue()
{
	return m_value;
}

// ------------------------------------------------------------
void CForce::setDirection(double vx, double vy)
{
	double l = sqrt(pow(vx,2)+pow(vy,2));
	m_direction[0] = vx/l;
	m_direction[1] = vy/l;
	this->initAngle();
}

// ------------------------------------------------------------
void CForce::getDirection(double &vx, double &vy)
{
	vx = m_direction[0];
	vy = m_direction[1];
	this->initAngle();
}

// ------------------------------------------------------------
void CForce::setLength(double length)
{
	m_length = length;
}

// ------------------------------------------------------------
double CForce::getLength()
{
	return m_length;
}

// ------------------------------------------------------------
void CForce::doGeometry()
{
	double x;
	double y;
	double length;
	double arrowLength;
	double oldLength;
	double oldArrowLength;

	this->getPosition(x, y);
	
	switch (m_forceType) {
	case FT_VECTOR:
		if (CUiSettings::getInstance()->getLineThickness()>0.0)
			glLineWidth(CUiSettings::getInstance()->getLineThickness());
		else
			glLineWidth(2.0);

		if (CUiSettings::getInstance()->getSymbolLength()>0.0)
		{
			length = CUiSettings::getInstance()->getSymbolLength();
			arrowLength = 0.25*length;

			oldLength = m_length;
			oldArrowLength = m_arrowSize;

			this->setLength(length);
			this->setArrowSize(arrowLength);
		}

		glBegin(GL_LINES);
		//glVertex2d(0, 0);
		glVertex2d(0 + m_direction[0]*m_length*0.02, 0 + m_direction[1]*m_length*0.02);
		glVertex2d(0 - m_direction[0]*m_length, 0 - m_direction[1]*m_length);
		glVertex2d(0, 0);
		glVertex2d(0 - m_leftPos[0], 0 - m_leftPos[1]);
		glVertex2d(0, 0);
		glVertex2d(0 - m_rightPos[0], 0 - m_rightPos[1]);
		glEnd();

		if (CUiSettings::getInstance()->getSymbolLength()>0.0)
		{
			this->setLength(oldLength);
			this->setArrowSize(oldArrowLength);
		}
		break;
	case FT_SCALAR:
		
		break;
	default:
		
		break;
	}
}

// ------------------------------------------------------------
void CForce::setArrowAngle(double angle)
{
	m_arrowAngle = angle*2.0*M_PI/360.0;
	this->initAngle();
}

// ------------------------------------------------------------
double CForce::getArrowAngle()
{
	return m_arrowAngle*360.0/M_PI/2.0;
}

// ------------------------------------------------------------
void CForce::initAngle()
{
	m_angle = atan2(m_direction[1], m_direction[0]);	

	m_leftPos[0] = m_arrowSize*cos(m_angle+m_arrowAngle);
	m_leftPos[1] = m_arrowSize*sin(m_angle+m_arrowAngle);
	m_rightPos[0] = m_arrowSize*cos(m_angle-m_arrowAngle);
	m_rightPos[1] = m_arrowSize*sin(m_angle-m_arrowAngle);
}

// ------------------------------------------------------------
void CForce::setArrowSize(double size)
{
	m_arrowSize = size;
	this->initAngle();
}

// ------------------------------------------------------------
double CForce::getArrowSize()
{
	return m_arrowSize;
}

// ------------------------------------------------------------
void CForce::saveToStream(ostream &out)
{
	out << m_value << " ";
	out << m_direction[0] << " ";
	out << m_direction[1] << " ";
	out << m_length << " ";
	out << m_arrowSize << " ";
	out << m_arrowAngle << endl;
}

// ------------------------------------------------------------
void CForce::readFromStream(istream &in)
{
	in >> m_value;
	in >> m_direction[0];
	in >> m_direction[1];
	in >> m_length;
	in >> m_arrowSize;
	in >> m_arrowAngle;
	this->initAngle();
}

// ------------------------------------------------------------
void CForce::assignFrom(CForce *force)
{
	double vx, vy;

	m_value = force->getValue();
	m_length = force->getLength();
	m_arrowSize = force->getArrowSize();
	m_arrowAngle = force->getArrowAngle()*2.0*M_PI/360.0;
	force->getDirection(vx, vy);
	m_direction[0] = vx;
	m_direction[1] = vy;
	this->initAngle();
}

// ------------------------------------------------------------
double CForce::getValueX()
{
	return m_direction[0]*m_value;
}

// ------------------------------------------------------------
double CForce::getValueY()
{
	return m_direction[1]*m_value;
}
