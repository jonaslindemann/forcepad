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

#ifndef _CFemGridSolver_h_
#define _CFemGridSolver_h_

#define BS_NO_ERROR			0 
#define BS_NO_NODES			1
#define BS_NO_ELEMENTS		2
#define BS_NO_BC			3
#define BS_NO_LOADS			4
#define BS_UNSTABLE			5
#define BS_INVALID_MODEL	6
#define BS_LOAD_OUTSIDE_AE  7
#define BS_BC_OUTSIDE_AE    8

//#include "FemModel.h"

#include "FemGrid.h"

/** 
 * Fem Internal solver class.
 *
 * This class is responsible for converting the FemModel class
 * into a finite element problem and then solving it.
 */
class CFemGridSolver {
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
	double m_maxNodeValue;
	double m_maxStressValue;

	double m_maxPosStressValue;
	double m_maxNegStressValue;

	double m_weight;
	double m_forceMagnitude;
	bool m_useWeight;

	TErrorType m_errorStatus;
	CFemGrid* m_femGrid;
	double m_elementTreshold;
public:
	void setUseWeight(bool flag);
	void setForceMagnitude(double value);
	void setWeight(double weight);
	int getDofs();
	double getMaxNegStressValue();
	double getMaxPosStressValue();

	/** FemInternalSolver class constructor. */
	CFemGridSolver();

	/** FeminternalSolver class destructor. */
	virtual ~CFemGridSolver();

	// Methods

	/** Finite element solver for solving active/inactive elements. */
	void execute();

	// Get/set methods

	/** Set FemModel instance containing problem description. */
	void setFemGrid(CFemGrid* femGrid);

	/** Returns current FemModel instance */
	CFemGrid* getFemGrid();

	/** Returns error status from finite element solver. */
	TErrorType getLastError();

	/** 
	 * Sets element treshold for active elements.
	 *
	 * All element with stiffness < \c treshold will be
	 * deactivated and not used when solving the problem.
	 */
	void setElementTreshold(double treshold);

	/** Returns maximum nodal deformation. */
	double getMaxNodeValue();

	/** Returns maximum stress value. */
	double getMaxStressValue();
};

#endif 
