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
#define NOMINMAX
#include <windows.h>
#endif

#include "FemGridSolver2.h"
#include "matlabgen.h"

#ifndef USE_QT
#include "LogWindow.h"
#include <FL/Fl.h>
#else
#include <QCoreApplication>
#define so_print(a, b)  ((void)0)
#define so_show()       ((void)0)
#define so_println()    ((void)0)
#define so_hide()       ((void)0)
#endif

#include <set>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <climits>
#include <ctime>

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
	m_nDof = 0;

	// Optimisation parameters

	m_optPenalty = 3.0;
	m_optVolfrac = 0.5;
	m_optMinChange = 0.01;
	m_optMaxLoops = 100;
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
#ifndef USE_QT
	Fl::check();
	Fl::flush();
#else
	QCoreApplication::processEvents();
#endif
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
	// Deprecated: use execute() instead.
}

void CFemGridSolver2::setupHinges()
{
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
	int rows, cols;
	int bwLeftRight, bwBottomTop;
	int maxBandwidth;

	m_femGrid->getGridSize(rows, cols);

	m_femGrid->resetDofs();
	m_nDof = m_femGrid->enumerateDofs(ED_LEFT_RIGHT);
	bwLeftRight = m_femGrid->getBandwidth()-1;

	m_femGrid->resetDofs();
	m_nDof = m_femGrid->enumerateDofs(ED_BOTTOM_TOP);
	bwBottomTop = m_femGrid->getBandwidth()-1;

	if (bwLeftRight<bwBottomTop)
	{
		m_femGrid->resetDofs();
		m_nDof = m_femGrid->enumerateDofs(ED_LEFT_RIGHT);
		maxBandwidth = bwLeftRight;
	}
	else
		maxBandwidth = bwBottomTop;

	return maxBandwidth;
}

int CFemGridSolver2::assembleSystem(calfem::TripletList& Ktriplets)
{
	int i, j, l;
	int rows, cols;
	int topo[8];
	int ptype = m_ptype;
	int ir = 2;
	int nElements = 0;

	double elementValue;
	double ex[4];
	double ey[4];
	double E = m_elasticModulus;
	double v = m_youngsModulus;
	double t = m_thickness;

	calfem::RowVec Ep(3);
	calfem::RowVec Ex(4);
	calfem::RowVec Ey(4);
	calfem::IntRowVec DofTopo(8);
	calfem::ColVec fe(8);
	calfem::Matrix Ke(8,8);
	calfem::Matrix D;

	m_Eq = calfem::RowVec::Zero(2);
	if (m_useWeight)
		m_Eq(1) = -1.0;

	Ep << ptype, t, ir;
	m_f = calfem::ColVec::Zero(m_nDof);

	m_femGrid->getGridSize(rows, cols);

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
				Ex << ex[0], ex[1], ex[2], ex[3];
				Ey << ey[0], ey[1], ey[2], ey[3];

				if (Ex.maxCoeff()>m_maxX) m_maxX = Ex.maxCoeff();
				if (Ex.minCoeff()<m_minX) m_minX = Ex.minCoeff();
				if (Ey.maxCoeff()>m_maxY) m_maxY = Ey.maxCoeff();
				if (Ey.minCoeff()<m_minY) m_minY = Ey.minCoeff();

				calfem::hooke(ptype, E*(double)elementValue*m_stiffnessScalefactor, v, D);

				for (l=0; l<8; l++)
					DofTopo(l) = topo[l];

				calfem::plani4e(Ex, Ey, Ep, D, m_Eq, Ke, fe);
				calfem::spassem(DofTopo, Ktriplets, Ke, m_f, fe);
				nElements++;
			}
		}
	}

	return nElements;
}

int CFemGridSolver2::assembleSystemOpt(calfem::TripletList& Ktriplets, calfem::Matrix& X, calfem::Matrix& L, double penalty)
{
	int i, j, l;
	int rows, cols;
	int topo[8];
	int ptype = m_ptype;
	int ir = 2;
	int nElements = 0;

	double elementValue;
	double ex[4];
	double ey[4];
	double E = m_elasticModulus;
	double v = m_youngsModulus;
	double t = m_thickness;

	calfem::RowVec Ep(3);
	calfem::RowVec Ex(4);
	calfem::RowVec Ey(4);
	calfem::IntRowVec DofTopo(8);
	calfem::ColVec fe(8);
	calfem::Matrix Ke(8,8);
	calfem::Matrix D;

	m_Eq = calfem::RowVec::Zero(2);
	if (m_useWeight)
		m_Eq(1) = -1.0;

	Ep << ptype, t, ir;
	m_f = calfem::ColVec::Zero(m_nDof);

	m_femGrid->getGridSize(rows, cols);

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
				Ex << ex[0], ex[1], ex[2], ex[3];
				Ey << ey[0], ey[1], ey[2], ey[3];

				if (Ex.maxCoeff()>m_maxX) m_maxX = Ex.maxCoeff();
				if (Ex.minCoeff()<m_minX) m_minX = Ex.minCoeff();
				if (Ey.maxCoeff()>m_maxY) m_maxY = Ey.maxCoeff();
				if (Ey.minCoeff()<m_minY) m_minY = Ey.minCoeff();

				calfem::hooke(ptype, E, v, D);

				for (l=0; l<8; l++)
					DofTopo(l) = topo[l];

				Ke.setZero();
				calfem::plani4e(Ex, Ey, Ep, D, m_Eq, Ke, fe);

				if (L(i,j)<1.0)
					Ke = Ke * pow(X(i,j), penalty);

				calfem::spassem(DofTopo, Ktriplets, Ke, m_f, fe);
				nElements++;
			}
		}
	}

	return nElements;
}

void CFemGridSolver2::setupForcesAndConstraints(bool& loadsDefined, bool& bcsDefined, bool& vectorBcsDefined,
	std::set<int>& uniqueDofs, std::set<int>& uniqueVectorDofs,
	std::vector<CConstraint*>& vectorConstraints, std::vector<double>& prescribedValues)
{
	int i;
	int dofs[2];
	double x, y;
	double vx, vy;
	double value;

	loadsDefined = false;

	if (m_useWeight)
	{
		double Fsum = 0.0;
		for (i=1; i<m_nDof; i+=2)
			Fsum += m_f(i);
		for (i=1; i<m_nDof; i+=2)
			m_f(i) = -m_weight*m_f(i)/Fsum;
	}

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
			m_f(dofs[0]-1) += vx*value*m_forceMagnitude;
			m_f(dofs[1]-1) += vy*value*m_forceMagnitude;
			loadsDefined = true;
		}

		pointLoad = m_femGrid->getNextPointLoad();
	}

	if ((m_Eq(0)!=0.0)||(m_Eq(1)!=0.0))
		loadsDefined = true;

	progressMessage("Setting up boundary conditions.", 40);

	CConstraint* pointConstraint = m_femGrid->getFirstPointConstraint();

	prescribedValues.assign(m_nDof, 0.0);

	bcsDefined = false;
	vectorBcsDefined = false;

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
				prescribedValues[dofs[0]-1] = 0.0;
				prescribedValues[dofs[1]-1] = 0.0;
				bcsDefined = true;
				break;
			case CConstraint::CT_X:
				uniqueDofs.insert(dofs[0]);
				prescribedValues[dofs[0]-1] = 0.0;
				bcsDefined = true;
				break;
			case CConstraint::CT_Y:
				uniqueDofs.insert(dofs[1]);
				prescribedValues[dofs[1]-1] = 0.0;
				bcsDefined = true;
				break;
			case CConstraint::CT_VECTOR:
				uniqueVectorDofs.insert(dofs[0]);
				uniqueVectorDofs.insert(dofs[1]);
				{
					auto si = uniqueVectorDofs.find(dofs[0]);
					if (si != uniqueVectorDofs.end())
						vectorConstraints.push_back(pointConstraint);
				}
				vectorBcsDefined = true;
				break;
			default:
				break;
			}
		}

		pointConstraint = m_femGrid->getNextPointConstraint();
	}
}

void CFemGridSolver2::assembleVectorConstraints(calfem::TripletList& Ktriplets, std::vector<CConstraint*>& vectorConstraints)
{
	int dofs[2];
	double x1, y1, x2, y2, ex, ey;
	double E = m_elasticModulus;
	calfem::RowVec BarEx(2);
	calfem::RowVec BarEy(2);
	calfem::RowVec BarEp(2);
	calfem::IntRowVec BarDofTopo(4);
	calfem::Matrix BarKe(4,4);
	calfem::ColVec zero_fe = calfem::ColVec::Zero(4);

	BarEp << E, m_constraintStiffnessScale;

	for (auto vci=vectorConstraints.begin(); vci!=vectorConstraints.end(); vci++)
	{
		CConstraint* constraint = *vci;
		constraint->getPosition(x2, y2);
		m_femGrid->getNearestDofs((int)x2, (int)y2, dofs);

		constraint->getDirection(ex, ey);

		x1 = x2 - ex;
		y1 = y2 - ey;

		BarEx << x1, x2;
		BarEy << y1, y2;
		BarDofTopo << -1, -1, dofs[0], dofs[1];

		calfem::bar2e(BarEx, BarEy, BarEp, BarKe);
		calfem::spassem(BarDofTopo, Ktriplets, BarKe, m_f, zero_fe);
	}
}

void CFemGridSolver2::removeDoubleDofs(std::set<int>& uniqueDofs, std::vector<double>& prescribedValues,
	calfem::IntColVec& bcDofs, calfem::ColVec& bcVals)
{
	int n = (int)uniqueDofs.size();
	bcDofs.resize(n);
	bcVals.resize(n);
	int k = 0;

	for (auto dof : uniqueDofs)
	{
		bcDofs(k) = dof;                    // 1-based DOF number
		bcVals(k) = prescribedValues[dof-1]; // 0-based vector access
		k++;
	}

	uniqueDofs.clear();
}

void CFemGridSolver2::computeElementForces()
{
	int i, j, l;
	int rows, cols;
	double elementValue;
	double ex[4];
	double ey[4];
	int topo[8];
	int ptype = m_ptype;
	int ir = 2;

	double E = m_elasticModulus;
	double v = m_youngsModulus;
	double t = m_thickness;

	calfem::RowVec Ep(3);
	calfem::RowVec Ex(4);
	calfem::RowVec Ey(4);
	calfem::IntRowVec DofTopo(8);
	calfem::RowVec Ed(8);
	calfem::Matrix Es;
	calfem::Matrix Et;
	calfem::Matrix D;

	Ep << ptype, t, ir;

	m_femGrid->getGridSize(rows, cols);

	Ed.setZero();
	m_maxStressValue = -1.0e300;
	m_maxMisesStressValue = -1.0e300;
	m_maxPosStressValue = -1.0e300;
	m_maxNegStressValue = -1.0e300;

	m_femGrid->zeroNodeResults();

	for (i=0; i<rows; i++)
	{
		for (j=0; j<cols; j++)
		{
			double sigx, sigy, tau;

			if (m_femGrid->getElement(i, j, elementValue, ex, ey, topo))
			{
				Ex << ex[0], ex[1], ex[2], ex[3];
				Ey << ey[0], ey[1], ey[2], ey[3];

				calfem::hooke(ptype, E*(double)elementValue*m_stiffnessScalefactor, v, D);

				for (l=0; l<8; l++)
					DofTopo(l) = topo[l];

				for (l=0; l<8; l++)
					Ed(l) = m_a(DofTopo(l)-1);

				calfem::plani4s(Ex, Ey, Ep, D, Ed, Es, Et);

				int ip;
				double ipStress[4];

				for (ip=0; ip<4; ip++)
				{
					sigx = Es(ip,0);
					sigy = Es(ip,1);
					tau  = Es(ip,2);

					double ds = (sigx-sigy)/2.0;
					double R = sqrt(pow(ds,2)+pow(tau,2));

					double sig1 = (sigx+sigy)/2.0+R;
					double sig2 = (sigx+sigy)/2.0-R;

					ipStress[ip] = sqrt( pow(sig1,2) - sig1*sig2 + pow(sig2,2) );
				}

				m_femGrid->addNodeResult(i,   j,   ipStress[0]);
				m_femGrid->addNodeResult(i,   j+1, ipStress[1]);
				m_femGrid->addNodeResult(i+1, j+1, ipStress[2]);
				m_femGrid->addNodeResult(i,   j+1, ipStress[3]);

				calfem::RowVec Es_avg = Es.colwise().sum() / 4.0;

				sigx = Es_avg(0);
				sigy = Es_avg(1);
				tau  = Es_avg(2);

				double ds = (sigx-sigy)/2.0;
				double R = sqrt(pow(ds,2)+pow(tau,2));

				double sig1 = (sigx+sigy)/2.0+R;
				double sig2 = (sigx+sigy)/2.0-R;
				double alfa = atan2(tau,ds)/2.0;

				double misesStress = sqrt( pow(sig1,2) - sig1*sig2 + pow(sig2,2) );

				m_femGrid->setResult(i, j, 0, sig1);
				m_femGrid->setResult(i, j, 1, sig2);
				m_femGrid->setResult(i, j, 2, alfa);
				m_femGrid->setResult(i, j, 3, misesStress);

				if (misesStress>m_maxMisesStressValue) m_maxMisesStressValue = misesStress;
				if (fabs(sig1)>m_maxStressValue) m_maxStressValue = fabs(sig1);
				if (fabs(sig2)>m_maxStressValue) m_maxStressValue = fabs(sig2);
				if (sig1<0.0 && fabs(sig1)>m_maxNegStressValue) m_maxNegStressValue = fabs(sig1);
				if (sig1>0.0 && fabs(sig1)>m_maxPosStressValue) m_maxPosStressValue = sig1;
				if (sig2<0.0 && fabs(sig2)>m_maxNegStressValue) m_maxNegStressValue = fabs(sig2);
				if (sig2>0.0 && fabs(sig2)>m_maxPosStressValue) m_maxPosStressValue = sig2;
			}
		}
	}

	m_femGrid->averageNodeResults();

	cout << "Max misses stress = " << m_maxMisesStressValue << endl;
}

void CFemGridSolver2::computeElementForcesOpt(calfem::Matrix& X, double penalty)
{
	int i, j, l;
	int rows, cols;
	double elementValue;
	double ex[4];
	double ey[4];
	int topo[8];
	int ptype = m_ptype;
	int ir = 2;

	double E = m_elasticModulus;
	double v = m_youngsModulus;
	double t = m_thickness;

	calfem::RowVec Ep(3);
	calfem::RowVec Ex(4);
	calfem::RowVec Ey(4);
	calfem::IntRowVec DofTopo(8);
	calfem::RowVec Ed(8);
	calfem::Matrix Es;
	calfem::Matrix Et;
	calfem::Matrix D;

	Ep << ptype, t, ir;

	m_femGrid->getGridSize(rows, cols);

	Ed.setZero();
	m_maxStressValue = -1.0e300;
	m_maxMisesStressValue = -1.0e300;
	m_maxPosStressValue = -1.0e300;
	m_maxNegStressValue = -1.0e300;

	m_femGrid->zeroNodeResults();

	for (i=0; i<rows; i++)
	{
		for (j=0; j<cols; j++)
		{
			double sigx, sigy, tau;

			if (m_femGrid->getElement(i, j, elementValue, ex, ey, topo))
			{
				Ex << ex[0], ex[1], ex[2], ex[3];
				Ey << ey[0], ey[1], ey[2], ey[3];

				calfem::hooke(ptype, E*pow(X(i,j),penalty), v, D);

				for (l=0; l<8; l++)
					DofTopo(l) = topo[l];

				for (l=0; l<8; l++)
					Ed(l) = m_a(DofTopo(l)-1);

				calfem::plani4s(Ex, Ey, Ep, D, Ed, Es, Et);

				int ip;
				double ipStress[4];

				for (ip=0; ip<4; ip++)
				{
					sigx = Es(ip,0);
					sigy = Es(ip,1);
					tau  = Es(ip,2);

					double ds = (sigx-sigy)/2.0;
					double R = sqrt(pow(ds,2)+pow(tau,2));

					double sig1 = (sigx+sigy)/2.0+R;
					double sig2 = (sigx+sigy)/2.0-R;

					ipStress[ip] = sqrt( pow(sig1,2) - sig1*sig2 + pow(sig2,2) );
				}

				m_femGrid->addNodeResult(i,   j,   ipStress[0]);
				m_femGrid->addNodeResult(i,   j+1, ipStress[1]);
				m_femGrid->addNodeResult(i+1, j+1, ipStress[2]);
				m_femGrid->addNodeResult(i,   j+1, ipStress[3]);

				calfem::RowVec Es_avg = Es.colwise().sum() / 4.0;

				sigx = Es_avg(0);
				sigy = Es_avg(1);
				tau  = Es_avg(2);

				double ds = (sigx-sigy)/2.0;
				double R = sqrt(pow(ds,2)+pow(tau,2));

				double sig1 = (sigx+sigy)/2.0+R;
				double sig2 = (sigx+sigy)/2.0-R;
				double alfa = atan2(tau,ds)/2.0;

				double misesStress = sqrt( pow(sig1,2) - sig1*sig2 + pow(sig2,2) );

				m_femGrid->setResult(i, j, 0, sig1);
				m_femGrid->setResult(i, j, 1, sig2);
				m_femGrid->setResult(i, j, 2, alfa);
				m_femGrid->setResult(i, j, 3, misesStress);

				if (misesStress>m_maxMisesStressValue) m_maxMisesStressValue = misesStress;
				if (fabs(sig1)>m_maxStressValue) m_maxStressValue = fabs(sig1);
				if (fabs(sig2)>m_maxStressValue) m_maxStressValue = fabs(sig2);
				if (sig1<0.0 && fabs(sig1)>m_maxNegStressValue) m_maxNegStressValue = fabs(sig1);
				if (sig1>0.0 && fabs(sig1)>m_maxPosStressValue) m_maxPosStressValue = sig1;
				if (sig2<0.0 && fabs(sig2)>m_maxNegStressValue) m_maxNegStressValue = fabs(sig2);
				if (sig2>0.0 && fabs(sig2)>m_maxPosStressValue) m_maxPosStressValue = sig2;
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
	calfem::RowVec BarEx(2);
	calfem::RowVec BarEy(2);
	calfem::RowVec BarEp(2);
	calfem::RowVec BarEd(4);

	double maxReactionForce = -1e300;
	double E = m_elasticModulus;

	BarEp << E, m_constraintStiffnessScale;

	for (auto vci=vectorConstraints.begin(); vci!=vectorConstraints.end(); vci++)
	{
		CConstraint* constraint = *vci;
		constraint->getPosition(x2, y2);
		m_femGrid->getNearestDofs((int)x2, (int)y2, dofs);

		constraint->getDirection(ex, ey);

		x1 = x2 - ex;
		y1 = y2 - ey;

		BarEx << x1, x2;
		BarEy << y1, y2;
		BarEd << 0.0, 0.0, m_a(dofs[0]-1), m_a(dofs[1]-1);

		calfem::bar2s(BarEx, BarEy, BarEp, BarEd, es);

		if (fabs(es)>maxReactionForce)
			maxReactionForce = fabs(es);

		constraint->getReactionForce()->setValue(es);
	}

	for (auto vci=vectorConstraints.begin(); vci!=vectorConstraints.end(); vci++)
	{
		CConstraint* constraint = *vci;

		double force = 100.0 * constraint->getReactionForce()->getValue()/maxReactionForce;
		constraint->getReactionForce()->setLength(-force);
	}
}

void CFemGridSolver2::objectiveFunctionAndSensitivity(calfem::Matrix& X, calfem::Matrix& dC, calfem::Matrix& L, double penalty, double& c)
{
	int i, j, l;
	int rows, cols;
	double elementValue;
	double ex[4];
	double ey[4];
	int topo[8];
	int ptype = m_ptype;
	int ir = 2;

	double E = m_elasticModulus;
	double v = m_youngsModulus;
	double t = m_thickness;

	calfem::RowVec Ep(3);
	calfem::RowVec Ex(4);
	calfem::RowVec Ey(4);
	calfem::IntRowVec DofTopo(8);
	calfem::RowVec Ed(8);
	calfem::Matrix Es;
	calfem::Matrix Et;
	calfem::Matrix D;
	calfem::ColVec fe(8);
	calfem::Matrix Ke(8,8);

	Ep << ptype, t, ir;

	m_femGrid->getGridSize(rows, cols);

	Ed.setZero();

	c = 0.0;
	dC.setZero();

	for (i=0; i<rows; i++)
	{
		for (j=0; j<cols; j++)
		{
			if (m_femGrid->getElement(i, j, elementValue, ex, ey, topo))
			{
				Ex << ex[0], ex[1], ex[2], ex[3];
				Ey << ey[0], ey[1], ey[2], ey[3];

				calfem::hooke(ptype, E, v, D);

				for (l=0; l<8; l++)
					DofTopo(l) = topo[l];

				for (l=0; l<8; l++)
					Ed(l) = m_a(DofTopo(l)-1);

				Ke.setZero();
				calfem::plani4e(Ex, Ey, Ep, D, m_Eq, Ke, fe);
				Ke = Ke * pow(X(i,j), penalty);

				if (L(i,j)<0.2)
				{
					double W = (Ed * Ke * Ed.transpose()).value();
					c += pow(X(i,j), penalty)*W;
					dC(i,j) = -penalty*pow(X(i,j), penalty-1)*W;
				}
				else
				{
					dC(i,j) = -1e50;
				}
			}
		}
	}
}

calfem::Matrix CFemGridSolver2::optimalityCriteriaUpdate(calfem::Matrix& X, calfem::Matrix& dC, calfem::Matrix& L, double volfrac, int nElements)
{
	double lmid;
	double l1 = 0.0;
	double l2 = 100000;
	double move = 0.2;

	calfem::Matrix Xnew;

	while (l2-l1 > 1e-4)
	{
		lmid = 0.5*(l2+l1);
		calfem::Matrix X1 = (X.array() - move).matrix();
		calfem::Matrix X2 = (X.array() + move).matrix();
		calfem::Matrix X3 = (-dC.array() / lmid).matrix();
		calfem::Matrix X4 = X.cwiseProduct(X3.array().sqrt().matrix());
		Xnew = (X1.cwiseMax(X2.cwiseMin(X4).cwiseMin(1.0))).cwiseMax(0.001);
		m_femGrid->assignNonElements(Xnew, 0.0);
		if (Xnew.sum() - volfrac * nElements > 0.0)
			l1 = lmid;
		else
			l2 = lmid;
	}

	return Xnew;
}

int CFemGridSolver2::scaleToAbsoluteSize(int r)
{
	auto grid_stride = m_femGrid->getStride();
	r = r * 6 / grid_stride;
	return r;
}

calfem::Matrix CFemGridSolver2::sensitivityFilter1(calfem::Matrix& X, calfem::Matrix& dC, double rmin)
{
	int i, j, k, l;
	int rows = (int)X.rows();
	int cols = (int)X.cols();
	double sum, fac;

	calfem::Matrix dCnew = calfem::Matrix::Zero(rows, cols);

	if (m_rminInPixels)
		rmin = rmin / (double)m_femGrid->getStride();

	for (i=0; i<cols; i++)
	{
		for (j=0; j<rows; j++)
		{
			sum = 0.0;

			int kmin = std::max(0, (int)(i - std::floor(rmin)));
			int kmax = std::min(cols-1, (int)(i + std::floor(rmin)));
			int lmin = std::max(0, (int)(j - std::floor(rmin)));
			int lmax = std::min(rows-1, (int)(j + std::floor(rmin)));

			for (k=kmin; k<=kmax; k++)
				for (l=lmin; l<=lmax; l++)
				{
					fac = rmin - sqrt(pow((double)(i-k),2.0)+pow((double)(j-l),2.0));
					sum += std::max(0.0,fac);
					dCnew(j,i) += std::max(0.0,fac)*X(l,k)*dC(l,k);
				}
			dCnew(j,i) = dCnew(j,i)/(X(j,i)*sum);
		}
	}

	return dCnew;
}

calfem::Matrix CFemGridSolver2::sensitivityFilter2(calfem::Matrix& dC, double rmin)
{
	int i, j, k, l;
	int rows = (int)dC.rows();
	int cols = (int)dC.cols();
	double sum, fac;

	calfem::Matrix dCnew = calfem::Matrix::Zero(rows, cols);

	if (m_rminInPixels)
		rmin = rmin / (double)m_femGrid->getStride();

	for (i=0; i<cols; i++)
	{
		for (j=0; j<rows; j++)
		{
			sum = 0.0;

			int kmin = std::max(0, (int)(i - std::floor(rmin)));
			int kmax = std::min(cols-1, (int)(i + std::floor(rmin)));
			int lmin = std::max(0, (int)(j - std::floor(rmin)));
			int lmax = std::min(rows-1, (int)(j + std::floor(rmin)));

			for (k=kmin; k<=kmax; k++)
				for (l=lmin; l<=lmax; l++)
				{
					fac = rmin - sqrt(pow((double)(i-k),2.0)+pow((double)(j-l),2.0));
					sum += std::max(0.0,fac);
					dCnew(j,i) += std::max(0.0,fac)*dC(l,k);
				}
			dCnew(j,i) = dCnew(j,i)/sum;
		}
	}

	return dCnew;
}

void CFemGridSolver2::executeOptimizer()
{
	int rows, cols;
	bool loadsDefined, bcsDefined, vectorBcsDefined;
	std::set<int> uniqueDofs;
	std::set<int> uniqueVectorDofs;
	std::vector<CConstraint*> vectorConstraints;
	std::vector<double> prescribedValues;

#ifdef WIN32
	DWORD startTime = timeGetTime();
#endif

	int nElements;

	m_errorStatus = CFemGridSolver2::ET_NO_ERROR;

	setupHinges();

	calculateOptimalBandwidth();

	if (m_nDof==0)
	{
		m_errorStatus = ET_NO_ELEMENTS;
		return;
	}

	calfem::Matrix X;
	calfem::Matrix Xold;
	calfem::Matrix dC;
	calfem::Matrix L;

	double change = 1.0;
	double penalty = m_optPenalty;
	double volfrac = m_optVolfrac;
	double c = 0.0;
	int loop = 0;

	m_femGrid->getGridSize(rows, cols);
	X  = calfem::Matrix::Zero(rows, cols);
	Xold = calfem::Matrix::Zero(rows, cols);
	dC = calfem::Matrix::Zero(rows, cols);
	L  = calfem::Matrix::Ones(rows, cols);

	cout << "Sum L = " << L.sum() << endl;

	m_femGrid->copyGrid(X, volfrac);
	m_femGrid->assignFieldFromImage(1, 1);
	m_femGrid->copyField(1, L);

	L = (L.array() > 0.0).select(1.0, L);

	while ((change>m_optMinChange)&&(loop<m_optMaxLoops)&&(this->continueCalc()))
	{
		loop++;
		Xold = X;

		this->progressMessage("Assembling system matrix.", 20);

		calfem::TripletList Ktriplets;

		clock_t t0 = clock();
		nElements = this->assembleSystemOpt(Ktriplets, X, L, penalty);
		clock_t t1 = clock();
		std::cout << "Assemble system = " << t1-t0 << std::endl;

		if (nElements==0)
		{
			m_errorStatus = ET_NO_ELEMENTS;
			return;
		}

		if ((m_femGrid->getPointLoadSize()==0)&&(m_Eq(0)==0.0)&&(m_Eq(1)==0.0))
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

		if (vectorBcsDefined)
			this->assembleVectorConstraints(Ktriplets, vectorConstraints);

		calfem::IntColVec bcDofs;
		calfem::ColVec bcVals;
		this->removeDoubleDofs(uniqueDofs, prescribedValues, bcDofs, bcVals);

		m_K_sparse.resize(m_nDof, m_nDof);
		m_K_sparse.setFromTriplets(Ktriplets.begin(), Ktriplets.end());

		m_a = calfem::ColVec::Zero(m_nDof);

		progressMessage("Solving system.", 50);

		t0 = clock();
		calfem::ColVec Q;
		if (!calfem::spsolveq(m_K_sparse, m_f, bcDofs, bcVals, m_a, Q))
		{
			m_errorStatus = ET_INVALID_MODEL;
			return;
		}
		t1 = clock();
		cout << "Equation system = " << t1-t0 << endl;

		t0 = clock();
		c = 0.0;
		dC.setZero();

		cout << "objectiveFunctionAndSensitivity()" << endl;
		this->progressMessage("Objective function and sensitivity.", 60);
		this->objectiveFunctionAndSensitivity(X, dC, L, m_optPenalty, c);
		t1 = clock();
		cout << "Objective function = " << t1-t0 << endl;

		if (!this->continueCalc())
			break;

		cout << "sensitivityFilter1()" << endl;
		this->progressMessage("Applying sensitivity filter.", 70);

		calfem::Matrix dCnew;

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

		cout << "optimalityCriteriaUpdate()" << endl;
		this->progressMessage("Design update.", 100);
		calfem::Matrix Xnew = this->optimalityCriteriaUpdate(X, dC, L, volfrac, nElements);
		calfem::Matrix Diff = Xnew - Xold;
		change = Diff.array().abs().maxCoeff();
		X = Xnew;

		m_femGrid->assignField(0, X);

		cout << "loop = " << loop << ", change = " << change << ", c = " << c << endl;
	}

	m_femGrid->assignGrid(X);

	this->execute();

#ifdef WIN32
	DWORD endTime = timeGetTime();
	cout << "Total execution time (ms) = " << endTime - startTime << endl;
#endif
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
	int ptype = m_ptype;
	int ir = 2;

	calfem::RowVec Ex(4);
	calfem::RowVec Ey(4);
	calfem::IntRowVec DofTopo(8);
	calfem::RowVec Ep(3);
	calfem::Matrix D;

	Ep << ptype, t, ir;

	m_femGrid->getGridSize(rows, cols);

	if (m_femGrid->getPointConstraintsSize()==0)
	{
		m_errorStatus = ET_NO_BCS;
		return;
	}

	// Re-build the BC set to get the same bcDofs/bcVals as execute()
	std::set<int> uniqueDofs;
	std::set<int> uniqueVectorDofs;
	std::vector<CConstraint*> vectorConstraints;
	std::vector<double> prescribedValues;
	double x, y, value;
	int dofs[2];
	bool vectorBcsDefined = false;

	CConstraint* pointConstraint = m_femGrid->getFirstPointConstraint();

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
				break;
			case CConstraint::CT_X:
				uniqueDofs.insert(dofs[0]);
				break;
			case CConstraint::CT_Y:
				uniqueDofs.insert(dofs[1]);
				break;
			case CConstraint::CT_VECTOR:
				uniqueVectorDofs.insert(dofs[0]);
				uniqueVectorDofs.insert(dofs[1]);
				{
					auto si = uniqueVectorDofs.find(dofs[0]);
					if (si != uniqueVectorDofs.end())
						vectorConstraints.push_back(pointConstraint);
				}
				vectorBcsDefined = true;
				break;
			default:
				break;
			}
		}

		pointConstraint = m_femGrid->getNextPointConstraint();
	}

	// Rebuild load vector with current loads
	m_f.setZero();

	if (m_useWeight)
	{
		double Fsum = 0.0;
		for (i=1; i<m_nDof; i+=2)
			Fsum += m_f(i);
		for (i=1; i<m_nDof; i+=2)
			m_f(i) = -m_weight*m_f(i)/Fsum;
	}

	bool loadsDefined = false;
	double vx, vy;

	m_femGrid->clearPoints();

	if ((m_femGrid->getPointLoadSize()==0)&&(m_Eq(0)==0.0)&&(m_Eq(1)==0.0))
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
			m_f(dofs[0]-1) += vx*value*m_forceMagnitude;
			m_f(dofs[1]-1) += vy*value*m_forceMagnitude;
			loadsDefined = true;
		}

		pointLoad = m_femGrid->getNextPointLoad();
	}

	if ((m_Eq(0)!=0.0)||(m_Eq(1)!=0.0))
		loadsDefined = true;

	if (!loadsDefined)
	{
		m_errorStatus = ET_LOAD_OUTSIDE_AE;
		return;
	}

	// Re-solve with updated load vector using stored K
	prescribedValues.assign(m_nDof, 0.0);
	calfem::IntColVec bcDofs;
	calfem::ColVec bcVals;
	this->removeDoubleDofs(uniqueDofs, prescribedValues, bcDofs, bcVals);

	m_a = calfem::ColVec::Zero(m_nDof);

	calfem::ColVec Q;
	if (!calfem::spsolveq(m_K_sparse, m_f, bcDofs, bcVals, m_a, Q))
	{
		m_errorStatus = ET_INVALID_MODEL;
		return;
	}

	m_maxNodeValue = -1.0e300;

	m_femGrid->setDisplacementSize(m_nDof);

	for (i=0; i<m_nDof; i++)
	{
		m_femGrid->setDisplacement(i+1, m_a(i));
		if (fabs(m_a(i))>m_maxNodeValue)
			m_maxNodeValue = fabs(m_a(i));
	}

	this->computeElementForces();

	if (vectorBcsDefined)
		this->computeReactionForces(vectorConstraints);

	m_femGrid->setMaxStressValue(m_maxStressValue);
	m_femGrid->setMaxPosStressValue(m_maxPosStressValue);
	m_femGrid->setMaxNegStressValue(m_maxNegStressValue);
	m_femGrid->setMaxNodeValue(m_maxNodeValue);
	m_femGrid->setMaxMisesStressValue(m_maxMisesStressValue);
}

void CFemGridSolver2::execute()
{
	int i;
	bool loadsDefined, bcsDefined, vectorBcsDefined;
	std::set<int> uniqueDofs;
	std::set<int> uniqueVectorDofs;
	std::vector<CConstraint*> vectorConstraints;
	std::vector<double> prescribedValues;

#ifdef WIN32
	DWORD startTime = timeGetTime();
#endif

	m_errorStatus = CFemGridSolver2::ET_NO_ERROR;

	setupHinges();

	calculateOptimalBandwidth();

	if (m_nDof==0)
	{
		m_errorStatus = ET_NO_ELEMENTS;
		return;
	}

	this->progressMessage("Assembling system matrix.", 20);

	int nElements;

	calfem::TripletList Ktriplets;

	nElements = this->assembleSystem(Ktriplets);

	if (nElements==0)
	{
		m_errorStatus = ET_NO_ELEMENTS;
		return;
	}

	if ((m_femGrid->getPointLoadSize()==0)&&(m_Eq(0)==0.0)&&(m_Eq(1)==0.0))
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

	if (vectorBcsDefined)
		this->assembleVectorConstraints(Ktriplets, vectorConstraints);

	this->removeDoubleDofs(uniqueDofs, prescribedValues, m_bcDofs, m_bcVals);

	m_K_sparse.resize(m_nDof, m_nDof);
	m_K_sparse.setFromTriplets(Ktriplets.begin(), Ktriplets.end());

	m_a = calfem::ColVec::Zero(m_nDof);

	progressMessage("Solving.", 60);

	calfem::ColVec Q;
	if (!calfem::spsolveq(m_K_sparse, m_f, m_bcDofs, m_bcVals, m_a, Q))
	{
		m_errorStatus = ET_INVALID_MODEL;
		return;
	}

	progressMessage("Storing results.", 80);

	m_maxNodeValue = -1.0e300;

	m_femGrid->setDisplacementSize(m_nDof);

	for (i=0; i<m_nDof; i++)
	{
		m_femGrid->setDisplacement(i+1, m_a(i));
		if (fabs(m_a(i))>m_maxNodeValue)
			m_maxNodeValue = fabs(m_a(i));
	}

	this->computeElementForces();

	if (vectorBcsDefined)
		this->computeReactionForces(vectorConstraints);

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
