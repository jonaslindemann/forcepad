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

#ifndef _CFemGridSolverCORBA_h_
#define _CFemGridSolverCORBA_h_

#include "FemGrid.h"

class CFemGridSolverCORBA {
public:
	enum TErrorType {
		ET_NO_ERROR,
		ET_NO_ELEMENTS,
		ET_NO_BCS,
		ET_NO_LOADS,
		ET_UNSTABLE,
		ET_INVALID_MODEL,
		ET_LOAD_OUTSIDE_AE,
		ET_BC_OUTSIDE_AE
	};
private:
	TErrorType m_errorStatus;
	CFemGrid* m_femGrid;
	int m_argc;
	char** m_argv;
	double m_maxNodeValue;
public:
	CFemGridSolverCORBA();
	virtual ~CFemGridSolverCORBA();

	void execute();
	TErrorType getLastError();

	void setFemGrid(CFemGrid* femGrid);
	void setCommandLine(int argc, char** argv);
};

#endif 

