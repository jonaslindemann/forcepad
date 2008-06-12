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

#ifndef _CForcePadClipboard_h_
#define _CForcePadClipboard_h_

#include "Clipboard.h"
#include "ForceSelection.h"
#include "ConstraintSelection.h"
#include "FemGrid2.h"

IvfSmartPointer(CForcePadClipboard);

class CForcePadClipboard : public CClipboard {
private:
	CForceSelection* m_forceSelection;
	CConstraintSelection* m_constraintSelection;
	CFemGrid2* m_grid;
	int m_drawingOffsetX;
	int m_drawingOffsetY;
public:
	CForcePadClipboard();
	virtual ~CForcePadClipboard();

	IvfClassInfo("CForcePadClipboard",CClipboard);

	void render(int x, int y);

	void setFemGrid(CFemGrid2* grid);
	void setDrawingOffset(int x, int y);

	virtual void copy(int x1, int y1, int x2, int y2);
	virtual void cut(int x1, int y1, int x2, int y2);
	virtual void paste(int x, int y);
};

#endif 
