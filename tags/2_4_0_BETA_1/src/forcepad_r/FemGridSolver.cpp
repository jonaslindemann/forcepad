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


#include "FemGridSolver.h"

#include "calfem.h"

#include "StatusOutput.h"

#ifndef NO_FLTK_DIALOGS
#include "StatusDlg.h"
#endif

//#include "StructuralElement.h"
//#include "StructuralElementPair.h"

//#define WANT_STREAM
//#define WANT_MATH

#include <set>

CFemGridSolver::CFemGridSolver()
{
	so_print("CFemGridSolver","Constructed.");
	m_maxNodeValue = -1.0e300;
	m_maxStressValue = -1.0e300;
	m_errorStatus = CFemGridSolver::ET_NO_ERROR;
	m_elementTreshold = 1.0e-5;
	m_forceMagnitude = 1.0;
	m_weight = 0.0;
	m_useWeight = false;
}

CFemGridSolver::~CFemGridSolver()
{
	so_print("CFemGridSolver","Destroyed.");
}


CFemGridSolver::TErrorType CFemGridSolver::getLastError()
{
	return m_errorStatus;
}

void CFemGridSolver::setFemGrid(CFemGrid *femGrid)
{
	m_femGrid = femGrid;
}

CFemGrid* CFemGridSolver::getFemGrid()
{
	return m_femGrid;
}

double CFemGridSolver::getMaxStressValue()
{
	return m_maxStressValue;
}

double CFemGridSolver::getMaxNodeValue()
{
	return m_maxNodeValue;
}

void CFemGridSolver::execute()
{
	int i, j, k, l;
	int rows, cols;
	float elementValue;
	double ex[3];
	double ey[3];
	int topo[6];

	int maxBandwidth;
	int nDof;
	int bwLeftRight;
	int bwBottomTop;

	//
	// Bring up status window
	//

#ifndef NO_FLTK_DIALOGS
	CStatusDlg* dlg = new CStatusDlg();
	dlg->show();
	dlg->refresh();
#endif
	//
	// Reset error status
	//

	m_errorStatus = CFemGridSolver::ET_NO_ERROR;

	//
	// Calculate optimum bandwidth
	//

	m_femGrid->getGridSize(rows, cols);

	so_print("CFemGridSolver",so_format("Gridsize = [ %d, %d ].",rows, cols));
	//so_print("CFemGridSolver","Enumerating dofs and calculating bandwidth." << endl << endl;

	nDof =  m_femGrid->enumerateDofs(ED_LEFT_RIGHT);
	bwLeftRight = m_femGrid->getBandwidth()-1;

	m_femGrid->resetDofs();
	nDof =  m_femGrid->enumerateDofs(ED_BOTTOM_TOP);
	bwBottomTop = m_femGrid->getBandwidth()-1;

	if (bwLeftRight<bwBottomTop)
	{
		m_femGrid->resetDofs();
		nDof = m_femGrid->enumerateDofs(ED_LEFT_RIGHT);
		maxBandwidth = bwLeftRight;
	}
	else
		maxBandwidth = bwBottomTop;

	so_print("CFemGridSolver",so_format("Total dofs = %d",nDof));
	so_print("CFemGridSolver",so_format("Bandwidth  = %d",maxBandwidth));
	
	if (nDof==0)
	{
		m_errorStatus = ET_NO_ELEMENTS;
#ifndef NO_FLTK_DIALOGS
		delete dlg;
#endif
		return;
	}

	//
	// Assemble system
	//

	SymmetricBandMatrix K(nDof,maxBandwidth);
	K = 0.0;

	so_print("CFemGridSolver","Assembling system matrix.");

#ifndef NO_FLTK_DIALOGS
	dlg->setStatusText("Assembling system matrix.");
	dlg->setProgress(20);
#endif

	ColumnVector f;
	ColumnVector fsys;
	ColumnVector gdof;
	ColumnVector ldof;
	ColumnVector GlobalA;
	ColumnVector a;

	double E = 2.1e9;
	double v = 0.35;
	double t = 1.0;
	int ptype = 1;    // Plain stress

	RowVector Ex(3);
	RowVector Ey(3);
	RowVector Eq(2);
	Eq = 0.0;
	if (m_useWeight)
		Eq(2)=-1.0;
	else
		Eq(2)=0.0;
	int elementsWithWeight = 0;

	RowVector Ep(2);
	Ep << ptype << t;

	Matrix Ke(6,6);
	ColumnVector fe(6);
	f.ReSize(nDof,1);
	f = 0.0;
	RowVector DofTopo(6);

	Matrix D;

	int nElements = 0;

	for (i=0; i<rows; i++)
	{
		for (j=0; j<cols; j++)
		{
			for (k=0; k<2; k++)
			{
				if (m_femGrid->getElement(i, j, k, elementValue, ex, ey, topo))
				{
					//
					// Get element coordinates
					//

					Ex(1) = ex[0]; Ey(1) = ey[0];
					Ex(2) = ex[1]; Ey(2) = ey[1];
					Ex(3) = ex[2]; Ey(3) = ey[2];
					
					// 
					// Get element properties
					//
					
					calfem::hooke(ptype, E*(double)elementValue, v, D);

					// 
					// Get element topology
					//
					
					for (l=0; l<6; l++)
					{
						DofTopo(l+1) = topo[l];
					}

					// 
					// Create element matrix
					//
					
					calfem::plante(Ex, Ey, Ep, D, Eq, Ke, fe);

					//
					// Assemble element matrix
					//
					
					calfem::assem(DofTopo, K, Ke, f, fe);
					nElements++;

					//
					// Add to total weight
					//

					elementsWithWeight++;
				}
			}
		}
	}

	if (nElements==0)
	{
		m_errorStatus = ET_NO_ELEMENTS;
#ifndef NO_FLTK_DIALOGS
		delete dlg;
#endif
		return;
	}

	so_print("CFemGridSolver",so_format("%d elements assembled.",nElements));

	//
	// Scale load vector for body weight
	//

	if (m_useWeight)
	{
		so_print("CFemGridSolver","Applying weight.");
		
		double Fsum = 0.0;
		
		for (i=2; i<=nDof; i+=2)
			Fsum+=f(i);
		
		for (i=2; i<=nDof; i+=2)
			f(i) = -m_weight*f(i)/Fsum;
	}

	//
	// Setup load vector
	//

	so_print("CFemGridSolver","Defining load vector.");

#ifndef NO_FLTK_DIALOGS
	dlg->setStatusText("Defining load vector.");
	dlg->setProgress(30);
#endif

	bool loadsDefined = false;
	double x, y;
	double vx, vy;
	double value;
	int dofs[2];

	m_femGrid->clearPoints();

	if ((m_femGrid->getPointLoadSize()==0)&&(Eq(1)==0.0)&&(Eq(2)==0.0))
	{
		m_errorStatus = ET_NO_LOADS;
#ifndef NO_FLTK_DIALOGS
		delete dlg;
#endif
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
			so_print("CFemGridSolver",so_format("\tLoad found at [ %g, %g ], dofs = [ %d, %d ]",x, y, dofs[0], dofs[1]));

			f(dofs[0]) += vx*m_forceMagnitude;
			f(dofs[1]) += vy*m_forceMagnitude;

			loadsDefined = true;
		}
		
		pointLoad = m_femGrid->getNextPointLoad();
	}

	if ((Eq(1)!=0.0)||(Eq(2)!=0.0))
		loadsDefined = true;

	if (!loadsDefined)
	{
		m_errorStatus = ET_LOAD_OUTSIDE_AE;
#ifndef NO_FLTK_DIALOGS
		delete dlg;
#endif
		return;
	}

	//
	// Define constraints
	//

	so_print("CFemGridSolver","Defining constraints.");

#ifndef NO_FLTK_DIALOGS
	dlg->setStatusText("Setting up boundary conditions.");
	dlg->setProgress(40);
#endif

	if (m_femGrid->getPointConstraintsSize()==0)
	{
		m_errorStatus = ET_NO_BCS;
#ifndef NO_FLTK_DIALOGS
		delete dlg;
#endif
		return;
	}

	int nConstraints = m_femGrid->getPointConstraintsSize();
	CConstraint* pointConstraint = m_femGrid->getFirstPointConstraint();

	set<int> uniqueDofs;
	set<int>::iterator si;
	double* prescribedValues = new double[nDof+1];
	bool bcsDefined = false;
	
	while (pointConstraint!=NULL) 
	{
		pointConstraint->getPosition(x, y);
		value = pointConstraint->getValue();
		m_femGrid->getNearestDofs((int)x, (int)y, dofs);

		if (dofs[0]>0)
		{
			so_print("CFemGridSolver",so_format("\tConstraint found at [ %g, %g ], dofs = [ %d, %d ]",x, y, dofs[0], dofs[1]));

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
			default:
				uniqueDofs.insert(dofs[0]);
				uniqueDofs.insert(dofs[1]);
				prescribedValues[dofs[0]] = 0.0;
				prescribedValues[dofs[1]] = 0.0;
				bcsDefined = true;
				break;
			}
		}
			
		pointConstraint = m_femGrid->getNextPointConstraint();
	}

	if (!bcsDefined)
	{
		m_errorStatus = ET_BC_OUTSIDE_AE;
#ifndef NO_FLTK_DIALOGS
		delete dlg;
#endif
		return;
	}

	// 
	// Remove doubly defined dofs
	//

	so_print("CFemGridSolver","\tUnique dofs:\n");

	Matrix Bc(uniqueDofs.size(),2);
	Bc = 0.0;
	int bcCount = 1;

	for (si=uniqueDofs.begin(); si!=uniqueDofs.end(); si++)
	{
		int dof = (*si);
		so_print("CFemGridSolver",so_format("\t%d",dof));
		
		Bc(bcCount,1) = dof;
		Bc(bcCount,2) = prescribedValues[dof];
		bcCount++;
	}

	delete [] prescribedValues;
	uniqueDofs.clear();

	// 
	// Remove boundary conditions from full system matrix
	// This is not very effective!
	// WARNING!! routine does not handle prescribed displacements!=0.0
	//

	so_print("CFemGridSolver","Removing boundary conditions from system matrix.");

#ifndef NO_FLTK_DIALOGS
	dlg->setStatusText("Removing bc:s from system matrix.");
	dlg->setProgress(50);
#endif

	RowVector Idx(nDof);
	Idx = 0.0;
	int nVars;

	for (i=1; i<=Bc.Nrows(); i++)
		Idx((int)Bc(i,1)) = 1.0;

	nVars = K.Nrows() - Bc.Nrows();

	so_print("CFemGridSolver",so_format("\tFree variables = %d",nVars));

	if (maxBandwidth>nVars)
		maxBandwidth = nVars;

	so_print("CFemGridSolver","\tCreating reduced system matrix.");
	SymmetricBandMatrix Ksys(nVars,maxBandwidth);

	fsys.ReSize(K.Nrows()-Bc.Nrows(),1);
	gdof.ReSize(K.Nrows() - Bc.Nrows(),1);
	ldof.ReSize(K.Nrows(),1);

	so_print("CFemGridSolver","\tZeroing matrices.");
	Ksys = 0.0;
	fsys = 0.0;
	gdof = 0.0;
	ldof = 0.0;

	int row = 1;
	int col = 1;

	so_print("CFemGridSolver","\tCopying K into Ksys.");
	for (i=1; i<=K.Nrows(); i++)
	{
		if (Idx(i)<1.0)
		{
			col=row;
			for (j=i; j<=K.Ncols(); j++)
			{
				if (Idx(j)<1.0)
				{
					if (abs(row-col)<=maxBandwidth)
						if (abs(i-j)<=maxBandwidth)
							Ksys(row, col) = K(i,j);	
					col++;
				}
			}
			fsys(row) = f(i);
			gdof(row) = i;
			ldof(i) = row;
			row++;
		}
		else
		{
			ldof(i) = -1.0;
		}
	}

	so_print("CFemGridSolver","\tRemoving K.");
	K.CleanUp();

	// 
	// Solve system
	//

	a.ReSize(nVars,1);
	a = 0.0;

	so_print("CFemGridSolver","Solving reduced system.");

#ifndef NO_FLTK_DIALOGS
	dlg->setStatusText("Solving.");
	dlg->setProgress(60);
#endif

	Try 
	{
		LinearEquationSolver X = Ksys;
		a = X.i() * fsys;
	}
	CatchAll 
	{
		m_errorStatus = ET_INVALID_MODEL;
#ifndef NO_FLTK_DIALOGS
		delete dlg;
#endif
		return;
	}

	so_print("CFemGridSolver","Done.");

	so_print("CFemGridSolver","Calculating results.");

	//
	// Create global displacement vector
	//

	so_print("CFemGridSolver","\tCreating global displacement vector.");

#ifndef NO_FLTK_DIALOGS
	dlg->setStatusText("Storing results.");
	dlg->setProgress(80);
#endif

	GlobalA.ReSize(nDof);
	GlobalA = 0.0;
	m_maxNodeValue = -1.0e300;

	m_femGrid->setDisplacementSize(nDof);

	for (i=1; i<=Ksys.Nrows(); i++)
	{
		GlobalA((int)gdof(i)) = a(i);
		m_femGrid->setDisplacement((int)gdof(i), a(i));
		//so_print( a(i) << endl;
		if (fabs(a(i))>m_maxNodeValue)
			m_maxNodeValue = fabs(a(i));
	}

	//
	// Store element forces in elements
	//
	
	RowVector Ed(6);
	RowVector Es(6); Es = -1.0;
	RowVector Et(6); Et = -1.0;
	Ed = 0.0;
	m_maxStressValue = -1.0e300;
	m_maxPosStressValue = -1.0e300;
	m_maxNegStressValue = -1.0e300;

	so_print("CFemGridSolver:","\tCalculating element forces.");

	for (i=0; i<rows; i++)
	{
		for (j=0; j<cols; j++)
		{
			double sigx[2];
			double sigy[2];
			double tau[2];

			for (k=0; k<2; k++)
			{
				if (m_femGrid->getElement(i, j, k, elementValue, ex, ey, topo))
				{
					//
					// Get element coordinates
					//

					Ex(1) = ex[0]; Ey(1) = ey[0];
					Ex(2) = ex[1]; Ey(2) = ey[1];
					Ex(3) = ex[2]; Ey(3) = ey[2];
					
					// 
					// Get element properties
					//
					
					calfem::hooke(ptype, E*(double)elementValue, v, D);

					// 
					// Get element topology
					//
					
					for (l=0; l<6; l++)
						DofTopo(l+1) = topo[l];

					//
					// Get element displacements
					//
					
					for (l=1; l<=6; l++)
						Ed(l) = GlobalA(DofTopo(l));

					// 
					// Calculate element forces 
					//
					
					calfem::plants(Ex, Ey, Ep, D, Ed, Es, Et);
					
					//
					// Calculate principal stresses
					//

					if (m_femGrid->getAverageStress()) 
					{
						sigx[k]=Es(1);
						sigy[k]=Es(2);
						tau[k]=Es(3);
					}
					else
					{
						double sigx = Es(1);
						double sigy = Es(2);
						double tau = Es(3);
						double ds = (sigx-sigy)/2.0;
						double R = sqrt(pow(ds,2)+pow(tau,2));
						
						double values[3];
						
						double sig1 = (sigx+sigy)/2.0+R; 
						double sig2 = (sigx+sigy)/2.0-R; 
						double alfa = atan2(tau,ds)/2.0;
						
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
										
						values[0] = sig1;
						values[1] = sig2;
						values[2] = alfa;
										
						m_femGrid->setResult(i, j, k, values);
					}
				}
			}

			if ((m_femGrid->getElement(i, j, 0, elementValue, ex, ey, topo))
				&&(m_femGrid->getElement(i, j, 0, elementValue, ex, ey, topo)))
			{
				if (m_femGrid->getAverageStress())
				{
					double sigx_avg = (sigx[0]+sigx[1])*0.5;
					double sigy_avg = (sigy[0]+sigy[1])*0.5;
					double tau_avg = (tau[0]+tau[1])*0.5;
					double ds = (sigx_avg-sigy_avg)/2.0;
					double R = sqrt(pow(ds,2)+pow(tau_avg,2));
					
					double values[3];
					
					double sig1 = (sigx_avg+sigy_avg)/2.0+R; 
					double sig2 = (sigx_avg+sigy_avg)/2.0-R; 
					double alfa = atan2(tau_avg,ds)/2.0;
					
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
									
					values[0] = sig1;
					values[1] = sig2;
					values[2] = alfa;
					
					m_femGrid->setResult(i, j, 0, values);
					m_femGrid->setResult(i, j, 1, values);
				}
			}
		}
	}

	
	//
	// Store max stress in FEM model
	//

	m_femGrid->setMaxStressValue(m_maxStressValue);
	m_femGrid->setMaxPosStressValue(m_maxPosStressValue);
	m_femGrid->setMaxNegStressValue(m_maxNegStressValue);
	m_femGrid->setMaxNodeValue(m_maxNodeValue);

#ifndef NO_FLTK_DIALOGS
	dlg->setProgress(99);
	dlg->hide();
	delete dlg;
#endif
}

void CFemGridSolver::setElementTreshold(double treshold)
{
	m_elementTreshold = treshold;
}

double CFemGridSolver::getMaxPosStressValue()
{
	return m_maxPosStressValue;
}

double CFemGridSolver::getMaxNegStressValue()
{
	return m_maxNegStressValue;
}

int CFemGridSolver::getDofs()
{
	return m_femGrid->enumerateDofs(ED_BOTTOM_TOP);
}

void CFemGridSolver::setWeight(double weight)
{
	m_weight = weight;
}

void CFemGridSolver::setForceMagnitude(double value)
{
	m_forceMagnitude = value;
}

void CFemGridSolver::setUseWeight(bool flag)
{
	m_useWeight = flag;
}
