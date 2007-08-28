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

#ifndef _CElement_h_
#define _CElement_h_

#include "Base.h"
#include "Node.h"

IvfSmartPointer(CElement);

class CElement : public CBase {
private:
	std::vector<CNodePtr> m_nodes;
public:
	/** Color class constructor. */
	CElement();

	/** Color class destructor. */
	virtual ~CElement();

	IvfClassInfo("CElement",CBase);

	void setNodes(int number);
	void setNode(int idx, CNode* node);
	CNode* getNode(int idx);
};

#endif 
