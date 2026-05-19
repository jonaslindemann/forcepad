//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2007 Division of Structural Mecahnics, Lund University
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
#include "matlabgen.h"
#include "LogWindow.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <set>
#include <fstream>

FemGridSolver::FemGridSolver()
{
	so_print("FemGridSolver","Constructed.");
	m_maxNodeValue = -1.0e300;
	m_maxStressValue = -1.0e300;
	m_maxMisesStressValue = -1.0e300;
	m_errorStatus = FemGridSolver::ET_NO_ERROR;
	m_elementTreshold = 1.0e-5;
	m_forceMagnitude = 1.0;
	m_weight = 0.0;
	m_useWeight = false;
	m_outputMatlab = false;
	m_stiffnessScalefactor = 1000.0;
	m_elasticModulus = 2.1e9;
	m_youngsModulus = 0.35;
	m_thickness = 1.0;
	m_ptype = 1;
	m_constraintStiffnessScale = 1e6;
	m_reduceSystemMatrix = false;

	// Events

	m_statusMessageEvent = NULL;
	m_logMessageEvent = NULL;
}

FemGridSolver::~FemGridSolver()
{
	so_print("FemGridSolver","Destroyed.");
}

void FemGridSolver::progressMessage(const std::string message, const int progress)
{
	if (m_statusMessageEvent!=NULL)
	{
		m_statusMessageEvent->onStatusMessage(message, progress);
	}
}

void FemGridSolver::logMessage(const std::string context, const std::string message)
{
	if (m_logMessageEvent!=NULL)
	{
		m_logMessageEvent->onLogMessage(context, message);
	}
}

FemGridSolver::TErrorType FemGridSolver::getLastError()
{
	return m_errorStatus;
}

void FemGridSolver::setFemGrid(fp::FemGrid *femGrid)
{
	m_femGrid = femGrid;
}

fp::FemGrid* FemGridSolver::getFemGrid()
{
	return m_femGrid;
}

double FemGridSolver::getMaxStressValue()
{
	return m_maxStressValue;
}

double FemGridSolver::getMaxNodeValue()
{
	return m_maxNodeValue;
}

void FemGridSolver::execute()
{
	int i, j, k, l;
	int rows, cols;
	double elementValue;
	double ex[3];
	double ey[3];
	int topo[6];

	int maxBandwidth;
	int bwLeftRight;
	int bwBottomTop;

#ifdef WIN32
	DWORD startTime = timeGetTime();
#endif

	//
	// Reset error status
	//

	m_errorStatus = FemGridSolver::ET_NO_ERROR;

	//
	// Apply hinge constraints directly to stiffness grid
	//

	fp::Constraint* pointConstraint = m_femGrid->getFirstPointConstraint();

	while (pointConstraint!=NULL) 
	{
		switch (pointConstraint->getConstraintType()) {
		case fp::Constraint::CT_HINGE:
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

	logMessage("FemGridSolver","Enumerating dofs and calculating bandwidth.");

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

    //so_print("FemGridSolver",so_format("Total dofs = %d",m_nDof));
    //so_print("FemGridSolver",so_format("Bandwidth  = %d",maxBandwidth));
	
	if (m_nDof==0)
	{
		m_errorStatus = ET_NO_ELEMENTS;
		return;
	}

	//
	// Assemble system
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

	RowVector Ex(3);
	RowVector Ey(3);
	m_Eq.ReSize(2);
	m_Eq = 0.0;
	if (m_useWeight)
		m_Eq(2)=-1.0;
	else
		m_Eq(2)=0.0;
	int elementsWithWeight = 0;

	RowVector Ep(2);
	Ep << ptype << t;

	Matrix Ke(6,6);
	ColumnVector fe(6);
	m_f.ReSize(m_nDof,1);
	m_f = 0.0;
	RowVector DofTopo(6);

	Matrix D;

	int nElements = 0;

	Matrix Ex_out;
	Matrix Ey_out;
	Matrix Topo_out;

	if (m_outputMatlab)
	{
		nElements = m_femGrid->getElementCount();

		Ex_out.ReSize(nElements, 3);
		Ey_out.ReSize(nElements, 3);
		Topo_out.ReSize(nElements, 6);
	}

	nElements = 0;

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
					// Store element coordinates
					//

					if (m_outputMatlab)
					{
						Ex_out(nElements+1, 1) = Ex(1);
						Ex_out(nElements+1, 2) = Ex(2);
						Ex_out(nElements+1, 3) = Ex(3);

						Ey_out(nElements+1, 1) = Ey(1);
						Ey_out(nElements+1, 2) = Ey(2);
						Ey_out(nElements+1, 3) = Ey(3);
					}

					// 
					// Get element properties
					//
					
					calfem::hooke(ptype, E*(double)elementValue*m_stiffnessScalefactor, v, D);

					// 
					// Get element topology
					//
					
					for (l=0; l<6; l++)
					{
						DofTopo(l+1) = topo[l];

						if (m_outputMatlab)
							Topo_out(nElements+1,l+1) = topo[l];
					}

					// 
					// Create element matrix
					//
					
					calfem::plante(Ex, Ey, Ep, D, m_Eq, Ke, fe);

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
	}

	if (nElements==0)
	{
		m_errorStatus = ET_NO_ELEMENTS;
		return;
	}

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

    //so_print("FemGridSolver",so_format("%d elements assembled.",nElements));

	///////////////////////////////////////////////////////////////////////////
	// Scale load vector for body weight
	///////////////////////////////////////////////////////////////////////////

	if (m_useWeight)
	{
		so_print("FemGridSolver","Applying weight.");
		
		double Fsum = 0.0;
		
		for (i=2; i<=m_nDof; i+=2)
			Fsum+=m_f(i);
		
		for (i=2; i<=m_nDof; i+=2)
			m_f(i) = -m_weight*m_f(i)/Fsum;
	}

	///////////////////////////////////////////////////////////////////////////
	// Setup load vector
	///////////////////////////////////////////////////////////////////////////

	so_print("FemGridSolver","Defining load vector.");

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

	fp::Force* pointLoad = m_femGrid->getFirstPointLoad();

	while (pointLoad!=NULL) 
	{
		pointLoad->getPosition(x, y);
		value = pointLoad->getValue();
		pointLoad->getDirection(vx, vy);
		m_femGrid->getNearestDofs((int)x, (int)y, dofs);

		if (dofs[0]>0)
		{
            //so_print("FemGridSolver",so_format("\tLoad found at [ %g, %g ], dofs = [ %d, %d ]",x, y, dofs[0], dofs[1]));

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
	// Define constraints
	//

	so_print("FemGridSolver","Defining constraints.");
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

	vector<fp::Constraint*> vectorConstraints;
	vector<fp::Constraint*>::iterator vci;

	double* prescribedValues = new double[m_nDof+1];
	bool bcsDefined = false;
	bool vectorBcsDefined = false;
	
	while (pointConstraint!=NULL) 
	{
		pointConstraint->getPosition(x, y);
		value = pointConstraint->getValue();
		m_femGrid->getNearestDofs((int)x, (int)y, dofs);

		if (dofs[0]>0)
		{
            //so_print("FemGridSolver",so_format("\tConstraint found at [ %g, %g ], dofs = [ %d, %d ]",x, y, dofs[0], dofs[1]));

			switch (pointConstraint->getConstraintType()) {
			case fp::Constraint::CT_XY:
				uniqueDofs.insert(dofs[0]);
				uniqueDofs.insert(dofs[1]);
				prescribedValues[dofs[0]] = 0.0;
				prescribedValues[dofs[1]] = 0.0;
				bcsDefined = true;
				break;
			case fp::Constraint::CT_X:
				uniqueDofs.insert(dofs[0]);
				prescribedValues[dofs[0]] = 0.0;
				bcsDefined = true;
				break;
			case fp::Constraint::CT_Y:
				uniqueDofs.insert(dofs[1]);
				prescribedValues[dofs[1]] = 0.0;
				bcsDefined = true;
				break;
			case fp::Constraint::CT_VECTOR:
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
			fp::Constraint* constraint = *vci;
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

	so_print("FemGridSolver","\tUnique dofs:\n");

	Matrix Bc(uniqueDofs.size(),2);
	Bc = 0.0;
	int bcCount = 1;

	for (si=uniqueDofs.begin(); si!=uniqueDofs.end(); si++)
	{
		int dof = (*si);
        //so_print("FemGridSolver",so_format("\t%d",dof));
		
		Bc(bcCount,1) = dof;
		Bc(bcCount,2) = prescribedValues[dof];
		bcCount++;
	}

	delete [] prescribedValues;
	uniqueDofs.clear();

	// Variables used when a reduced system matrix is needed.

	RowVector Idx(m_nDof);
	Idx = 0.0;
	int nVars;

	SymmetricBandMatrix Ksys;

	if (m_reduceSystemMatrix)
	{
		///////////////////////////////////////////////////////////////////////////
		// Remove boundary conditions from full system matrix
		///////////////////////////////////////////////////////////////////////////
		// 
		// This is not very effective!
		// WARNING!! routine does not handle prescribed displacements!=0.0
		//

		so_print("FemGridSolver","Removing boundary conditions from system matrix.");
		progressMessage("Removing bc:s from system matrix.", 50);


		for (i=1; i<=Bc.Nrows(); i++)
			Idx((int)Bc(i,1)) = 1.0;

		nVars = K.Nrows() - Bc.Nrows();

        //so_print("FemGridSolver",so_format("\tFree variables = %d",nVars));

		if (maxBandwidth>nVars)
			maxBandwidth = nVars;

		so_print("FemGridSolver","\tCreating reduced system matrix.");
		Ksys.ReSize(nVars,maxBandwidth);

		fsys.ReSize(K.Nrows()-Bc.Nrows(),1);
		gdof.ReSize(K.Nrows() - Bc.Nrows(),1);
		ldof.ReSize(K.Nrows(),1);

		so_print("FemGridSolver","\tZeroing matrices.");
		Ksys = 0.0;
		fsys = 0.0;
		gdof = 0.0;
		ldof = 0.0;

		int row = 1;
		int col = 1;

		so_print("FemGridSolver","\tCopying K into Ksys.");
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
				fsys(row) = m_f(i);
				gdof(row) = i;
				ldof(i) = row;
				row++;
			}
			else
			{
				ldof(i) = -1.0;
			}
		}

		so_print("FemGridSolver","\tRemoving K.");
		K.CleanUp();
	}

	//
	// Solve system
	//

	if (m_reduceSystemMatrix)
	{
		m_a.ReSize(nVars,1);
		m_a = 0.0;

		so_print("FemGridSolver","Solving reduced system.");
		progressMessage("Solving.", 60);

		Try 
		{
			LinearEquationSolver X = Ksys;
			m_a = X.i() * fsys;
		}
		CatchAll 
		{
			m_errorStatus = ET_INVALID_MODEL;
			return;
		}

		so_print("FemGridSolver","Done.");
	}
	else
	{
		m_a.ReSize(m_nDof,1);
		m_a = 0.0;

		so_print("FemGridSolver","Solving system.");
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

		so_print("FemGridSolver","Done.");
	}

	so_print("FemGridSolver","Calculating results.");

	//
	// Create global displacement vector
	//

	so_print("FemGridSolver","\tCreating global displacement vector.");

	progressMessage("Storing results.", 80);

	if (m_reduceSystemMatrix)
	{
		GlobalA.ReSize(m_nDof);
		GlobalA = 0.0;
		m_maxNodeValue = -1.0e300;

		m_femGrid->setDisplacementSize(m_nDof);

		for (i=1; i<=Ksys.Nrows(); i++)
		{
			GlobalA((int)gdof(i)) = m_a(i);
			m_femGrid->setDisplacement((int)gdof(i), m_a(i));
			if (fabs(m_a(i))>m_maxNodeValue)
				m_maxNodeValue = fabs(m_a(i));
		}
	}
	else
	{
		m_maxNodeValue = -1.0e300;

		m_femGrid->setDisplacementSize(m_nDof);

		for (i=1; i<=K.Nrows(); i++)
		{
			m_femGrid->setDisplacement(i, m_a(i));
			if (fabs(m_a(i))>m_maxNodeValue)
				m_maxNodeValue = fabs(m_a(i));
		}
	}

	//
	// Store element forces in elements
	//
	
	RowVector Ed(6);
	RowVector Es(6); Es = -1.0;
	RowVector Et(6); Et = -1.0;
	Ed = 0.0;
	m_maxStressValue = -1.0e300;
	m_maxMisesStressValue = -1.0e300;
	m_maxPosStressValue = -1.0e300;
	m_maxNegStressValue = -1.0e300;

	so_print("FemGridSolver:","\tCalculating element forces.");

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
					
					calfem::hooke(ptype, E*(double)elementValue*m_stiffnessScalefactor, v, D);

					// 
					// Get element topology
					//
					
					for (l=0; l<6; l++)
						DofTopo(l+1) = topo[l];

					//
					// Get element displacements
					//
					
					for (l=1; l<=6; l++)
						if (m_reduceSystemMatrix)
							Ed(l) = GlobalA(DofTopo(l));
						else
							Ed(l) = m_a(DofTopo(l));


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
						double values[4];

						double sigx = Es(1);
						double sigy = Es(2);
						double tau = Es(3);
						double ds = (sigx-sigy)/2.0;
						double R = sqrt(pow(ds,2)+pow(tau,2));						
						
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
										
						double misesStress = sqrt(	pow(sig1,2) - sig1*sig2 + pow(sig2,2) );

						if (misesStress>m_maxMisesStressValue)
							m_maxMisesStressValue = misesStress;

						values[0] = sig1;
						values[1] = sig2;
						values[2] = alfa;
						values[3] = misesStress;
						
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
					
					double values[4];
					
					double sig1 = (sigx_avg+sigy_avg)/2.0+R; 
					double sig2 = (sigx_avg+sigy_avg)/2.0-R; 
					double alfa = atan2(tau_avg,ds)/2.0;
					
					double misesStress = sqrt(	pow(sig1,2) - sig1*sig2 + pow(sig2,2) );

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
									
					values[0] = sig1;
					values[1] = sig2;
					values[2] = alfa;

					m_femGrid->setResult(i, j, 0, values);
					m_femGrid->setResult(i, j, 1, values);
					m_femGrid->setResult(i, j, 0, 3, misesStress);
					m_femGrid->setResult(i, j, 1, 3, misesStress);
				}
			}
		}
	}

	cout << "Max misses stress = " << m_maxMisesStressValue << endl;

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
			fp::Constraint* constraint = *vci;
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
			fp::Constraint* constraint = *vci;

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

void FemGridSolver::executeUpdate()
{
	int i, j, k, l;
	int rows, cols;
	double ex[3];
	double ey[3];
	int topo[6];
	double elementValue;

	double E = m_elasticModulus;
	double v = m_youngsModulus;
	double t = m_thickness;
	int ptype = m_ptype;    // Plane stress

	RowVector Ex(3);
	RowVector Ey(3);
	RowVector DofTopo(6);
	RowVector Ep(2);

	Matrix D;

	Ep << ptype << t;

	m_femGrid->getGridSize(rows, cols);

	//
	// Define constraints
	//

	if (m_femGrid->getPointConstraintsSize()==0)
	{
		m_errorStatus = ET_NO_BCS;
		return;
	}

	fp::Constraint* pointConstraint = m_femGrid->getFirstPointConstraint();

	set<int> uniqueDofs;
	set<int> uniqueVectorDofs;
	set<int>::iterator si;

	vector<fp::Constraint*> vectorConstraints;
	vector<fp::Constraint*>::iterator vci;

	double* prescribedValues = new double[m_nDof+1];
	bool bcsDefined = false;
	bool vectorBcsDefined = false;
	double x, y, value;
	int dofs[2];
	
	while (pointConstraint!=NULL) 
	{
		pointConstraint->getPosition(x, y);
		value = pointConstraint->getValue();
		m_femGrid->getNearestDofs((int)x, (int)y, dofs);

		if (dofs[0]>0)
		{
			switch (pointConstraint->getConstraintType()) {
			case fp::Constraint::CT_XY:
				uniqueDofs.insert(dofs[0]);
				uniqueDofs.insert(dofs[1]);
				prescribedValues[dofs[0]] = 0.0;
				prescribedValues[dofs[1]] = 0.0;
				bcsDefined = true;
				break;
			case fp::Constraint::CT_X:
				uniqueDofs.insert(dofs[0]);
				prescribedValues[dofs[0]] = 0.0;
				bcsDefined = true;
				break;
			case fp::Constraint::CT_Y:
				uniqueDofs.insert(dofs[1]);
				prescribedValues[dofs[1]] = 0.0;
				bcsDefined = true;
				break;
			case fp::Constraint::CT_VECTOR:
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

	fp::Force* pointLoad = m_femGrid->getFirstPointLoad();

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
	
	RowVector Ed(6);
	RowVector Es(6); Es = -1.0;
	RowVector Et(6); Et = -1.0;
	Ed = 0.0;
	m_maxStressValue = -1.0e300;
	m_maxMisesStressValue = -1.0e300;
	m_maxPosStressValue = -1.0e300;
	m_maxNegStressValue = -1.0e300;

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
					
					calfem::hooke(ptype, E*(double)elementValue*m_stiffnessScalefactor, v, D);

					// 
					// Get element topology
					//
					
					for (l=0; l<6; l++)
						DofTopo(l+1) = topo[l];

					//
					// Get element displacements
					//
					
					for (l=1; l<=6; l++)
						Ed(l) = m_a(DofTopo(l));


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
						double values[4];

						double sigx = Es(1);
						double sigy = Es(2);
						double tau = Es(3);
						double ds = (sigx-sigy)/2.0;
						double R = sqrt(pow(ds,2)+pow(tau,2));						
						
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
										
						double misesStress = sqrt(	pow(sig1,2) - sig1*sig2 + pow(sig2,2) );

						if (misesStress>m_maxMisesStressValue)
							m_maxMisesStressValue = misesStress;

						values[0] = sig1;
						values[1] = sig2;
						values[2] = alfa;
						values[3] = misesStress;
												
						m_femGrid->setResult(i, j, k, values);
					}
				}
			}

			if (m_femGrid->isSpecialElement(i,j, 0)&&(m_femGrid->isSpecialElement(i,j,1)))
			{
				double values[4];
				values[0] = 0.0;
				values[1] = 0.0;
				values[2] = 0.0;
				
				m_femGrid->setResult(i, j, 0, values);
				m_femGrid->setResult(i, j, 1, values);
				m_femGrid->setResult(i, j, 0, 3, 0.0);
				m_femGrid->setResult(i, j, 1, 3, 0.0);
			}
			else if ((m_femGrid->getElement(i, j, 0, elementValue, ex, ey, topo))
				&&(m_femGrid->getElement(i, j, 0, elementValue, ex, ey, topo)))
			{
				if (m_femGrid->getAverageStress())
				{
					double sigx_avg = (sigx[0]+sigx[1])*0.5;
					double sigy_avg = (sigy[0]+sigy[1])*0.5;
					double tau_avg = (tau[0]+tau[1])*0.5;
					double ds = (sigx_avg-sigy_avg)/2.0;
					double R = sqrt(pow(ds,2)+pow(tau_avg,2));
					
					double values[4];
					
					double sig1 = (sigx_avg+sigy_avg)/2.0+R; 
					double sig2 = (sigx_avg+sigy_avg)/2.0-R; 
					double alfa = atan2(tau_avg,ds)/2.0;

					double misesStress = sqrt(	pow(sig1,2) - sig1*sig2 + pow(sig2,2) );

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
									
					values[0] = sig1;
					values[1] = sig2;
					values[2] = alfa;
					
					m_femGrid->setResult(i, j, 0, values);
					m_femGrid->setResult(i, j, 1, values);
					m_femGrid->setResult(i, j, 0, 3, misesStress);
					m_femGrid->setResult(i, j, 1, 3, misesStress);
				}
			}
		}
	}

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
			fp::Constraint* constraint = *vci;
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
			fp::Constraint* constraint = *vci;

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

void FemGridSolver::setElementTreshold(double treshold)
{
	m_elementTreshold = treshold;
}

double FemGridSolver::getMaxPosStressValue()
{
	return m_maxPosStressValue;
}

double FemGridSolver::getMaxNegStressValue()
{
	return m_maxNegStressValue;
}

int FemGridSolver::getDofs()
{
	return m_femGrid->enumerateDofs(ED_BOTTOM_TOP);
}

void FemGridSolver::setWeight(double weight)
{
	m_weight = weight;
}

void FemGridSolver::setForceMagnitude(double value)
{
	m_forceMagnitude = value;
}

void FemGridSolver::setUseWeight(bool flag)
{
	m_useWeight = flag;
}

void FemGridSolver::setOutputMatlab(bool flag)
{
	m_outputMatlab = flag;
}

void FemGridSolver::setMatlabFilename(const char *name)
{
	m_matlabFilename = name;
}

void FemGridSolver::setStiffnessScalefactor(const double scalefactor)
{
	m_stiffnessScalefactor = scalefactor;
}

double FemGridSolver::getStiffnessScalefactor()
{
	return m_stiffnessScalefactor;
}

void FemGridSolver::setElasticModulus(double elasticModulus)
{
	m_elasticModulus = elasticModulus;
}

void FemGridSolver::setThickness(double thickness)
{
	m_thickness = thickness;
}

void FemGridSolver::setYoungsModulus(double youngsModulus)
{
	m_youngsModulus = youngsModulus;
}

void FemGridSolver::setConstraintStiffnessScale(double scale)
{
	m_constraintStiffnessScale = scale;
}

double FemGridSolver::getConstraintStiffnessScale()
{
	return m_constraintStiffnessScale;
}

void FemGridSolver::setStatusMessageEvent(GSStatusMessageEvent* eventMethod)
{
	m_statusMessageEvent = eventMethod;
}

void FemGridSolver::setLogMessageEvent(GSLogMessageEvent* eventMethod)
{
	m_logMessageEvent = eventMethod;
}



