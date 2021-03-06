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

#ifdef WIN32
#include <windows.h>
#endif

#include "FemGridSolver2.h"
#include "matlabgen.h"
#include "LogWindow.h"

#include <set>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <climits>
#include <ctime>

#include <FL/Fl.h>

CFemGridSolver2::CFemGridSolver2()
{
	so_print("CFemGridSolver2","Constructed.");
	m_maxNodeValue = -1.0e300;
	m_maxStressValue = -1.0e300;
	m_maxMisesStressValue = -1.0e300;
	m_errorStatus = CFemGridSolver2::ET_NO_ERROR;
	m_elementTreshold = 1.0e-5;
	m_forceMagnitude = 1.0;
	m_weight = 0.0;
	m_useWeight = false;
	m_outputMatlab = false;
	m_stiffnessScalefactor = 1.0;
	m_elasticModulus = 2.1e9;
	m_youngsModulus = 0.35;
	m_thickness = 1.0;
	m_ptype = 1;
	m_constraintStiffnessScale = 1e6;
	m_reduceSystemMatrix = false;

	// Optimisation parameters

	m_optPenalty = 3.0;
	m_optVolfrac = 0.5;
	m_optMinChange = 0.01;
	m_optMaxLoops = 100;
	//m_optRmin = 2.75;
	m_optRmin = 1.75;
	m_filterType = FT_SHARP_CONTOURING;
	m_scaleToAbsoluteSize = true;
	m_rminInPixels = true;

	// Events

	m_statusMessageEvent = NULL;
	m_logMessageEvent = NULL;
	m_continueCalcEvent = NULL;
}

CFemGridSolver2::~CFemGridSolver2()
{
	so_print("CFemGridSolver2","Destroyed.");
}

bool CFemGridSolver2::continueCalc()
{
	Fl::check();
	Fl::flush();
	if (m_continueCalcEvent!=NULL)
		return m_continueCalcEvent->onContinueCalc();
	else
		return true;
}

void CFemGridSolver2::progressMessage(const std::string message, const int progress)
{
	if (m_statusMessageEvent!=NULL)
		m_statusMessageEvent->onStatusMessage(message, progress);
}

void CFemGridSolver2::logMessage(const std::string context, const std::string message)
{
	if (m_logMessageEvent!=NULL)
	{
		m_logMessageEvent->onLogMessage(context, message);
	}
}

CFemGridSolver2::TErrorType CFemGridSolver2::getLastError()
{
	return m_errorStatus;
}

void CFemGridSolver2::setFemGrid(CFemGrid2 *femGrid)
{
	m_femGrid = femGrid;
}

CFemGrid2* CFemGridSolver2::getFemGrid()
{
	return m_femGrid;
}

double CFemGridSolver2::getMaxStressValue()
{
	return m_maxStressValue;
}

double CFemGridSolver2::getMaxNodeValue()
{
	return m_maxNodeValue;
}

void CFemGridSolver2::execute_old()
{
	int i, j, l;
	int rows, cols;
	double elementValue;
	double ex[4];
	double ey[4];
	int topo[8];

	int maxBandwidth;
	int bwLeftRight;
	int bwBottomTop;

#ifdef WIN32
	DWORD startTime = timeGetTime();
#endif
    
	//
	// Reset error status
	//

	m_errorStatus = CFemGridSolver2::ET_NO_ERROR;

	//
	// Apply hinge constraints directly to stiffness grid
	//

	CConstraint* pointConstraint = m_femGrid->getFirstPointConstraint();

	while (pointConstraint!=NULL) 
	{
		switch (pointConstraint->getConstraintType()) {
		case CConstraint::CT_HINGE:
			double x, y;
			double x1, y1, x2, y2;
			double oldValue;
			double scaleFactor;
			scaleFactor = 0.1;
			pointConstraint->getPosition(x, y);
			pointConstraint->getHingeStart(x1, y1);
			pointConstraint->getHingeEnd(x2, y2);
			oldValue = m_femGrid->getStiffness(x, y);
			m_femGrid->setStiffnessLine(x1, y1, x2, y2, 0.0, (double)m_femGrid->getStride()*0.2);
			m_femGrid->setStiffness(x, y, oldValue*scaleFactor, 0, false);
			break;
		default:
			break;
		}

		pointConstraint = m_femGrid->getNextPointConstraint();
	}

	//
	// Calculate optimum bandwidth
	//

	m_femGrid->getGridSize(rows, cols);

	logMessage("CFemGridSolver2","Enumerating dofs and calculating bandwidth.");

	m_femGrid->resetDofs();
	m_nDof =  m_femGrid->enumerateDofs(ED_LEFT_RIGHT);
	bwLeftRight = m_femGrid->getBandwidth()-1;

	m_femGrid->resetDofs();
	m_nDof =  m_femGrid->enumerateDofs(ED_BOTTOM_TOP);
	bwBottomTop = m_femGrid->getBandwidth()-1;

	if (bwLeftRight<bwBottomTop)
	{
		m_femGrid->resetDofs();
		m_nDof = m_femGrid->enumerateDofs(ED_LEFT_RIGHT);
		maxBandwidth = bwLeftRight;
	}
	else
		maxBandwidth = bwBottomTop;

    //so_print("CFemGridSolver2",so_format("Total dofs = %d",m_nDof));
    //so_print("CFemGridSolver2",so_format("Bandwidth  = %d",maxBandwidth));
	
	if (m_nDof==0)
	{
		m_errorStatus = ET_NO_ELEMENTS;
		return;
	}

	//
	// Setup system matrices
	//

	SymmetricBandMatrix K(m_nDof,maxBandwidth);
	K = 0.0;

	this->progressMessage("Assembling system matrix.", 20);

	ColumnVector fsys;
	ColumnVector gdof;
	ColumnVector ldof;
	ColumnVector GlobalA;

	double E = m_elasticModulus;
	double v = m_youngsModulus;
	double t = m_thickness;
	int ptype = m_ptype;    // Plane stress
	int ir = 2; // Integration rule
	int elementsWithWeight = 0;
	int nElements;

	RowVector Ex(4);
	RowVector Ey(4);
	m_Eq.ReSize(2);
	m_Eq = 0.0;
	if (m_useWeight)
		m_Eq(2)=-1.0;
	else
		m_Eq(2)=0.0;

	RowVector Ep(3);
	Ep << ptype << t << ir;
	Matrix Ke(8,8);
	ColumnVector fe(8);
	m_f.ReSize(m_nDof,1);
	m_f = 0.0;
	RowVector DofTopo(8);
	Matrix D;

	// 
	// Setup matrices for MATLAB output
	//

	Matrix Ex_out;
	Matrix Ey_out;
	Matrix Topo_out;

	if (m_outputMatlab)
	{
		nElements = m_femGrid->getElementCount();
		Ex_out.ReSize(nElements, 4);
		Ey_out.ReSize(nElements, 4);
		Topo_out.ReSize(nElements, 8);
	}

	//
	// Setup variables for assemblation
	//

	nElements = 0;
	m_minX = 1e300;
	m_maxX = -1e300;
	m_minY = 1e300;
	m_maxY = -1e300;

	///////////////////////////////////////////////////////////////////////////
	//
	// Element assemblation
	//
	///////////////////////////////////////////////////////////////////////////

    clock_t t0 = clock();
    
	for (i=0; i<rows; i++)
	{
		for (j=0; j<cols; j++)
		{
			if (m_femGrid->getElement(i, j, elementValue, ex, ey, topo))
			{
				//
				// Get element coordinates
				//

				Ex(1) = ex[0]; Ey(1) = ey[0];
				Ex(2) = ex[1]; Ey(2) = ey[1];
				Ex(3) = ex[2]; Ey(3) = ey[2];
				Ex(4) = ex[3]; Ey(4) = ey[3];

				//
				// Determine the size of the model
				//

				if (Ex.Maximum()>m_maxX)
					m_maxX = Ex.Maximum();
				if (Ex.Minimum()<m_minX)
					m_minX = Ex.Minimum();

				if (Ey.Maximum()>m_maxY)
					m_maxY = Ey.Maximum();
				if (Ey.Minimum()<m_minY)
					m_minY = Ey.Minimum();
				//
				// Store element coordinates
				//

				if (m_outputMatlab)
				{
					Ex_out(nElements+1, 1) = Ex(1);
					Ex_out(nElements+1, 2) = Ex(2);
					Ex_out(nElements+1, 3) = Ex(3);
					Ex_out(nElements+1, 4) = Ex(4);

					Ey_out(nElements+1, 1) = Ey(1);
					Ey_out(nElements+1, 2) = Ey(2);
					Ey_out(nElements+1, 3) = Ey(3);
					Ey_out(nElements+1, 4) = Ey(4);
				}

				// 
				// Get element properties
				//
				
				calfem::hooke(ptype, E*(double)elementValue*m_stiffnessScalefactor, v, D);

				// 
				// Get element topology
				//
				
				for (l=0; l<8; l++)
				{
					DofTopo(l+1) = topo[l];

					if (m_outputMatlab)
						Topo_out(nElements+1,l+1) = topo[l];
				}

				// 
				// Create element matrix
				//
				
				calfem::plani4e(Ex, Ey, Ep, D, m_Eq, Ke, fe);

				//
				// Assemble element matrix
				//
				
				calfem::assem(DofTopo, K, Ke, m_f, fe);
				nElements++;

				//
				// Add to total weight
				//

				elementsWithWeight++;
			}
		}
	}
    clock_t t1 = clock();
    clock_t dt = t1-t0;
    
    std::cout << "Element assemblage = " << dt << " tics." << std::endl;

	//
	// If no elements where assembled something was wrong return 
	// with error.
	//

	if (nElements==0)
	{
		m_errorStatus = ET_NO_ELEMENTS;
		return;
	}

	//
	// Export Ex, Ey, and Topo matrices to MATLAB m-file
	//

	if (m_outputMatlab)
	{
		using namespace std;
		
		fstream cf;

		cf.open("calfem.m", ios::out);

		calfem::writeMatrix("Ex", Ex_out, cf);
		calfem::writeMatrix("Ey", Ey_out, cf);
		calfem::writeMatrix("Topo", Topo_out, cf);

		cf.close();
	}

    //so_print("CFemGridSolver2",so_format("%d elements assembled.",nElements));
	//cout << "Model size" << endl;
	//cout << "\t x,max = " << m_maxX << endl;
	//cout << "\t x,min = " << m_minX << endl;
	//cout << "\t y,max = " << m_maxY << endl;
	//cout << "\t y,min = " << m_minY << endl;

	///////////////////////////////////////////////////////////////////////////
	//
	// Setup forces and constraints
	//
	///////////////////////////////////////////////////////////////////////////

	//
	// Scale load vector for body weight
	//

	if (m_useWeight)
	{
		so_print("CFemGridSolver2","Applying weight.");
		
		double Fsum = 0.0;
		
		for (i=2; i<=m_nDof; i+=2)
			Fsum+=m_f(i);
		
		for (i=2; i<=m_nDof; i+=2)
			m_f(i) = -m_weight*m_f(i)/Fsum;
	}

	//
	// Setup load vector
	//

	so_print("CFemGridSolver2","Defining load vector.");
	progressMessage("Defining load vector.", 30);

	bool loadsDefined = false;
	double x, y;
	double vx, vy;
	double value;
	int dofs[2];

	m_femGrid->clearPoints();

	if ((m_femGrid->getPointLoadSize()==0)&&(m_Eq(1)==0.0)&&(m_Eq(2)==0.0))
	{
		m_errorStatus = ET_NO_LOADS;
		return;
	}

	CForce* pointLoad = m_femGrid->getFirstPointLoad();

	while (pointLoad!=NULL) 
	{
		pointLoad->getPosition(x, y);
		value = pointLoad->getValue();
		pointLoad->getDirection(vx, vy);
		m_femGrid->getNearestDofs((int)x, (int)y, dofs);

		if (dofs[0]>0)
		{
            //so_print("CFemGridSolver2",so_format("\tLoad found at [ %g, %g ], dofs = [ %d, %d ]",x, y, dofs[0], dofs[1]));

			m_f(dofs[0]) += vx*value*m_forceMagnitude;
			m_f(dofs[1]) += vy*value*m_forceMagnitude;

			loadsDefined = true;
		}
		
		pointLoad = m_femGrid->getNextPointLoad();
	}

	if ((m_Eq(1)!=0.0)||(m_Eq(2)!=0.0))
		loadsDefined = true;

	//
	// If no loads have been defined return with an error.
	//

	if (!loadsDefined)
	{
		m_errorStatus = ET_LOAD_OUTSIDE_AE;
		return;
	}

	//
	// Define constraints
	//

	so_print("CFemGridSolver2","Defining constraints.");
	progressMessage("Setting up boundary conditions.", 40);

	if (m_femGrid->getPointConstraintsSize()==0)
	{
		m_errorStatus = ET_NO_BCS;
		return;
	}

	pointConstraint = m_femGrid->getFirstPointConstraint();

	set<int> uniqueDofs;
	set<int> uniqueVectorDofs;
	set<int>::iterator si;

	vector<CConstraint*> vectorConstraints;
	vector<CConstraint*>::iterator vci;

	double* prescribedValues = new double[m_nDof+1];
	bool bcsDefined = false;
	bool vectorBcsDefined = false;
	
	while (pointConstraint!=NULL) 
	{
		pointConstraint->getPosition(x, y);
		m_femGrid->getNearestDofs((int)x, (int)y, dofs);

		if (dofs[0]>0)
		{
            //so_print("CFemGridSolver2",so_format("\tConstraint found at [ %g, %g ], dofs = [ %d, %d ]",x, y, dofs[0], dofs[1]));

			switch (pointConstraint->getConstraintType()) {
			case CConstraint::CT_XY:
				uniqueDofs.insert(dofs[0]);
				uniqueDofs.insert(dofs[1]);
				prescribedValues[dofs[0]] = 0.0;
				prescribedValues[dofs[1]] = 0.0;
				bcsDefined = true;
				break;
			case CConstraint::CT_X:
				uniqueDofs.insert(dofs[0]);
				prescribedValues[dofs[0]] = 0.0;
				bcsDefined = true;
				break;
			case CConstraint::CT_Y:
				uniqueDofs.insert(dofs[1]);
				prescribedValues[dofs[1]] = 0.0;
				bcsDefined = true;
				break;
			case CConstraint::CT_VECTOR:
				uniqueVectorDofs.insert(dofs[0]);
				uniqueVectorDofs.insert(dofs[1]);
				si = uniqueVectorDofs.find(dofs[0]);
				if (si != uniqueVectorDofs.end())
					vectorConstraints.push_back(pointConstraint);
				vectorBcsDefined = true;
				break;
			default:
				break;
			}
		}
			
		pointConstraint = m_femGrid->getNextPointConstraint();
	}

	if ((!bcsDefined)&&(!vectorBcsDefined))
	{
		m_errorStatus = ET_BC_OUTSIDE_AE;
		return;
	}

	//
	// Assemble Vector Constraints
	//

	if (vectorBcsDefined)
	{
		double x1, y1, x2, y2, ex, ey;
		RowVector BarEx(2);
		RowVector BarEy(2);
		RowVector BarEp(2);
		Matrix BarKe(4,4);
		RowVector BarDofTopo(4);

		BarEp << E << m_constraintStiffnessScale;

		for (vci=vectorConstraints.begin(); vci!=vectorConstraints.end(); vci++)
		{
			CConstraint* constraint = *vci;
			constraint->getPosition(x2, y2);
			m_femGrid->getNearestDofs((int)x2, (int)y2, dofs);

			constraint->getDirection(ex, ey);

			x1 = x2 - ex;
			y1 = y2 - ey;

			BarEx << x1 << x2;
			BarEy << y1 << y2;
			BarDofTopo << -1.0 << -1.0 << dofs[0] << dofs[1];

			calfem::bar2e(BarEx, BarEy, BarEp, BarKe);
			calfem::assem(BarDofTopo, K, BarKe);
		}
	}

	// 
	// Remove doubly defined dofs
	//

	so_print("CFemGridSolver2","\tUnique dofs:\n");

	Matrix Bc(uniqueDofs.size(),2);
	Bc = 0.0;
	int bcCount = 1;

	for (si=uniqueDofs.begin(); si!=uniqueDofs.end(); si++)
	{
		int dof = (*si);
        //so_print("CFemGridSolver2",so_format("\t%d",dof));
		
		Bc(bcCount,1) = dof;
		Bc(bcCount,2) = prescribedValues[dof];
		bcCount++;
	}

	delete [] prescribedValues;
	uniqueDofs.clear();

	// Variables used when a reduced system matrix is needed.

	RowVector Idx(m_nDof);
	Idx = 0.0;

	SymmetricBandMatrix Ksys;


	///////////////////////////////////////////////////////////////////////////
	//
	// Solve system
	//
	///////////////////////////////////////////////////////////////////////////

	m_a.ReSize(m_nDof,1);
	m_a = 0.0;

	so_print("CFemGridSolver2","Solving system.");
	progressMessage("Solving.", 60);

	Try 
	{
		m_X = K; // LU decomposition
		m_a = m_X.i() * m_f;
	}
	CatchAll 
	{
		m_errorStatus = ET_INVALID_MODEL;
		return;
	}

	so_print("CFemGridSolver2","Done.");

	//
	// Create global displacement vector
	//

	so_print("CFemGridSolver2","\tCreating global displacement vector.");

	progressMessage("Storing results.", 80);

	m_maxNodeValue = -1.0e300;

	m_femGrid->setDisplacementSize(m_nDof);

	for (i=1; i<=K.Nrows(); i++)
	{
		m_femGrid->setDisplacement(i, m_a(i));
		if (fabs(m_a(i))>m_maxNodeValue)
			m_maxNodeValue = fabs(m_a(i));
	}

	///////////////////////////////////////////////////////////////////////////
	//
	// Store element forces in elements
	//
	///////////////////////////////////////////////////////////////////////////
	
	so_print("CFemGridSolver2","Calculating results.");

	RowVector Ed(8);
	Matrix Es; 
	Matrix Et; 
	Ed = 0.0;
	m_maxStressValue = -1.0e300;
	m_maxMisesStressValue = -1.0e300;
	m_maxPosStressValue = -1.0e300;
	m_maxNegStressValue = -1.0e300;

	so_print("CFemGridSolver2:","\tCalculating element forces.");

	m_femGrid->zeroNodeResults();

	for (i=0; i<rows; i++)
	{
		for (j=0; j<cols; j++)
		{
			double sigx;
			double sigy;
			double tau;

			if (m_femGrid->getElement(i, j, elementValue, ex, ey, topo))
			{
				//
				// Get element coordinates
				//

				Ex(1) = ex[0]; Ey(1) = ey[0];
				Ex(2) = ex[1]; Ey(2) = ey[1];
				Ex(3) = ex[2]; Ey(3) = ey[2];
				Ex(4) = ex[3]; Ey(4) = ey[3];
				
				// 
				// Get element properties
				//
				
				calfem::hooke(ptype, E*(double)elementValue*m_stiffnessScalefactor, v, D);

				// 
				// Get element topology
				//
				
				for (l=0; l<8; l++)
					DofTopo(l+1) = topo[l];

				//
				// Get element displacements
				//
				
				for (l=1; l<=8; l++)
					if (m_reduceSystemMatrix)
						Ed(l) = GlobalA(DofTopo(l));
					else
						Ed(l) = m_a(DofTopo(l));


				// 
				// Calculate element forces 
				//
				
				calfem::plani4s(Ex, Ey, Ep, D, Ed, Es, Et);

				// 
				// Calculate mises stresses for the integration points and store in 
				// nodes.
				//

				//cout << "Ex = " << Ex(1) << ", " << Ex(2) << ", " << Ex(3) << ", " << Ex(4) << endl;
				//cout << "Ey = " << Ey(1) << ", " << Ey(2) << ", " << Ey(3) << ", " << Ey(4) << endl;

				int ip;

				double ipStress[4];

				for (ip=1; ip<=4; ip++)
				{
					sigx=Es(ip,1);
					sigy=Es(ip,2);
					tau=Es(ip,3);

					double ds = (sigx-sigy)/2.0;
					double R = sqrt(pow(ds,2)+pow(tau,2));						

					double sig1 = (sigx+sigy)/2.0+R; 
					double sig2 = (sigx+sigy)/2.0-R; 

					ipStress[ip-1] = sqrt( pow(sig1,2) - sig1*sig2 + pow(sig2,2) );
				}

				//     i,j+1    i+1,j+1
				//     o-----o
				//     |     |
				//     |     |
				//     o-----o
				//     i,j    i,j+1

				m_femGrid->addNodeResult(i,j,ipStress[0]);
				m_femGrid->addNodeResult(i,j+1,ipStress[1]);
				m_femGrid->addNodeResult(i+1,j+1,ipStress[2]);
				m_femGrid->addNodeResult(i,j+1,ipStress[3]);

				//
				// Average stresses from integration points. Is this correct?? CHECK!
				//

				RowVector Es_avg = Es.sum_columns() / 4.0;
				
				//
				// Calculate principal stresses
				//

				sigx=Es_avg(1);
				sigy=Es_avg(2);
				tau=Es_avg(3);

				double ds = (sigx-sigy)/2.0;
				double R = sqrt(pow(ds,2)+pow(tau,2));						

				double sig1 = (sigx+sigy)/2.0+R; 
				double sig2 = (sigx+sigy)/2.0-R; 
				double alfa = atan2(tau,ds)/2.0;

				double misesStress = sqrt(	pow(sig1,2) - sig1*sig2 + pow(sig2,2) );

				m_femGrid->setResult(i, j, 0, sig1);
				m_femGrid->setResult(i, j, 1, sig2);
				m_femGrid->setResult(i, j, 2, alfa);
				m_femGrid->setResult(i, j, 3, misesStress);

				if (misesStress>m_maxMisesStressValue)
					m_maxMisesStressValue = misesStress;
				
				if (fabs(sig1)>m_maxStressValue)
					m_maxStressValue = fabs(sig1);
				
				if (fabs(sig2)>m_maxStressValue)
					m_maxStressValue = fabs(sig2);
				
				if (sig1<0.0)
					if (fabs(sig1)>m_maxNegStressValue)
						m_maxNegStressValue = fabs(sig1);
					
				if (sig1>0.0)
					if (fabs(sig1)>m_maxPosStressValue)
						m_maxPosStressValue = sig1;
						
				if (sig2<0.0)
					if (fabs(sig2)>m_maxNegStressValue)
						m_maxNegStressValue = fabs(sig2);
							
				if (sig2>0.0)
					if (fabs(sig2)>m_maxPosStressValue)
						m_maxPosStressValue = sig2;
			}
		}
	}

	m_femGrid->averageNodeResults();

	cout << "Max misses stress = " << m_maxMisesStressValue << endl;

	///////////////////////////////////////////////////////////////////////////
	//
	// Calculate reaction forces from vector constraints
	//
	///////////////////////////////////////////////////////////////////////////

	if (vectorBcsDefined)
	{
		double x1, y1, x2, y2, ex, ey, es;
		RowVector BarEx(2);
		RowVector BarEy(2);
		RowVector BarEp(2);
		RowVector BarEd(4);

		double maxReactionForce = -1e300;

		BarEp << E << m_constraintStiffnessScale;

		for (vci=vectorConstraints.begin(); vci!=vectorConstraints.end(); vci++)
		{
			CConstraint* constraint = *vci;
			constraint->getPosition(x2, y2);
			m_femGrid->getNearestDofs((int)x2, (int)y2, dofs);

			constraint->getDirection(ex, ey);

			x1 = x2 - ex;
			y1 = y2 - ey;

			BarEx << x1 << x2;
			BarEy << y1 << y2;
			BarEd << 0.0 << 0.0 << m_a(dofs[0]) << m_a(dofs[1]) ;

			calfem::bar2s(BarEx, BarEy, BarEp, BarEd, es);

			if (fabs(es)>maxReactionForce)
				maxReactionForce = fabs(es);

			constraint->getReactionForce()->setValue(es);
		}

		for (vci=vectorConstraints.begin(); vci!=vectorConstraints.end(); vci++)
		{
			CConstraint* constraint = *vci;

			double force = 100.0 * constraint->getReactionForce()->getValue()/maxReactionForce;
			constraint->getReactionForce()->setLength(-force);
		}
	}

	//
	// Store max stress in FEM model
	//

	m_femGrid->setMaxStressValue(m_maxStressValue);
	m_femGrid->setMaxPosStressValue(m_maxPosStressValue);
	m_femGrid->setMaxNegStressValue(m_maxNegStressValue);
	m_femGrid->setMaxNodeValue(m_maxNodeValue);
	m_femGrid->setMaxMisesStressValue(m_maxMisesStressValue);

#ifdef WIN32
	DWORD endTime = timeGetTime();
	cout << "Total execution time (ms) = " << endTime - startTime << endl;
#endif

	progressMessage("Finished.", 99);
	progressMessage("Finished.", 0);

}

void CFemGridSolver2::setupHinges()
{
	//
	// Apply hinge constraints directly to stiffness grid
	//

	CConstraint* pointConstraint = m_femGrid->getFirstPointConstraint();

	while (pointConstraint!=NULL) 
	{
		switch (pointConstraint->getConstraintType()) {
		case CConstraint::CT_HINGE:
			double x, y;
			double x1, y1, x2, y2;
			double oldValue;
			double scaleFactor;
			scaleFactor = 0.1;
			pointConstraint->getPosition(x, y);
			pointConstraint->getHingeStart(x1, y1);
			pointConstraint->getHingeEnd(x2, y2);
			oldValue = m_femGrid->getStiffness(x, y);
			m_femGrid->setStiffnessLine(x1, y1, x2, y2, 0.0, (double)m_femGrid->getStride()*0.2);
			m_femGrid->setStiffness(x, y, oldValue*scaleFactor, 0, false);
			break;
		default:
			break;
		}

		pointConstraint = m_femGrid->getNextPointConstraint();
	}
}

int CFemGridSolver2::calculateOptimalBandwidth()
{
	//
	// Calculate optimum bandwidth
	//

	int rows, cols;
	int bwLeftRight, bwBottomTop;
	int maxBandwidth;

	m_femGrid->getGridSize(rows, cols);

	//logMessage("CFemGridSolver2","Enumerating dofs and calculating bandwidth.");

	m_femGrid->resetDofs();
	m_nDof =  m_femGrid->enumerateDofs(ED_LEFT_RIGHT);
	bwLeftRight = m_femGrid->getBandwidth()-1;

	m_femGrid->resetDofs();
	m_nDof =  m_femGrid->enumerateDofs(ED_BOTTOM_TOP);
	bwBottomTop = m_femGrid->getBandwidth()-1;

	if (bwLeftRight<bwBottomTop)
	{
		m_femGrid->resetDofs();
		m_nDof = m_femGrid->enumerateDofs(ED_LEFT_RIGHT);
		maxBandwidth = bwLeftRight;
	}
	else
		maxBandwidth = bwBottomTop;

	//so_print("CFemGridSolver2",so_format("Total dofs = %d",m_nDof));
	//so_print("CFemGridSolver2",so_format("Bandwidth  = %d",maxBandwidth));

	return maxBandwidth;
}

int CFemGridSolver2::assembleSystem(SymmetricBandMatrix& K)
{
	int i, j, l;
	int rows, cols;
	int topo[8];
	int ptype = m_ptype;    // Plane stress
	int ir = 2; // Integration rule
	int elementsWithWeight = 0;
	int nElements;

	double elementValue;
	double ex[4];
	double ey[4];
	double E = m_elasticModulus;
	double v = m_youngsModulus;
	double t = m_thickness;

	RowVector Ep(3);
	RowVector Ex(4);
	RowVector Ey(4);
	RowVector DofTopo(8);

	ColumnVector fe(8);

	Matrix Ke(8,8);
	Matrix D;

	m_Eq.ReSize(2);
	m_Eq = 0.0;
	if (m_useWeight)
		m_Eq(2)=-1.0;
	else
		m_Eq(2)=0.0;

	Ep << ptype << t << ir;
	m_f.ReSize(m_nDof,1);
	m_f = 0.0;

	m_femGrid->getGridSize(rows, cols);

	//
	// Setup variables for assemblation
	//

	nElements = 0;
	m_minX = 1e300;
	m_maxX = -1e300;
	m_minY = 1e300;
	m_maxY = -1e300;

	for (i=0; i<rows; i++)
	{
		for (j=0; j<cols; j++)
		{
			if (m_femGrid->getElement(i, j, elementValue, ex, ey, topo))
			{
				//
				// Get element coordinates
				//

				Ex(1) = ex[0]; Ey(1) = ey[0];
				Ex(2) = ex[1]; Ey(2) = ey[1];
				Ex(3) = ex[2]; Ey(3) = ey[2];
				Ex(4) = ex[3]; Ey(4) = ey[3];

				//
				// Determine the size of the model
				//

				if (Ex.Maximum()>m_maxX)
					m_maxX = Ex.Maximum();
				if (Ex.Minimum()<m_minX)
					m_minX = Ex.Minimum();

				if (Ey.Maximum()>m_maxY)
					m_maxY = Ey.Maximum();
				if (Ey.Minimum()<m_minY)
					m_minY = Ey.Minimum();

				// 
				// Get element properties
				//
				
				calfem::hooke(ptype, E*(double)elementValue*m_stiffnessScalefactor, v, D);

				// 
				// Get element topology
				//
				
				for (l=0; l<8; l++)
					DofTopo(l+1) = topo[l];

				// 
				// Create element matrix
				//
				
				calfem::plani4e(Ex, Ey, Ep, D, m_Eq, Ke, fe);

				//
				// Assemble element matrix
				//
				
				calfem::assem(DofTopo, K, Ke, m_f, fe);
				nElements++;

				//
				// Add to total weight
				//

				elementsWithWeight++;
			}
		}
	}

	return nElements;
}

int CFemGridSolver2::assembleSystemOpt(SymmetricBandMatrix& K, Matrix& X, Matrix& L, double penalty)
{
	int i, j, l;
	int rows, cols;
	int topo[8];
	int ptype = m_ptype;    // Plane stress
	int ir = 2; // Integration rule
	int elementsWithWeight = 0;
	int nElements;

	double elementValue;
	double ex[4];
	double ey[4];
	double E = m_elasticModulus;
	double v = m_youngsModulus;
	double t = m_thickness;

	RowVector Ep(3);
	RowVector Ex(4);
	RowVector Ey(4);
	RowVector DofTopo(8);

	ColumnVector fe(8);

	Matrix Ke(8,8);
	Matrix D;

	m_Eq.ReSize(2);
	m_Eq = 0.0;
	if (m_useWeight)
		m_Eq(2)=-1.0;
	else
		m_Eq(2)=0.0;

	Ep << ptype << t << ir;
	m_f.ReSize(m_nDof,1);
	m_f = 0.0;

	m_femGrid->getGridSize(rows, cols);

	//
	// Setup variables for assemblation
	//

	nElements = 0;
	m_minX = 1e300;
	m_maxX = -1e300;
	m_minY = 1e300;
	m_maxY = -1e300;

	for (i=0; i<rows; i++)
	{
		for (j=0; j<cols; j++)
		{
			if (m_femGrid->getElement(i, j, elementValue, ex, ey, topo))
			{
				//
				// Get element coordinates
				//

				Ex(1) = ex[0]; Ey(1) = ey[0];
				Ex(2) = ex[1]; Ey(2) = ey[1];
				Ex(3) = ex[2]; Ey(3) = ey[2];
				Ex(4) = ex[3]; Ey(4) = ey[3];

				//
				// Determine the size of the model
				//

				if (Ex.Maximum()>m_maxX)
					m_maxX = Ex.Maximum();
				if (Ex.Minimum()<m_minX)
					m_minX = Ex.Minimum();

				if (Ey.Maximum()>m_maxY)
					m_maxY = Ey.Maximum();
				if (Ey.Minimum()<m_minY)
					m_minY = Ey.Minimum();

				// 
				// Get element properties
				//


				calfem::hooke(ptype, E, v, D);
				//calfem::hooke(ptype, pow(E*X(i,j),penalty), v, D);

				// 
				// Get element topology
				//
				
				for (l=0; l<8; l++)
					DofTopo(l+1) = topo[l];

				// 
				// Create element matrix
				//
				
				Ke = 0.0;
				calfem::plani4e(Ex, Ey, Ep, D, m_Eq, Ke, fe);

				// Only apply density of L<1.0 

				if (L(i+1,j+1)<1.0)
					Ke = Ke * pow(X(i+1,j+1),penalty);
				
				//
				// Assemble element matrix
				//
				
				calfem::assem(DofTopo, K, Ke, m_f, fe);
				nElements++;

				//
				// Add to total weight
				//

				elementsWithWeight++;
			}
		}
	}

	return nElements;
}

void CFemGridSolver2::setupForcesAndConstraints(bool& loadsDefined, bool& bcsDefined, bool& vectorBcsDefined, std::set<int>& uniqueDofs, std::set<int>& uniqueVectorDofs, vector<CConstraint*>& vectorConstraints, RowVector& prescribedValues)
{
	//
	// Scale load vector for body weight
	//

	int i;
	int dofs[2];
	double x, y;
	double vx, vy;
	double value;

	loadsDefined = false;

	if (m_useWeight)
	{
		//so_print("CFemGridSolver2","Applying weight.");
		
		double Fsum = 0.0;
		
		for (i=2; i<=m_nDof; i+=2)
			Fsum+=m_f(i);
		
		for (i=2; i<=m_nDof; i+=2)
			m_f(i) = -m_weight*m_f(i)/Fsum;
	}

	//
	// Setup load vector
	//

	//so_print("CFemGridSolver2","Defining load vector.");
	progressMessage("Defining load vector.", 30);

	m_femGrid->clearPoints();

	CForce* pointLoad = m_femGrid->getFirstPointLoad();

	while (pointLoad!=NULL) 
	{
		pointLoad->getPosition(x, y);
		value = pointLoad->getValue();
		pointLoad->getDirection(vx, vy);
		m_femGrid->getNearestDofs((int)x, (int)y, dofs);

		if (dofs[0]>0)
		{
			//so_print("CFemGridSolver2",so_format("\tLoad found at [ %g, %g ], dofs = [ %d, %d ]",x, y, dofs[0], dofs[1]));

			m_f(dofs[0]) += vx*value*m_forceMagnitude;
			m_f(dofs[1]) += vy*value*m_forceMagnitude;

			loadsDefined = true;
		}
		
		pointLoad = m_femGrid->getNextPointLoad();
	}

	if ((m_Eq(1)!=0.0)||(m_Eq(2)!=0.0))
		loadsDefined = true;

	//
	// Define constraints
	//

	//so_print("CFemGridSolver2","Defining constraints.");
	progressMessage("Setting up boundary conditions.", 40);

	CConstraint* pointConstraint = m_femGrid->getFirstPointConstraint();

	set<int>::iterator si;

	vector<CConstraint*>::iterator vci;

	prescribedValues.ReSize(m_nDof);

	bcsDefined = false;
	vectorBcsDefined = false;
	
	while (pointConstraint!=NULL) 
	{
		pointConstraint->getPosition(x, y);
		m_femGrid->getNearestDofs((int)x, (int)y, dofs);

		if (dofs[0]>0)
		{
			//so_print("CFemGridSolver2",so_format("\tConstraint found at [ %g, %g ], dofs = [ %d, %d ]",x, y, dofs[0], dofs[1]));

			switch (pointConstraint->getConstraintType()) {
			case CConstraint::CT_XY:
				uniqueDofs.insert(dofs[0]);
				uniqueDofs.insert(dofs[1]);
				prescribedValues(dofs[0]) = 0.0;
				prescribedValues(dofs[1]) = 0.0;
				bcsDefined = true;
				break;
			case CConstraint::CT_X:
				uniqueDofs.insert(dofs[0]);
				prescribedValues(dofs[0]) = 0.0;
				bcsDefined = true;
				break;
			case CConstraint::CT_Y:
				uniqueDofs.insert(dofs[1]);
				prescribedValues(dofs[1]) = 0.0;
				bcsDefined = true;
				break;
			case CConstraint::CT_VECTOR:
				uniqueVectorDofs.insert(dofs[0]);
				uniqueVectorDofs.insert(dofs[1]);
				si = uniqueVectorDofs.find(dofs[0]);
				if (si != uniqueVectorDofs.end())
					vectorConstraints.push_back(pointConstraint);
				vectorBcsDefined = true;
				break;
			default:
				break;
			}
		}
			
		pointConstraint = m_femGrid->getNextPointConstraint();
	}
}

void CFemGridSolver2::assembleVectorConstraints(SymmetricBandMatrix& K, std::vector<CConstraint*>& vectorConstraints)
{
	int dofs[2];
	double x1, y1, x2, y2, ex, ey;
	double E = m_elasticModulus;
	RowVector BarEx(2);
	RowVector BarEy(2);
	RowVector BarEp(2);
	RowVector BarDofTopo(4);
	Matrix BarKe(4,4);

	BarEp << E << m_constraintStiffnessScale;

	vector<CConstraint*>::iterator vci;

	for (vci=vectorConstraints.begin(); vci!=vectorConstraints.end(); vci++)
	{
		CConstraint* constraint = *vci;
		constraint->getPosition(x2, y2);
		m_femGrid->getNearestDofs((int)x2, (int)y2, dofs);

		constraint->getDirection(ex, ey);

		x1 = x2 - ex;
		y1 = y2 - ey;

		BarEx << x1 << x2;
		BarEy << y1 << y2;
		BarDofTopo << -1.0 << -1.0 << dofs[0] << dofs[1];

		calfem::bar2e(BarEx, BarEy, BarEp, BarKe);
		calfem::assem(BarDofTopo, K, BarKe);
	}
}

void CFemGridSolver2::removeDoubleDofs(set<int>& uniqueDofs, RowVector& prescribedValues, Matrix& Bc)
{
	// 
	// Remove doubly defined dofs
	//

	//so_print("CFemGridSolver2","\tUnique dofs:\n");

	Bc = 0.0;
	int bcCount = 1;
	set<int>::iterator si;

	for (si=uniqueDofs.begin(); si!=uniqueDofs.end(); si++)
	{
		int dof = (*si);
		//so_print("CFemGridSolver2",so_format("\t%d",dof));
		
		Bc(bcCount,1) = dof;
		Bc(bcCount,2) = prescribedValues(dof);
		bcCount++;
	}

	uniqueDofs.clear();
}

void CFemGridSolver2::computeElementForces()
{
	//so_print("CFemGridSolver2","Calculating results.");

	int i, j, l;
	int rows, cols;
	double elementValue;
	double ex[4];
	double ey[4];
	int topo[8];
	int ptype = m_ptype;    // Plane stress
	int ir = 2; // Integration rule

	double E = m_elasticModulus;
	double v = m_youngsModulus;
	double t = m_thickness;

	RowVector Ep(3);
	RowVector Ex(4);
	RowVector Ey(4);
	RowVector DofTopo(8);

	Ep << ptype << t << ir;

	m_femGrid->getGridSize(rows, cols);

	RowVector Ed(8);
	Matrix Es; 
	Matrix Et; 
	Matrix D;

	Ed = 0.0;
	m_maxStressValue = -1.0e300;
	m_maxMisesStressValue = -1.0e300;
	m_maxPosStressValue = -1.0e300;
	m_maxNegStressValue = -1.0e300;

	//so_print("CFemGridSolver2:","\tCalculating element forces.");

	m_femGrid->zeroNodeResults();

	for (i=0; i<rows; i++)
	{
		for (j=0; j<cols; j++)
		{
			double sigx;
			double sigy;
			double tau;

			if (m_femGrid->getElement(i, j, elementValue, ex, ey, topo))
			{
				//
				// Get element coordinates
				//

				Ex(1) = ex[0]; Ey(1) = ey[0];
				Ex(2) = ex[1]; Ey(2) = ey[1];
				Ex(3) = ex[2]; Ey(3) = ey[2];
				Ex(4) = ex[3]; Ey(4) = ey[3];
				
				// 
				// Get element properties
				//
				
				calfem::hooke(ptype, E*(double)elementValue*m_stiffnessScalefactor, v, D);

				// 
				// Get element topology
				//
				
				for (l=0; l<8; l++)
					DofTopo(l+1) = topo[l];

				//
				// Get element displacements
				//
				
				for (l=1; l<=8; l++)
					Ed(l) = m_a(DofTopo(l));

				// 
				// Calculate element forces 
				//
				
				calfem::plani4s(Ex, Ey, Ep, D, Ed, Es, Et);

				// 
				// Calculate mises stresses for the integration points and store in 
				// nodes.
				//

				//cout << "Ex = " << Ex(1) << ", " << Ex(2) << ", " << Ex(3) << ", " << Ex(4) << endl;
				//cout << "Ey = " << Ey(1) << ", " << Ey(2) << ", " << Ey(3) << ", " << Ey(4) << endl;

				int ip;

				double ipStress[4];

				for (ip=1; ip<=4; ip++)
				{
					sigx=Es(ip,1);
					sigy=Es(ip,2);
					tau=Es(ip,3);

					double ds = (sigx-sigy)/2.0;
					double R = sqrt(pow(ds,2)+pow(tau,2));						

					double sig1 = (sigx+sigy)/2.0+R; 
					double sig2 = (sigx+sigy)/2.0-R; 

					ipStress[ip-1] = sqrt( pow(sig1,2) - sig1*sig2 + pow(sig2,2) );
				}

				//     i,j+1    i+1,j+1
				//     o-----o
				//     |     |
				//     |     |
				//     o-----o
				//     i,j    i,j+1

				m_femGrid->addNodeResult(i,j,ipStress[0]);
				m_femGrid->addNodeResult(i,j+1,ipStress[1]);
				m_femGrid->addNodeResult(i+1,j+1,ipStress[2]);
				m_femGrid->addNodeResult(i,j+1,ipStress[3]);

				//
				// Average stresses from integration points. Is this correct?? CHECK!
				//

				RowVector Es_avg = Es.sum_columns() / 4.0;
				
				//
				// Calculate principal stresses
				//

				sigx=Es_avg(1);
				sigy=Es_avg(2);
				tau=Es_avg(3);

				double ds = (sigx-sigy)/2.0;
				double R = sqrt(pow(ds,2)+pow(tau,2));						

				double sig1 = (sigx+sigy)/2.0+R; 
				double sig2 = (sigx+sigy)/2.0-R; 
				double alfa = atan2(tau,ds)/2.0;

				double misesStress = sqrt(	pow(sig1,2) - sig1*sig2 + pow(sig2,2) );

				m_femGrid->setResult(i, j, 0, sig1);
				m_femGrid->setResult(i, j, 1, sig2);
				m_femGrid->setResult(i, j, 2, alfa);
				m_femGrid->setResult(i, j, 3, misesStress);

				if (misesStress>m_maxMisesStressValue)
					m_maxMisesStressValue = misesStress;
				
				if (fabs(sig1)>m_maxStressValue)
					m_maxStressValue = fabs(sig1);
				
				if (fabs(sig2)>m_maxStressValue)
					m_maxStressValue = fabs(sig2);
				
				if (sig1<0.0)
					if (fabs(sig1)>m_maxNegStressValue)
						m_maxNegStressValue = fabs(sig1);
					
				if (sig1>0.0)
					if (fabs(sig1)>m_maxPosStressValue)
						m_maxPosStressValue = sig1;
						
				if (sig2<0.0)
					if (fabs(sig2)>m_maxNegStressValue)
						m_maxNegStressValue = fabs(sig2);
							
				if (sig2>0.0)
					if (fabs(sig2)>m_maxPosStressValue)
						m_maxPosStressValue = sig2;
			}
		}
	}

	m_femGrid->averageNodeResults();

	cout << "Max misses stress = " << m_maxMisesStressValue << endl;
}

void CFemGridSolver2::computeElementForcesOpt(Matrix& X, double penalty)
{
	//so_print("CFemGridSolver2","Calculating results.");

	int i, j, l;
	int rows, cols;
	double elementValue;
	double ex[4];
	double ey[4];
	int topo[8];
	int ptype = m_ptype;    // Plane stress
	int ir = 2; // Integration rule

	double E = m_elasticModulus;
	double v = m_youngsModulus;
	double t = m_thickness;

	RowVector Ep(3);
	RowVector Ex(4);
	RowVector Ey(4);
	RowVector DofTopo(8);

	Ep << ptype << t << ir;

	m_femGrid->getGridSize(rows, cols);

	RowVector Ed(8);
	Matrix Es; 
	Matrix Et; 
	Matrix D;

	Ed = 0.0;
	m_maxStressValue = -1.0e300;
	m_maxMisesStressValue = -1.0e300;
	m_maxPosStressValue = -1.0e300;
	m_maxNegStressValue = -1.0e300;

	//so_print("CFemGridSolver2:","\tCalculating element forces.");

	m_femGrid->zeroNodeResults();

	for (i=0; i<rows; i++)
	{
		for (j=0; j<cols; j++)
		{
			double sigx;
			double sigy;
			double tau;

			if (m_femGrid->getElement(i, j, elementValue, ex, ey, topo))
			{
				//
				// Get element coordinates
				//

				Ex(1) = ex[0]; Ey(1) = ey[0];
				Ex(2) = ex[1]; Ey(2) = ey[1];
				Ex(3) = ex[2]; Ey(3) = ey[2];
				Ex(4) = ex[3]; Ey(4) = ey[3];
				
				// 
				// Get element properties
				//
				
				calfem::hooke(ptype, E*pow(X(i+1,j+1),penalty), v, D);

				// 
				// Get element topology
				//
				
				for (l=0; l<8; l++)
					DofTopo(l+1) = topo[l];

				//
				// Get element displacements
				//
				
				for (l=1; l<=8; l++)
					Ed(l) = m_a(DofTopo(l));

				// 
				// Calculate element forces 
				//
				
				calfem::plani4s(Ex, Ey, Ep, D, Ed, Es, Et);

				// 
				// Calculate mises stresses for the integration points and store in 
				// nodes.
				//

				//cout << "Ex = " << Ex(1) << ", " << Ex(2) << ", " << Ex(3) << ", " << Ex(4) << endl;
				//cout << "Ey = " << Ey(1) << ", " << Ey(2) << ", " << Ey(3) << ", " << Ey(4) << endl;

				int ip;

				double ipStress[4];

				for (ip=1; ip<=4; ip++)
				{
					sigx=Es(ip,1);
					sigy=Es(ip,2);
					tau=Es(ip,3);

					double ds = (sigx-sigy)/2.0;
					double R = sqrt(pow(ds,2)+pow(tau,2));						

					double sig1 = (sigx+sigy)/2.0+R; 
					double sig2 = (sigx+sigy)/2.0-R; 

					ipStress[ip-1] = sqrt( pow(sig1,2) - sig1*sig2 + pow(sig2,2) );
				}

				//     i,j+1    i+1,j+1
				//     o-----o
				//     |     |
				//     |     |
				//     o-----o
				//     i,j    i,j+1

				m_femGrid->addNodeResult(i,j,ipStress[0]);
				m_femGrid->addNodeResult(i,j+1,ipStress[1]);
				m_femGrid->addNodeResult(i+1,j+1,ipStress[2]);
				m_femGrid->addNodeResult(i,j+1,ipStress[3]);

				//
				// Average stresses from integration points. Is this correct?? CHECK!
				//

				RowVector Es_avg = Es.sum_columns() / 4.0;
				
				//
				// Calculate principal stresses
				//

				sigx=Es_avg(1);
				sigy=Es_avg(2);
				tau=Es_avg(3);

				double ds = (sigx-sigy)/2.0;
				double R = sqrt(pow(ds,2)+pow(tau,2));						

				double sig1 = (sigx+sigy)/2.0+R; 
				double sig2 = (sigx+sigy)/2.0-R; 
				double alfa = atan2(tau,ds)/2.0;

				double misesStress = sqrt(	pow(sig1,2) - sig1*sig2 + pow(sig2,2) );

				m_femGrid->setResult(i, j, 0, sig1);
				m_femGrid->setResult(i, j, 1, sig2);
				m_femGrid->setResult(i, j, 2, alfa);
				m_femGrid->setResult(i, j, 3, misesStress);

				if (misesStress>m_maxMisesStressValue)
					m_maxMisesStressValue = misesStress;
				
				if (fabs(sig1)>m_maxStressValue)
					m_maxStressValue = fabs(sig1);
				
				if (fabs(sig2)>m_maxStressValue)
					m_maxStressValue = fabs(sig2);
				
				if (sig1<0.0)
					if (fabs(sig1)>m_maxNegStressValue)
						m_maxNegStressValue = fabs(sig1);
					
				if (sig1>0.0)
					if (fabs(sig1)>m_maxPosStressValue)
						m_maxPosStressValue = sig1;
						
				if (sig2<0.0)
					if (fabs(sig2)>m_maxNegStressValue)
						m_maxNegStressValue = fabs(sig2);
							
				if (sig2>0.0)
					if (fabs(sig2)>m_maxPosStressValue)
						m_maxPosStressValue = sig2;
			}
		}
	}

	m_femGrid->averageNodeResults();

	cout << "Max misses stress = " << m_maxMisesStressValue << endl;
}

void CFemGridSolver2::computeReactionForces(std::vector<CConstraint*>& vectorConstraints)
{
	double x1, y1, x2, y2, ex, ey, es;
	int dofs[2];
	RowVector BarEx(2);
	RowVector BarEy(2);
	RowVector BarEp(2);
	RowVector BarEd(4);

	double maxReactionForce = -1e300;
	double E = m_elasticModulus;
	vector<CConstraint*>::iterator vci;

	BarEp << E << m_constraintStiffnessScale;

	for (vci=vectorConstraints.begin(); vci!=vectorConstraints.end(); vci++)
	{
		CConstraint* constraint = *vci;
		constraint->getPosition(x2, y2);
		m_femGrid->getNearestDofs((int)x2, (int)y2, dofs);

		constraint->getDirection(ex, ey);

		x1 = x2 - ex;
		y1 = y2 - ey;

		BarEx << x1 << x2;
		BarEy << y1 << y2;
		BarEd << 0.0 << 0.0 << m_a(dofs[0]) << m_a(dofs[1]) ;

		calfem::bar2s(BarEx, BarEy, BarEp, BarEd, es);

		if (fabs(es)>maxReactionForce)
			maxReactionForce = fabs(es);

		constraint->getReactionForce()->setValue(es);
	}

	for (vci=vectorConstraints.begin(); vci!=vectorConstraints.end(); vci++)
	{
		CConstraint* constraint = *vci;

		double force = 100.0 * constraint->getReactionForce()->getValue()/maxReactionForce;
		constraint->getReactionForce()->setLength(-force);
	}
}

void CFemGridSolver2::objectiveFunctionAndSensitivity(Matrix& X, Matrix& dC, Matrix& L, double penalty, double& c)
{
	//so_print("CFemGridSolver2","Calculating results.");

	int i, j, l;
	int rows, cols;
	double elementValue;
	double ex[4];
	double ey[4];
	int topo[8];
	int ptype = m_ptype;    // Plane stress
	int ir = 2; // Integration rule

	double E = m_elasticModulus;
	double v = m_youngsModulus;
	double t = m_thickness;

	RowVector Ep(3);
	RowVector Ex(4);
	RowVector Ey(4);
	RowVector DofTopo(8);

	Ep << ptype << t << ir;

	m_femGrid->getGridSize(rows, cols);

	RowVector Ed(8);
	Matrix Es; 
	Matrix Et; 
	Matrix D;

	ColumnVector fe(8);
	Matrix Ke(8,8);

	Ed = 0.0;
	m_maxStressValue = -1.0e300;
	m_maxMisesStressValue = -1.0e300;
	m_maxPosStressValue = -1.0e300;
	m_maxNegStressValue = -1.0e300;

	//so_print("CFemGridSolver2:","\tCalculating element forces.");

	c = 0.0;
	dC = 0.0;

	for (i=0; i<rows; i++)
	{
		for (j=0; j<cols; j++)
		{
			if (m_femGrid->getElement(i, j, elementValue, ex, ey, topo))
			{
				//
				// Get element coordinates
				//

				Ex(1) = ex[0]; Ey(1) = ey[0];
				Ex(2) = ex[1]; Ey(2) = ey[1];
				Ex(3) = ex[2]; Ey(3) = ey[2];
				Ex(4) = ex[3]; Ey(4) = ey[3];
				
				// 
				// Get element properties
				//
				
				calfem::hooke(ptype, E, v, D);

				// 
				// Get element topology
				//
				
				for (l=0; l<8; l++)
					DofTopo(l+1) = topo[l];

				//
				// Get element displacements
				//
				
				for (l=1; l<=8; l++)
					Ed(l) = m_a(DofTopo(l));

				// 
				// Calculate strain energy 
				//

				Ke = 0.0;
				calfem::plani4e(Ex, Ey, Ep, D, m_Eq, Ke, fe);
				Ke = Ke * pow(X(i+1,j+1),penalty);

				// [1x8][8x8][8x1] = [1x1]

				if (L(i+1,j+1)<0.2)
				{
					double W = (Ed * Ke * Ed.t()).as_scalar();
					c += pow(X(i+1,j+1),penalty)*W;
					dC(i+1,j+1) = -penalty*pow(X(i+1,j+1),penalty-1)*W;
				}
				else
				{
					// Is this correct ?
					//double W = (Ed * Ke * Ed.t()).as_scalar();
					//c += pow(X(i+1,j+1),penalty)*W;
					dC(i+1,j+1) = -1e50;
				}
				//cout << "W = " << W << ", pow(X(i+1,j+1),penalty) = " << pow(X(i+1,j+1),penalty) << endl;
			}
		}
	}
}

ReturnMatrix CFemGridSolver2::optimalityCriteriaUpdate(Matrix& X, Matrix& dC, Matrix& L, double volfrac, int nElements)
{
	using namespace calfem;

	//%%%%%%%%%% OPTIMALITY CRITERIA UPDATE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	//function [xnew]=OC(nelx,nely,x,volfrac,dc)  
	//l1 = 0; l2 = 100000; move = 0.2;
	//while (l2-l1 > 1e-4)
	//  lmid = 0.5*(l2+l1);
	//  xnew = max( 0.001, max( x-move, min( 1., min( x+move, x.*sqrt(-dc./lmid));
	//  if sum(sum(xnew)) - volfrac*nelx*nely > 0;                                
	//    l1 = lmid;
	//  else
	//    l2 = lmid;
	//  end
	//end

	double lmid;
	double l1 = 0.0;
	double l2 = 100000;
	double move = 0.2;

	Matrix Xnew;

	//int rigidElements = (int)L.Sum();
	//cout << "rigidElements = " << rigidElements << endl;

	while (l2-l1 > 1e-4)
	{
		lmid = 0.5*(l2+l1);
		Matrix X1 = X - move;
		Matrix X2 = X + move;
		Matrix X3 = -dC/lmid;
		Matrix X4 = elementMultiply(X,matrixSqrt(X3));
		Xnew = matrixMax2(0.001, matrixMax1(X1, matrixMin2( 1.0, matrixMin1(X2, X4))));
		m_femGrid->assignNonElements(Xnew, 0.0);
		//if (Xnew.Sum() - volfrac * (nElements-rigidElements) > 0.0)
		if (Xnew.Sum() - volfrac * (nElements) > 0.0)
			l1 = lmid;
		else
			l2 = lmid;
	}
	//assignWhereGreaterThan(Xnew, L, X, 0.0);
	Xnew.release(); return Xnew;
}

int CFemGridSolver2::scaleToAbsoluteSize(int r)
{
	auto grid_stride = m_femGrid->getStride();
	r = r * 6 / grid_stride; // 6 is default grid size, this should scale the filter radius to be constant wrt. canvas size
	return r;
}

ReturnMatrix CFemGridSolver2::sensitivityFilter1(Matrix& X, Matrix& dC, double rmin)
{
	//function [dcn]=Ole_Sigmund(nelx,nely,rmin,x,dc)                                   % ved rmin<1 deaktiveres filtreringen
	//dcn=zeros(nely,nelx);
	//for i = 1:nelx
	//  for j = 1:nely
	//    sum=0.0; 
	//    for k = max(i-floor(rmin),1):min(i+floor(rmin),nelx)
	//      for l = max(j-floor(rmin),1):min(j+floor(rmin),nely)
	//        fac = rmin-sqrt((i-k)^2+(j-l)^2);
	//        sum = sum+max(0,fac);
	//        dcn(j,i) = dcn(j,i) + max(0,fac)*x(l,k)*dc(l,k);
	//      end
	//    end
	//    dcn(j,i) = dcn(j,i)/(x(j,i)*sum);
	//  end
	//end	
	int i, j, k, l;
	int rows = X.nrows();
	int cols = X.ncols();
	double sum, fac;
	Matrix dCnew;

	dCnew = Matrix(rows, cols);
	dCnew = 0.0;

	if (m_rminInPixels)
		rmin = rmin / (double)m_femGrid->getStride();
  	//	rmin = scaleToAbsoluteSize(rmin);

	for (i=1; i<=cols; i++)
	{
		for (j=1; j<=rows; j++)
		{
			sum = 0.0;

			for (k=max(i-floor(rmin),1.0); k<=min(i+floor(rmin),(double)cols); k++)
				for (l=max(j-floor(rmin),1.0); l<=min(j+floor(rmin),(double)rows); l++)
				{
					fac = rmin - sqrt(pow((double)(i-k),2.0)+pow((double)(j-l),2.0));
					sum += max(0.0,fac);
					dCnew(j,i) += max(0.0,fac)*X(l,k)*dC(l,k);
				}
			dCnew(j,i) = dCnew(j,i)/(X(j,i)*sum);
		}
	}
	dCnew.release(); return dCnew;
}

ReturnMatrix CFemGridSolver2::sensitivityFilter2(Matrix& dC, double rmin)
{
	//function [dcn]=Back_Pedersen(nelx,nely,rmin,dc)         
	//A(1:nely, 1:nelx)=1;  % Volumen for each element
	//dcn=zeros(nely,nelx);
	//for i = 1:nelx
	//  for j = 1:nely
	//    sum=0.0; 
	//    for k = max(i-floor(rmin),1):min(i+floor(rmin),nelx);
	//      for l = max(j-floor(rmin),1):min(j+floor(rmin),nely);
	//        fac = rmin-sqrt((i-k)^2+(j-l)^2);
	//        sum = sum+max(0,fac)*A(l,k);
	//        dcn(j,i) = dcn(j,i) + max(0,fac)*dc(l,k);
	//      end
	//    end
	//    dcn(j,i) = dcn(j,i)/sum;% FE-ANALYSIS
	//
	//  end
	//end
	int i, j, k, l;
	int rows = dC.nrows();
	int cols = dC.ncols();
	double sum, fac;
	Matrix dCnew;

	dCnew = Matrix(rows, cols);
	dCnew = 0.0;

	if (m_rminInPixels)
		rmin = rmin / (double)m_femGrid->getStride();

	for (i=1; i<=cols; i++)
	{
		for (j=1; j<=rows; j++)
		{
			sum = 0.0;

			for (k=max(i-floor(rmin),1.0); k<=min(i+floor(rmin),(double)cols); k++)
				for (l=max(j-floor(rmin),1.0); l<=min(j+floor(rmin),(double)rows); l++)
				{
					fac = rmin - sqrt(pow((double)(i-k),2.0)+pow((double)(j-l),2.0));
					sum += max(0.0,fac);
					dCnew(j,i) += max(0.0,fac)*dC(l,k);
				}
			dCnew(j,i) = dCnew(j,i)/sum;
		}
	}
	dCnew.release(); return dCnew;
}

void CFemGridSolver2::executeOptimizer()
{
	int i;
	int maxBandwidth;
	int rows, cols;
	bool loadsDefined, bcsDefined, vectorBcsDefined;
	set<int> uniqueDofs;
	set<int> uniqueVectorDofs;
	vector<CConstraint*> vectorConstraints;
	RowVector prescribedValues;

#ifdef WIN32
	DWORD startTime = timeGetTime();
#endif


	ColumnVector fsys;
	ColumnVector gdof;
	ColumnVector ldof;
	ColumnVector GlobalA;

	int nElements;

	//
	// Reset error status
	//

	m_errorStatus = CFemGridSolver2::ET_NO_ERROR;

	///////////////////////////////////////////////////////////////////////////
	// Apply hinge constraints directly to stiffness grid
	//

	setupHinges();

	///////////////////////////////////////////////////////////////////////////
	// Calculate optimum bandwidth
	//

	maxBandwidth = this->calculateOptimalBandwidth();
	
	if (m_nDof==0)
	{
		m_errorStatus = ET_NO_ELEMENTS;
		return;
	}

	SymmetricBandMatrix K(m_nDof,maxBandwidth);

	///////////////////////////////////////////////////////////////////////////
	// Init optimisation
	//

	Matrix X;
	Matrix Xold;
	Matrix dC;
	Matrix L;

	double change = 1.0;
	double penalty = m_optPenalty;
	double volfrac = m_optVolfrac;
	double c = 0.0;
	int loop = 0;

	m_femGrid->getGridSize(rows, cols);
	X.ReSize(rows, cols);
	Xold.ReSize(rows, cols);
	dC.ReSize(rows, cols);
	L.ReSize(rows, cols);

	L = 1.0;

	cout << "Sum L = " << L.Sum() << endl;

	m_femGrid->copyGrid(X, volfrac); // Copy grid values to X multiplied with volfrac
	m_femGrid->assignFieldFromImage(1, 1);
	m_femGrid->copyField(1, L);

	calfem::assignWhereGreaterThan(L, L, 1.0, 0.0);
	//calfem::assignWhereGreaterThan(X, L, 1.0, 0.0);

	///////////////////////////////////////////////////////////////////////////
	// Start optimisation loop
	//

	while ((change>m_optMinChange)&&(loop<m_optMaxLoops)&&(this->continueCalc()))
	{
		loop++;
		Xold = X;


		///////////////////////////////////////////////////////////////////////////
		// Setup system matrices
		//

		this->progressMessage("Assembling system matrix.", 20);

		K = 0.0;

		///////////////////////////////////////////////////////////////////////////
		// Assemble system
		//

        clock_t t0 = clock();
		nElements = this->assembleSystemOpt(K, X, L, penalty);
        clock_t t1 = clock();
        std::cout << "Assemble system = " << t1-t0 << std::endl;

		//
		// If no elements where assembled something was wrong return 
		// with error.
		//

		if (nElements==0)
		{
			m_errorStatus = ET_NO_ELEMENTS;
			return;
		}

		///////////////////////////////////////////////////////////////////////////
		// Setup forces and constraints
		//

		if ((m_femGrid->getPointLoadSize()==0)&&(m_Eq(1)==0.0)&&(m_Eq(2)==0.0))
		{
			m_errorStatus = ET_NO_LOADS;
			return;
		}

		if (m_femGrid->getPointConstraintsSize()==0)
		{
			m_errorStatus = ET_NO_BCS;
			return;
		}

		this->progressMessage("Setting up forces and constraints.", 30);
		setupForcesAndConstraints(loadsDefined, bcsDefined, vectorBcsDefined, uniqueDofs, uniqueVectorDofs, vectorConstraints, prescribedValues);

		if ((!bcsDefined)&&(!vectorBcsDefined))
		{
			m_errorStatus = ET_BC_OUTSIDE_AE;
			return;
		}

		if (!loadsDefined)
		{
			m_errorStatus = ET_LOAD_OUTSIDE_AE;
			return;
		}

		///////////////////////////////////////////////////////////////////////////
		// Assemble Vector Constraints
		//

		if (vectorBcsDefined)
			this->assembleVectorConstraints(K, vectorConstraints);

		Matrix Bc(uniqueDofs.size(),2);
		this->removeDoubleDofs(uniqueDofs, prescribedValues, Bc);

		///////////////////////////////////////////////////////////////////////////
		// Solve system
		//

		m_a.ReSize(m_nDof,1);
		m_a = 0.0;

		progressMessage("Solving system.", 50);

        t0 = clock();
		Try 
		{
			m_X = K; // LU decomposition
			m_a = m_X.i() * m_f;
		}
		CatchAll 
		{
			m_errorStatus = ET_INVALID_MODEL;
			return;
		}
        t1 = clock();
        cout << "Equation system = " << t1-t0 << endl;

		// Objective functions

        t0 = clock();

		c = 0.0;
		dC = 0.0;

		cout << "objectiveFunctionAndSensitivity()" << endl;
		this->progressMessage("Objective function and sensitivity.", 60);
		this->objectiveFunctionAndSensitivity(X, dC, L, m_optPenalty, c);
        t1 = clock();
        cout << "Objective function = " << t1-t0 << endl;

		// Filter sensitivities

		if (!this->continueCalc())
		{
			break;
		}

		cout << "sensitivityFilter1()" << endl;
		this->progressMessage("Applying sensitivity filter.", 70);

		Matrix dCnew;

		switch (m_filterType) {
			case FT_NO_FILTER:
				break;
			case FT_OLE_SIGMUND:
				dCnew = this->sensitivityFilter1(X, dC, m_optRmin);
				dC = dCnew;
				break;
			case FT_SHARP_CONTOURING:
				dCnew = this->sensitivityFilter2(dC, m_optRmin);
				dC = dCnew;
				break;
		}

		// Design update by the optimality criteria method

		cout << "optimalityCriteriaUpdate()" << endl;
		this->progressMessage("Design update.", 100);
		Matrix Xnew = this->optimalityCriteriaUpdate(X, dC, L, volfrac, nElements);
		Matrix Diff = Xnew-Xold;
		change = Diff.maximum_absolute_value();
		X = Xnew;

		// Assign current X to field 0 for visualisation purposes

		m_femGrid->assignField(0, X);

		cout << "loop = " << loop << ", change = " << change << ", c = " << c << endl;

	}

	// Copy optimised shape to original grid

	m_femGrid->assignGrid(X);

	this->execute();

	return;

	///////////////////////////////////////////////////////////////////////////
	// Create global displacement vector
	//

	progressMessage("Storing results.", 80);

	m_maxNodeValue = -1.0e300;

	m_femGrid->setDisplacementSize(m_nDof);

	for (i=1; i<=K.Nrows(); i++)
	{
		m_femGrid->setDisplacement(i, m_a(i));
		if (fabs(m_a(i))>m_maxNodeValue)
			m_maxNodeValue = fabs(m_a(i));
	}

	///////////////////////////////////////////////////////////////////////////
	// Store element forces in elements
	//

	this->computeElementForcesOpt(X, m_optPenalty);
	
	///////////////////////////////////////////////////////////////////////////
	// Calculate reaction forces from vector constraints
	//

	if (vectorBcsDefined)
		this->computeReactionForces(vectorConstraints);

	///////////////////////////////////////////////////////////////////////////
	// Store max stress in FEM model
	//

	m_femGrid->setMaxStressValue(m_maxStressValue);
	m_femGrid->setMaxPosStressValue(m_maxPosStressValue);
	m_femGrid->setMaxNegStressValue(m_maxNegStressValue);
	m_femGrid->setMaxNodeValue(m_maxNodeValue);
	m_femGrid->setMaxMisesStressValue(m_maxMisesStressValue);

#ifdef WIN32
	DWORD endTime = timeGetTime();
	cout << "Total execution time (ms) = " << endTime - startTime << endl;
#endif

	progressMessage("Finished.", 99);
	progressMessage("Finished.", 0);

}


void CFemGridSolver2::executeUpdate()
{
	int i, j, l;
	int rows, cols;
	double ex[4];
	double ey[4];
	int topo[8];
	double elementValue;

	double E = m_elasticModulus;
	double v = m_youngsModulus;
	double t = m_thickness;
	int ptype = m_ptype;    // Plane stress
	int ir = 2;

	RowVector Ex(4);
	RowVector Ey(4);
	RowVector DofTopo(8);
	RowVector Ep(3);

	Matrix D;

	Ep << ptype << t << ir;

	m_femGrid->getGridSize(rows, cols);

	//
	// Define constraints
	//

	if (m_femGrid->getPointConstraintsSize()==0)
	{
		m_errorStatus = ET_NO_BCS;
		return;
	}

	CConstraint* pointConstraint = m_femGrid->getFirstPointConstraint();

	set<int> uniqueDofs;
	set<int> uniqueVectorDofs;
	set<int>::iterator si;

	vector<CConstraint*> vectorConstraints;
	vector<CConstraint*>::iterator vci;

	double* prescribedValues = new double[m_nDof+1];
	bool vectorBcsDefined = false;
	double x, y, value;
	int dofs[2];
	
	while (pointConstraint!=NULL) 
	{
		pointConstraint->getPosition(x, y);
		m_femGrid->getNearestDofs((int)x, (int)y, dofs);

		if (dofs[0]>0)
		{
			switch (pointConstraint->getConstraintType()) {
			case CConstraint::CT_XY:
				uniqueDofs.insert(dofs[0]);
				uniqueDofs.insert(dofs[1]);
				prescribedValues[dofs[0]] = 0.0;
				prescribedValues[dofs[1]] = 0.0;
				break;
			case CConstraint::CT_X:
				uniqueDofs.insert(dofs[0]);
				prescribedValues[dofs[0]] = 0.0;
				break;
			case CConstraint::CT_Y:
				uniqueDofs.insert(dofs[1]);
				prescribedValues[dofs[1]] = 0.0;
				break;
			case CConstraint::CT_VECTOR:
				uniqueVectorDofs.insert(dofs[0]);
				uniqueVectorDofs.insert(dofs[1]);
				si = uniqueVectorDofs.find(dofs[0]);
				if (si != uniqueVectorDofs.end())
					vectorConstraints.push_back(pointConstraint);
				vectorBcsDefined = true;
				break;
			default:
				break;
			}
		}
			
		pointConstraint = m_femGrid->getNextPointConstraint();
	}

	//
	// Scale load vector for body weight
	//

	m_f = 0.0;

	if (m_useWeight)
	{
		double Fsum = 0.0;
		
		for (i=2; i<=m_nDof; i+=2)
			Fsum+=m_f(i);
		
		for (i=2; i<=m_nDof; i+=2)
			m_f(i) = -m_weight*m_f(i)/Fsum;
	}

	//
	// Setup load vector
	//

	bool loadsDefined = false;
	double vx, vy;

	m_femGrid->clearPoints();

	if ((m_femGrid->getPointLoadSize()==0)&&(m_Eq(1)==0.0)&&(m_Eq(2)==0.0))
	{
		m_errorStatus = ET_NO_LOADS;
		return;
	}

	CForce* pointLoad = m_femGrid->getFirstPointLoad();

	while (pointLoad!=NULL) 
	{
		pointLoad->getPosition(x, y);
		value = pointLoad->getValue();
		pointLoad->getDirection(vx, vy);
		m_femGrid->getNearestDofs((int)x, (int)y, dofs);

		if (dofs[0]>0)
		{
			m_f(dofs[0]) += vx*value*m_forceMagnitude;
			m_f(dofs[1]) += vy*value*m_forceMagnitude;

			loadsDefined = true;
		}
		
		pointLoad = m_femGrid->getNextPointLoad();
	}

	if ((m_Eq(1)!=0.0)||(m_Eq(2)!=0.0))
		loadsDefined = true;

	if (!loadsDefined)
	{
		m_errorStatus = ET_LOAD_OUTSIDE_AE;
		return;
	}

	//
	// Update solution vector 
	//

	m_a.ReSize(m_nDof,1);
	m_a = 0.0;

	Try 
	{
		//m_X = K; // LU decomposition
		m_a = m_X.i() * m_f;
	}
	CatchAll 
	{
		m_errorStatus = ET_INVALID_MODEL;
		return;
	}

	//
	// Create global displacement vector
	//

	m_maxNodeValue = -1.0e300;

	m_femGrid->setDisplacementSize(m_nDof);

	for (i=1; i<=m_nDof; i++)
	{
		m_femGrid->setDisplacement(i, m_a(i));
		if (fabs(m_a(i))>m_maxNodeValue)
			m_maxNodeValue = fabs(m_a(i));
	}

	//
	// Store element forces in elements
	//
	
	RowVector Ed(8);
	Matrix Es; 
	Matrix Et; 
	Ed = 0.0;
	m_maxStressValue = -1.0e300;
	m_maxMisesStressValue = -1.0e300;
	m_maxPosStressValue = -1.0e300;
	m_maxNegStressValue = -1.0e300;

	m_femGrid->zeroNodeResults();

	for (i=0; i<rows; i++)
	{
		for (j=0; j<cols; j++)
		{
			double sigx;
			double sigy;
			double tau;

			if (m_femGrid->getElement(i, j, elementValue, ex, ey, topo))
			{
				//
				// Get element coordinates
				//

				Ex(1) = ex[0]; Ey(1) = ey[0];
				Ex(2) = ex[1]; Ey(2) = ey[1];
				Ex(3) = ex[2]; Ey(3) = ey[2];
				Ex(4) = ex[3]; Ey(4) = ey[3];
				
				// 
				// Get element properties
				//
				
				calfem::hooke(ptype, E*(double)elementValue*m_stiffnessScalefactor, v, D);

				// 
				// Get element topology
				//
				
				for (l=0; l<8; l++)
					DofTopo(l+1) = topo[l];

				//
				// Get element displacements
				//
				
				for (l=1; l<=8; l++)
					Ed(l) = m_a(DofTopo(l));

				// 
				// Calculate element forces 
				//
				
				calfem::plani4s(Ex, Ey, Ep, D, Ed, Es, Et);

				int ip;

				double ipStress[4];

				for (ip=1; ip<=4; ip++)
				{
					sigx=Es(ip,1);
					sigy=Es(ip,2);
					tau=Es(ip,3);

					double ds = (sigx-sigy)/2.0;
					double R = sqrt(pow(ds,2)+pow(tau,2));						

					double sig1 = (sigx+sigy)/2.0+R; 
					double sig2 = (sigx+sigy)/2.0-R; 

					ipStress[ip-1] = sqrt( pow(sig1,2) - sig1*sig2 + pow(sig2,2) );
				}

				//     i,j+1    i+1,j+1
				//     o-----o
				//     |     |
				//     |     |
				//     o-----o
				//     i,j    i,j+1

				m_femGrid->addNodeResult(i,j,ipStress[0]);
				m_femGrid->addNodeResult(i,j+1,ipStress[1]);
				m_femGrid->addNodeResult(i+1,j+1,ipStress[2]);
				m_femGrid->addNodeResult(i,j+1,ipStress[3]);

				//
				// Average stresses from integration points. Is this correct?? CHECK!
				//

				RowVector Es_avg = Es.sum_columns() / 4.0;
				
				//
				// Calculate principal stresses
				//

				sigx=Es_avg(1);
				sigy=Es_avg(2);
				tau=Es_avg(3);

				double ds = (sigx-sigy)/2.0;
				double R = sqrt(pow(ds,2)+pow(tau,2));						

				double sig1 = (sigx+sigy)/2.0+R; 
				double sig2 = (sigx+sigy)/2.0-R; 
				double alfa = atan2(tau,ds)/2.0;

				double misesStress = sqrt(	pow(sig1,2) - sig1*sig2 + pow(sig2,2) );

				m_femGrid->setResult(i, j, 0, sig1);
				m_femGrid->setResult(i, j, 1, sig2);
				m_femGrid->setResult(i, j, 2, alfa);
				m_femGrid->setResult(i, j, 3, misesStress);

				if (misesStress>m_maxMisesStressValue)
					m_maxMisesStressValue = misesStress;
				
				if (fabs(sig1)>m_maxStressValue)
					m_maxStressValue = fabs(sig1);
				
				if (fabs(sig2)>m_maxStressValue)
					m_maxStressValue = fabs(sig2);
				
				if (sig1<0.0)
					if (fabs(sig1)>m_maxNegStressValue)
						m_maxNegStressValue = fabs(sig1);
					
				if (sig1>0.0)
					if (fabs(sig1)>m_maxPosStressValue)
						m_maxPosStressValue = sig1;
						
				if (sig2<0.0)
					if (fabs(sig2)>m_maxNegStressValue)
						m_maxNegStressValue = fabs(sig2);
							
				if (sig2>0.0)
					if (fabs(sig2)>m_maxPosStressValue)
						m_maxPosStressValue = sig2;
			}
		}
	}

	m_femGrid->averageNodeResults();

	//
	// Calculate reaction forces from vector constraints
	//

	if (vectorBcsDefined)
	{
		double x1, y1, x2, y2, ex, ey, es;
		RowVector BarEx(2);
		RowVector BarEy(2);
		RowVector BarEp(2);
		RowVector BarEd(4);

		double maxReactionForce = -1e300;

		BarEp << E << m_constraintStiffnessScale;

		for (vci=vectorConstraints.begin(); vci!=vectorConstraints.end(); vci++)
		{
			CConstraint* constraint = *vci;
			constraint->getPosition(x2, y2);
			m_femGrid->getNearestDofs((int)x2, (int)y2, dofs);

			constraint->getDirection(ex, ey);

			x1 = x2 - ex;
			y1 = y2 - ey;

			BarEx << x1 << x2;
			BarEy << y1 << y2;
			BarEd << 0.0 << 0.0 << m_a(dofs[0]) << m_a(dofs[1]) ;

			calfem::bar2s(BarEx, BarEy, BarEp, BarEd, es);

			if (fabs(es)>maxReactionForce)
				maxReactionForce = fabs(es);

			constraint->getReactionForce()->setValue(es);
		}

		for (vci=vectorConstraints.begin(); vci!=vectorConstraints.end(); vci++)
		{
			CConstraint* constraint = *vci;

			double force = 100.0 * constraint->getReactionForce()->getValue()/maxReactionForce;
			constraint->getReactionForce()->setLength(-force);
		}
	}

	//
	// Store max stress in FEM model
	//

	m_femGrid->setMaxStressValue(m_maxStressValue);
	m_femGrid->setMaxPosStressValue(m_maxPosStressValue);
	m_femGrid->setMaxNegStressValue(m_maxNegStressValue);
	m_femGrid->setMaxNodeValue(m_maxNodeValue);
	m_femGrid->setMaxMisesStressValue(m_maxMisesStressValue);
}

void CFemGridSolver2::setElementTreshold(double treshold)
{
	m_elementTreshold = treshold;
}

double CFemGridSolver2::getMaxPosStressValue()
{
	return m_maxPosStressValue;
}

double CFemGridSolver2::getMaxNegStressValue()
{
	return m_maxNegStressValue;
}

int CFemGridSolver2::getDofs()
{
	return m_femGrid->enumerateDofs(ED_BOTTOM_TOP);
}

void CFemGridSolver2::setWeight(double weight)
{
	m_weight = weight;
}

void CFemGridSolver2::setForceMagnitude(double value)
{
	m_forceMagnitude = value;
}

void CFemGridSolver2::setUseWeight(bool flag)
{
	m_useWeight = flag;
}

void CFemGridSolver2::setOutputMatlab(bool flag)
{
	m_outputMatlab = flag;
}

void CFemGridSolver2::setMatlabFilename(const char *name)
{
	m_matlabFilename = name;
}

void CFemGridSolver2::setStiffnessScalefactor(const double scalefactor)
{
	m_stiffnessScalefactor = scalefactor;
}

double CFemGridSolver2::getStiffnessScalefactor()
{
	return m_stiffnessScalefactor;
}

void CFemGridSolver2::setElasticModulus(double elasticModulus)
{
	m_elasticModulus = elasticModulus;
}

void CFemGridSolver2::setThickness(double thickness)
{
	m_thickness = thickness;
}

void CFemGridSolver2::setYoungsModulus(double youngsModulus)
{
	m_youngsModulus = youngsModulus;
}

void CFemGridSolver2::setConstraintStiffnessScale(double scale)
{
	m_constraintStiffnessScale = scale;
}

double CFemGridSolver2::getConstraintStiffnessScale()
{
	return m_constraintStiffnessScale;
}

void CFemGridSolver2::setStatusMessageEvent(CGSStatusMessageEvent* eventMethod)
{
	m_statusMessageEvent = eventMethod;
}

void CFemGridSolver2::setLogMessageEvent(CGSLogMessageEvent* eventMethod)
{
	m_logMessageEvent = eventMethod;
}

void CFemGridSolver2::setContinueCalcEvent(CGSContinueCalcEvent* eventMethod)
{
	m_continueCalcEvent = eventMethod;
}

void CFemGridSolver2::setOptVolumeFraction(double fraction)
{
	m_optVolfrac = fraction;
}

double CFemGridSolver2::getOptVolumeFraction()
{
	return m_optVolfrac;
}

void CFemGridSolver2::setOptRmin(double rmin)
{
	m_optRmin = rmin;
}

double CFemGridSolver2::getOptRmin()
{
	return m_optRmin;
}

void CFemGridSolver2::setOptMinChange(double minChange)
{
	m_optMinChange = minChange;
}

double CFemGridSolver2::getOptMinChange()
{
	return m_optMinChange;
}

void CFemGridSolver2::setOptMaxLoops(int loops)
{
	m_optMaxLoops = loops;
}

int CFemGridSolver2::getOptMaxLoops()
{
	return m_optMaxLoops;
}

void CFemGridSolver2::setOptFilterType(TFilterType filterType)
{
	m_filterType = filterType;
}

CFemGridSolver2::TFilterType CFemGridSolver2::getOptFilterType()
{
	return m_filterType;
}

void CFemGridSolver2::execute()
{
	int i;
	int maxBandwidth;
	bool loadsDefined, bcsDefined, vectorBcsDefined;
	set<int> uniqueDofs;
	set<int> uniqueVectorDofs;
	vector<CConstraint*> vectorConstraints;
	RowVector prescribedValues;

#ifdef WIN32
	DWORD startTime = timeGetTime();
#endif

	//
	// Reset error status
	//

	m_errorStatus = CFemGridSolver2::ET_NO_ERROR;

	///////////////////////////////////////////////////////////////////////////
	// Apply hinge constraints directly to stiffness grid
	//

	setupHinges();

	///////////////////////////////////////////////////////////////////////////
	// Calculate optimum bandwidth
	//

	maxBandwidth = this->calculateOptimalBandwidth();
	
	if (m_nDof==0)
	{
		m_errorStatus = ET_NO_ELEMENTS;
		return;
	}

	///////////////////////////////////////////////////////////////////////////
	// Setup system matrices
	//

	this->progressMessage("Assembling system matrix.", 20);

	ColumnVector fsys;
	ColumnVector gdof;
	ColumnVector ldof;
	ColumnVector GlobalA;

	int nElements;

	SymmetricBandMatrix K(m_nDof,maxBandwidth);
	K = 0.0;

	///////////////////////////////////////////////////////////////////////////
	// Assemble system
	//

	nElements = this->assembleSystem(K);

	//
	// If no elements where assembled something was wrong return 
	// with error.
	//

	if (nElements==0)
	{
		m_errorStatus = ET_NO_ELEMENTS;
		return;
	}

	//so_print("CFemGridSolver2",so_format("%d elements assembled.",nElements));

	///////////////////////////////////////////////////////////////////////////
	// Setup forces and constraints
	//

	if ((m_femGrid->getPointLoadSize()==0)&&(m_Eq(1)==0.0)&&(m_Eq(2)==0.0))
	{
		m_errorStatus = ET_NO_LOADS;
		return;
	}

	if (m_femGrid->getPointConstraintsSize()==0)
	{
		m_errorStatus = ET_NO_BCS;
		return;
	}

	setupForcesAndConstraints(loadsDefined, bcsDefined, vectorBcsDefined, uniqueDofs, uniqueVectorDofs, vectorConstraints, prescribedValues);

	if ((!bcsDefined)&&(!vectorBcsDefined))
	{
		m_errorStatus = ET_BC_OUTSIDE_AE;
		return;
	}

	if (!loadsDefined)
	{
		m_errorStatus = ET_LOAD_OUTSIDE_AE;
		return;
	}

	///////////////////////////////////////////////////////////////////////////
	// Assemble Vector Constraints
	//

	if (vectorBcsDefined)
		this->assembleVectorConstraints(K, vectorConstraints);

	Matrix Bc(uniqueDofs.size(),2);
	this->removeDoubleDofs(uniqueDofs, prescribedValues, Bc);

	///////////////////////////////////////////////////////////////////////////
	// Solve system
	//

	m_a.ReSize(m_nDof,1);
	m_a = 0.0;

	//so_print("CFemGridSolver2","Solving system.");
	progressMessage("Solving.", 60);

	Try 
	{
		m_X = K; // LU decomposition
		m_a = m_X.i() * m_f;
	}
	CatchAll 
	{
		m_errorStatus = ET_INVALID_MODEL;
		return;
	}

	//so_print("CFemGridSolver2","Done.");

	///////////////////////////////////////////////////////////////////////////
	// Create global displacement vector
	//

	//so_print("CFemGridSolver2","\tCreating global displacement vector.");

	progressMessage("Storing results.", 80);

	m_maxNodeValue = -1.0e300;

	m_femGrid->setDisplacementSize(m_nDof);

	for (i=1; i<=K.Nrows(); i++)
	{
		m_femGrid->setDisplacement(i, m_a(i));
		if (fabs(m_a(i))>m_maxNodeValue)
			m_maxNodeValue = fabs(m_a(i));
	}

	///////////////////////////////////////////////////////////////////////////
	// Store element forces in elements
	//

	this->computeElementForces();
	
	///////////////////////////////////////////////////////////////////////////
	// Calculate reaction forces from vector constraints
	//

	if (vectorBcsDefined)
		this->computeReactionForces(vectorConstraints);

	///////////////////////////////////////////////////////////////////////////
	// Store max stress in FEM model
	//

	m_femGrid->setMaxStressValue(m_maxStressValue);
	m_femGrid->setMaxPosStressValue(m_maxPosStressValue);
	m_femGrid->setMaxNegStressValue(m_maxNegStressValue);
	m_femGrid->setMaxNodeValue(m_maxNodeValue);
	m_femGrid->setMaxMisesStressValue(m_maxMisesStressValue);

#ifdef WIN32
	DWORD endTime = timeGetTime();
	cout << "Total execution time (ms) = " << endTime - startTime << endl;
#endif

	progressMessage("Finished.", 99);
	progressMessage("Finished.", 0);
}
