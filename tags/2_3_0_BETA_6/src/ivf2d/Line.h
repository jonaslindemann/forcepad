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

#ifndef _CLine_h_
#define _CLine_h_

#include "Vec3d.h"
#include "Shape.h"

IvfSmartPointer(CLine);

class CLine : public CShape {
private:
	void initLine();
	int m_width;
	int m_startPos[2];
	int m_endPos[2];
	CVec3d m_p1;
	CVec3d m_p2;
	CVec3d m_p3;
	CVec3d m_p4;
public:
	CLine();
	virtual ~CLine();

	IvfClassInfo("CLine",CShape);

	void doGeometry();

	int getWidth();
	void setEndPos(int x, int y);
	void setStartPos(int x, int y);
	void setWidth(int width);
};

#endif 
