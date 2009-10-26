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

#include "ElementGrid.h"

CElementGrid::CElementGrid()
{
	m_rows = -1;
	m_cols = -1;
	m_width = 1.0;
}

CElementGrid::~CElementGrid()
{

}

void CElementGrid::initGrid()
{
	//
	//  o----o----o----| |----o----o----o
	//  
	//  |<----------------------------->|
	//
	//              m_width
	//              m_cols
	//
	//              xStep = m_width / m_cols
	//

	m_nodes.clear();
	m_nodes.resize(m_rows+1);

	int i, j;

	for (i=0; i<(int)m_nodes.size(); i++)
	{
		for (j=0; j<=m_cols; j++)
		{
			CNodePtr node = new CNode();
			m_nodes[i].push_back(node);
		}
	}
}

void CElementGrid::setSize(int rows, int cols)
{
	m_rows = rows;
	m_cols = cols;

	initGrid();
}

void CElementGrid::setWidth(double width)
{

}
