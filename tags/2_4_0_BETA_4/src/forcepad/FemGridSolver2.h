//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2008 Division of Structural Mecahnics, Lund University
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

#ifndef _CFemGridSolver2_h_
#define _CFemGridSolver2_h_

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

#include "Base.h"
#include "FemGrid2.h"

#include "calfem.h"


class CGSStatusMessageEvent {
public:
	virtual ~CGSStatusMessageEvent() {};
	virtual void onStatusMessage(const std::string& message, const int progress) = 0;
};

class CGSLogMessageEvent {
public:
	virtual ~CGSLogMessageEvent() {};
	virtual void onLogMessage(const std::string& context, const std::string& message) = 0;
};

class CGSContinueCalcEvent {
public:
	virtual ~CGSContinueCalcEvent() {};
	virtual bool onContinueCalc() = 0;
};

IvfSmartPointer(CFemGridSolver2);

/** 
 * Fem Internal solver class.
 *
 * This class is responsible for converting the FemModel class
 * into a finite element problem and then solving it.
 */
class CFemGridSolver2 : public CBase {
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
	enum TFilterType {
		FT_NO_FILTER,
		FT_OLE_SIGMUND,
		FT_BACK_PEDERSEN
	};
private:
	double m_maxNodeValue;
	double m_maxStressValue;

	double m_maxPosStressValue;
	double m_maxNegStressValue;

	double m_maxMisesStressValue;

	double m_minX;
	double m_maxX;
	double m_minY;
	double m_maxY;

	bool m_outputMatlab;
	std::string m_matlabFilename;

	TErrorType m_errorStatus;
	CFemGrid2* m_femGrid;

	double m_elementTreshold;
	double m_stiffnessScalefactor;
	double m_elasticModulus;
	double m_youngsModulus;
	double m_thickness;
	double m_constraintStiffnessScale;
	bool   m_reduceSystemMatrix;
	int m_ptype;
	double m_weight;
	double m_forceMagnitude;
	bool m_useWeight;

	BandLUMatrix			m_X;
	ColumnVector			m_a;
	ColumnVector			m_f;
	RowVector				m_Eq;
	int						m_nDof;

	// Optimisation parameters

	double m_optVolfrac;
	double m_optRmin;
	double m_optPenalty;
	double m_optMinChange;
	int m_optMaxLoops;
	TFilterType m_filterType;


	CGSStatusMessageEvent* m_statusMessageEvent;
	CGSLogMessageEvent* m_logMessageEvent;
	CGSContinueCalcEvent* m_continueCalcEvent;
public:
	/** FemInternalSolver class constructor. */
	CFemGridSolver2();

	/** FeminternalSolver class destructor. */
	virtual ~CFemGridSolver2();

	/* ---- Methods ----- */

	/** Finite element solver for solving active/inactive elements. */
	void execute();
	void execute_old();

	/** Finite element solver for solving active/inactive elements. */
	int assembleSystem(SymmetricBandMatrix& K);
	int assembleSystemOpt(SymmetricBandMatrix& K, Matrix& X, double penalty);
	void setupHinges();
	int calculateOptimalBandwidth();
	void setupForcesAndConstraints(bool& loadsDefined, bool& bcsDefined, bool& vectorBcsDefined, std::set<int>& uniqueDofs, std::set<int>& uniqueVectorDofs, vector<CConstraint*>& vectorConstraints, RowVector& prescribedValues);
	void assembleVectorConstraints(SymmetricBandMatrix& K, std::vector<CConstraint*>& vectorConstraints);
	void removeDoubleDofs(std::set<int>& uniqueDofs, RowVector& prescribedValues, Matrix& Bc);
	void computeElementForces();
	void computeElementForcesOpt(Matrix& X, double penalty);
	void computeReactionForces(std::vector<CConstraint*>& vectorConstraints);

	void objectiveFunctionAndSensitivity(Matrix& X, Matrix& dC, double penalty, double& c);
	ReturnMatrix optimalityCriteriaUpdate(Matrix& X, Matrix& dC, double volfrac, int nElements);
	ReturnMatrix sensitivityFilter1(Matrix& X, Matrix& dC, double rmin);
	ReturnMatrix sensitivityFilter2(Matrix& dC, double rmin);

	void executeOptimizer();

	void executeUpdate();

	void progressMessage(const std::string message, const int progress);
	void logMessage(const std::string context, const std::string message);
	bool continueCalc();

	/* ----- Get/set methods ----- */

	/** Set FemModel instance containing problem description. */
	void setFemGrid(CFemGrid2* femGrid);

	/** Returns current FemModel instance */
	CFemGrid2* getFemGrid();

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

	/** Set elastic modulus **/
	void setElasticModulus(double elasticModulus);

	/** Set thicknes **/
	void setThickness(double thickness);

	/** Set youngs modulus **/
	void setYoungsModulus(double youngsModulus);

	void setMatlabFilename(const char* name);
	void setOutputMatlab(bool flag);
	void setUseWeight(bool flag);
	void setForceMagnitude(double value);
	void setWeight(double weight);
	int getDofs();
	double getMaxNegStressValue();
	double getMaxPosStressValue();

	void setStiffnessScalefactor(const double scalefactor);
	double getStiffnessScalefactor();

	void setConstraintStiffnessScale(const double scalefactor);
	double getConstraintStiffnessScale();

	// Optimisation properties

	void setOptVolumeFraction(double fraction);
	double getOptVolumeFraction();

	void setOptRmin(double rmin);
	double getOptRmin();

	void setOptMinChange(double minChange);
	double getOptMinChange();

	void setOptMaxLoops(int loops);
	int getOptMaxLoops();

	void setOptFilterType(TFilterType filterType);
	TFilterType getOptFilterType();

	void setStatusMessageEvent(CGSStatusMessageEvent* eventMethod);
	void setLogMessageEvent(CGSLogMessageEvent* eventMethod);
	void setContinueCalcEvent(CGSContinueCalcEvent* eventMethod);
};

#endif 
