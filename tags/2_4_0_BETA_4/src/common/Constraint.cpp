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

#include "Constraint.h"

// ------------------------------------------------------------
CConstraint::CConstraint ()
		:CShape()
{
	m_size = 9.0;
	m_constraintType = CT_XY;
	m_value = 0.0;
	m_vectorSize = 30.0;
	m_vectorRadius = 5.0;
	m_reactionForce = new CReactionForce();
	m_visibleReaction = false;
}

// ------------------------------------------------------------
CConstraint::~CConstraint ()
{
}

// ------------------------------------------------------------
void CConstraint::doGeometry()
{
	glLineWidth(2.0);
	if (m_constraintType==CT_HINGE)
	{
		glEnable(GL_LINE_STIPPLE);
		glLineStipple(3, 0xAAAA);
	}
	glBegin(GL_LINES);
	
	switch (m_constraintType) {
	case CT_X:
		glVertex2d(0, 0-m_size/2.0);
		glVertex2d(0, 0+m_size/2.0);
		break;
	case CT_Y:
		glVertex2d(0-m_size/2.0, 0);
		glVertex2d(0+m_size/2.0, 0);
		break;
	case CT_XY:
		glVertex2d(0, 0-m_size/2.0);
		glVertex2d(0, 0+m_size/2.0);
		glVertex2d(0-m_size/2.0, 0);
		glVertex2d(0+m_size/2.0, 0);
		break;
	case CT_SCALAR:
		glVertex2d(0, 0-m_size/2.0);
		glVertex2d(0, 0+m_size/2.0);
		glVertex2d(0-m_size/2.0, 0);
		glVertex2d(0+m_size/2.0, 0);
		break;
	case CT_VECTOR:
		glVertex2d(-m_vectorRadius*2*m_direction[0], -m_vectorRadius*2*m_direction[1]);
		glVertex2d(-m_vectorSize*m_direction[0], -m_vectorSize*m_direction[1]);
		break;
	case CT_HINGE:
		glVertex2d(-m_vectorRadius*m_direction[0], -m_vectorRadius*m_direction[1]);
		glVertex2d(-m_hingeLength*m_direction[0], -m_hingeLength*m_direction[1]);
		glVertex2d(m_vectorRadius*m_direction[0], m_vectorRadius*m_direction[1]);
		glVertex2d(m_hingeLength*m_direction[0], m_hingeLength*m_direction[1]);
		break;
	default:
		
		break;
	}

	glEnd();

	if (m_constraintType==CT_HINGE)
	{
		glDisable(GL_LINE_STIPPLE);
	}

	if (m_constraintType==CT_VECTOR)
	{
		double x, y, ox, oy, angle;

		ox = -m_vectorRadius*m_direction[0];
		oy = -m_vectorRadius*m_direction[1];

		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_POLYGON);
		for (angle = 0.0; angle<2*M_PI; angle+=2*M_PI/20.0)
		{
			x = ox + (m_vectorRadius-1.0)*cos(angle);
			y = oy + (m_vectorRadius-1.0)*sin(angle);
			glVertex2d(x, y);
		}
		glEnd();
		
		this->getColor()->render();
		glBegin(GL_LINE_LOOP);
		for (angle = 0.0; angle<2*M_PI; angle+=2*M_PI/20.0)
		{
			x = ox + m_vectorRadius*cos(angle);
			y = oy + m_vectorRadius*sin(angle);
			glVertex2d(x, y);
		}
		glEnd();

		if (m_visibleReaction)
			m_reactionForce->render();
	}

	if (m_constraintType==CT_HINGE)
	{
		double x, y, ox, oy, angle;

		ox = 0.0;
		oy = 0.0;

		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_POLYGON);
		for (angle = 0.0; angle<2*M_PI; angle+=2*M_PI/20.0)
		{
			x = ox + (m_vectorRadius-1.0)*cos(angle);
			y = oy + (m_vectorRadius-1.0)*sin(angle);
			glVertex2d(x, y);
		}
		glEnd();
		
		this->getColor()->render();
		glBegin(GL_LINE_LOOP);
		for (angle = 0.0; angle<2*M_PI; angle+=2*M_PI/20.0)
		{
			x = ox + m_vectorRadius*cos(angle);
			y = oy + m_vectorRadius*sin(angle);
			glVertex2d(x, y);
		}
		glEnd();
	}
}

void CConstraint::getHingeStart(double &x, double &y)
{
	double xx, yy;
	this->getPosition(xx, yy);
	x = xx - m_hingeLength*m_direction[0];
	y = yy - m_hingeLength*m_direction[1];
}

void CConstraint::getHingeEnd(double &x, double &y)
{
	double xx, yy;
	this->getPosition(xx, yy);
	x = xx + m_hingeLength*m_direction[0];
	y = yy + m_hingeLength*m_direction[1];
}

// ------------------------------------------------------------
void CConstraint::setConstraintType(TConstraintType type)
{
	m_constraintType = type;
}

// ------------------------------------------------------------
CConstraint::TConstraintType CConstraint::getConstraintType()
{
	return m_constraintType;
}

// ------------------------------------------------------------
void CConstraint::setSize(double size)
{
	m_size = size;
}

// ------------------------------------------------------------
double CConstraint::getSize()
{
	return m_size;
}

// ------------------------------------------------------------
void CConstraint::saveToStream(ostream &out)
{
	switch (m_constraintType) {
	case CT_X:
		out << 0 << " ";
		break;
	case CT_Y:
		out << 1 << " ";
		break;
	case CT_XY:
		out << 2 << " ";
		break;
	case CT_VECTOR:
		out << 3 << " ";
		break;
	case CT_HINGE:
		out << 4 << " ";
		break;
	default:
		out << 2 << " ";
		break;
	}
	out << m_size << " ";

	if (m_constraintType == CT_VECTOR)
		out << m_direction[0] << " " << m_direction[1] << " ";

	if (m_constraintType == CT_HINGE)
		out << m_direction[0] << " " << m_direction[1] << " " << m_hingeLength << " ";
}

// ------------------------------------------------------------
void CConstraint::readFromStream(istream &in)
{
	int constraintType;
	in >> constraintType;

	switch (constraintType) {
	case 0:
		m_constraintType = CT_X;
		break;
	case 1:
		m_constraintType = CT_Y;
		break;
	case 2:
		m_constraintType = CT_XY;
		break;
	case 3:
		m_constraintType = CT_VECTOR;
		break;
	case 4:
		m_constraintType = CT_HINGE;
		break;
	default:
		m_constraintType = CT_XY;
		break;
	}
	
	in >> m_size;

	if (m_constraintType == CT_VECTOR)
	{
		in >> m_direction[0] >> m_direction[1];
		m_reactionForce->setDirection(m_direction[0], m_direction[1]);
	}

	if (m_constraintType == CT_HINGE)
		in >> m_direction[0] >> m_direction[1] >> m_hingeLength;
}

// ------------------------------------------------------------
void CConstraint::setValue(double value)
{
	m_value = value;
}

// ------------------------------------------------------------
double CConstraint::getValue()
{
	return m_value;
}

// ------------------------------------------------------------
void CConstraint::assignFrom(CConstraint *constraint)
{
	m_constraintType = constraint->getConstraintType();
	m_size = constraint->getSize();

	double ex, ey;
	constraint->getDirection(ex, ey);
	this->setColor(constraint->getColor());
	m_direction[0] = ex;
	m_direction[1] = ey;
	m_reactionForce->setDirection(ex, ey);
}

// ------------------------------------------------------------
void CConstraint::setDirection(double ex, double ey)
{
	double l = sqrt(pow(ex,2)+pow(ey,2));
	m_hingeLength = l;
	m_direction[0] = ex/l;
	m_direction[1] = ey/l;
	m_reactionForce->setDirection(ex, ey);

}

// ------------------------------------------------------------
void CConstraint::getDirection(double &ex, double &ey)
{
	ex = m_direction[0];
	ey = m_direction[1];
}

// ------------------------------------------------------------
void CConstraint::setVectorSize(double size)
{
	m_vectorSize = size;
}

// ------------------------------------------------------------
double CConstraint::getVectorSize()
{
	return m_vectorSize;
}

// ------------------------------------------------------------
void CConstraint::setShowReactionForce(bool flag)
{
	m_visibleReaction = flag;
}

// ------------------------------------------------------------
bool CConstraint::getShowReactionForce()
{
	return m_visibleReaction;
}

// ------------------------------------------------------------
CReactionForce* CConstraint::getReactionForce()
{
	return m_reactionForce;
}

