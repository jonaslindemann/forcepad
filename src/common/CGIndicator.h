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

#ifndef _CCGIndicator_h_
#define _CCGIndicator_h_

#include "Shape.h"

IvfSmartPointer(CCGIndicator);

class CCGIndicator : public CShape {
private:
	double m_arrowLength;
	bool m_gravityArrow;
	double m_indicatorSize;

	double m_direction[2];
	double m_angle;
	double m_arrowAngle;
	double m_arrowSize;
	double m_leftPos[2];
	double m_rightPos[2];

	void initAngle();
public:
	CCGIndicator();
	virtual ~CCGIndicator();

	IvfClassInfo("CCGIndicator",CShape);

	void setArrowLength(double length);
	double getArrowLength();

	void setShowGravityArrow(bool flag);
	bool getShowGravityArrow();

	void setIndicatorSize(double size);
	double getIndicatorSize();

	void setArrowSize(double size);
	double getArrowSize();

	virtual void doGeometry();
};

#endif 
