//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2007 Division of Structural Mechanics, Lund University
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

#ifndef _CNode_h_
#define _CNode_h_

#include "Point.h"

IvfSmartPointer(CNode);

class CNode : public CPoint {
private:
	int m_dofs[2];
public:
	/** Color class constructor. */
	CNode();

	/** Color class destructor. */
	virtual ~CNode();

	IvfClassInfo("CNode",CBase);
	int enumerate(int start);
	void setDof(int idx, int value);
	int getDof(int idx);
};

#endif 

