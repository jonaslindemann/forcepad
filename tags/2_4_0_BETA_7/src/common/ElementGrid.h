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

#ifndef _CElementGrid_h_
#define _CElementGrid_h_

#include "Base.h"
#include "Node.h"
#include "ElementGridCell.h"

IvfSmartPointer(CElementGrid);

class CElementGrid : public CBase {
private:
	std::vector< std::vector<CNodePtr> > m_nodes;
	std::vector< std::vector<CElementGridCellPtr> > m_elementGrid;
	int m_rows;
	int m_cols;

	double m_width;

	void initGrid();
public:
	CElementGrid();
	virtual ~CElementGrid();

	void setSize(int rows, int cols);
	void setWidth(double width);

	IvfClassInfo("CElementGrid",CBase);
};

#endif 

