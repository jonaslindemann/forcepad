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


// FemGridSolverCORBA.cpp: implementation of the CFemGridSolverCORBA class.
//
//////////////////////////////////////////////////////////////////////

#include "FemGridSolverCORBA.h"

#ifdef USE_CORBA
#include <OB/CORBA.h>
#include <OB/CosNaming.h>
#include <ForcePadSolver.h>
#endif


#include <set>
#include <iostream>
#include <fstream>
using namespace std;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFemGridSolverCORBA::CFemGridSolverCORBA()
{

}

CFemGridSolverCORBA::~CFemGridSolverCORBA()
{

}

void CFemGridSolverCORBA::execute()
{
#ifdef USE_CORBA
	ForcePadSolver::FemSystem_var femSystem;

	try 
	{
		//
		// Counter variables
		//
		
		int i, j, k, l, dofCount;
		
		//////////////////////////////////////////////////////
		// Initialize ORB
		//
		
		cout << "Initializing ORB." << endl;

		CORBA::ORB_var orb = CORBA::ORB_init(m_argc, m_argv);
		
		//
		// Get initial naming context
		//

		cout << "Resolving NameService." << endl;
		
		CORBA::Object_var nameRef;
		try
		{
			nameRef = orb -> resolve_initial_references("NameService");
		}
		catch(const CORBA::ORB::InvalidName&)
		{
			cerr << m_argv[0] << ": can't resolve `NameService'" << endl;
			throw 0;
		}

		//
		// Save reference
		//
		CORBA::String_var s = orb -> object_to_string(nameRef);
		
		const char* refFile = "name.ref";
		ofstream out(refFile);
		if(out.fail())
		{
			cerr << m_argv[0] << ": can't open `" << refFile << "': "
				<< strerror(errno) << endl;
			return;
		}
		
		out << s << endl;
		out.close();
		
		cout << "Narrowing initial naming context." << endl;
		CosNaming::NamingContext_var inc;
		inc = CosNaming::NamingContext::_narrow(nameRef);
		assert(!CORBA::is_nil(inc));
		
		//
		// Get model factory reference
		//
		
		cout << "Getting reference to FemSystemFactory." << endl;

		CORBA::Object_var systemFactoryRef;
		CosNaming::Name name;
		name.length(3);
		name[0].id = CORBA::string_dup("ForcePad");
		name[1].id = CORBA::string_dup("Factories");
		name[2].id = CORBA::string_dup("FemSystemFactory");
		
		try {
			systemFactoryRef = inc->resolve(name);
		} catch (const CosNaming::NamingContext::NotFound &) {
			cerr << "No name for FemSystemFactory factory." << endl;
			throw 0;
		} catch (const CORBA::Exception &e) {
			cerr << "Resolve failed: " << e << endl;
			throw 0;
		}
		
		//////////////////////////////////////////////////////
		// Create a FemSystem object
		//
		
		//
		// Try to narrow ForcePadSolver::FemModel
		//
		
		cout << "Narrowing FemSystemFactory." << endl;

		ForcePadSolver::FemSystemFactory_var femSystemFactory;
		try 
		{
			femSystemFactory = ForcePadSolver::FemSystemFactory::_narrow(systemFactoryRef);
		}
		catch (const CORBA::SystemException& se)
		{
			cerr << "Cannot narrow FemSystemFactory reference: " << se << endl;
			throw 0;
		}
		
		if (CORBA::is_nil(femSystemFactory))
		{
			cerr << "femSystemFactory is nil." << endl;
			throw 0;
		}
		
		//////////////////////////////////////////////////////
		// Get all object references
		//
		
		//
		// Create a FemSystem object
		//
		
		femSystem = femSystemFactory->create();
		
		if (CORBA::is_nil(femSystem))
		{
			cerr << "FemSystem is nil." << endl;
			throw 0;
		}

		//
		// Get a FemGrid
		//

		ForcePadSolver::FemGrid_var femGrid = femSystem->getFemGrid();

		if (CORBA::is_nil(femGrid))
		{
			cerr << "FemGrid is nil." << endl;
			throw 0;
		}

		//
		// Get a FemCalc calculation control object
		//
		
		ForcePadSolver::FemSolver_var femSolver = femSystem->getFemSolver();

		if (CORBA::is_nil(femSolver))
		{
			cerr << "FemCalc is nil." << endl;
			throw 0;
		}
		
		//////////////////////////////////////////////////////
		// Open a model file and execute
		//
		
		//
		// Set gridsize 
		//

		int rows, cols;
		int nDof;
		int bwLeftRight;
		int bwBottomTop;
		int maxBandwidth;

		m_femGrid->getGridSize(rows, cols);
		femGrid->setSize(rows, cols);
		cout << "Gridsize = [ " << rows << " x " << cols << " ]." << endl << endl;
		cout << "Enumerating dofs and calculating bandwidth." << endl << endl;
		
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
		
		cout << "\tTotal dofs = " << nDof << endl;
		cout << "\tBandwidth  = " << maxBandwidth<< endl << endl;

		//
		// Set stiffness
		//

		int nStiffness = rows*cols*2;
		ForcePadSolver::TStiffnessVector stiffnessVector(nStiffness);
		stiffnessVector.length(nStiffness);

		l = 0;		
		float value;

		for (i=0; i<rows; i++)
			for (j=0; j<cols; j++)
				for (k=0; k<2; k++)
				{
					value = m_femGrid->getGridValue(i, j, k);
					stiffnessVector[l++] = (double)value;
				}

		femGrid->setStiffness(stiffnessVector);


		cout << "Defining load vector." << endl << endl;
		
		
		
		//
		// Set forces
		//

		ForcePadSolver::TForceVector forces;
		ForcePadSolver::TForceDofVector forceDofs;

		bool loadsDefined = false;
		double x, y;
		double vx, vy;
		int dofs[2];
		int forceCount = 0;

		int nLoads = m_femGrid->getPointLoadSize();
		
		m_femGrid->clearPoints();
		
		if (nLoads==0)
		{
			m_errorStatus = ET_NO_LOADS;
			throw 0;
		}

		forces.length(nLoads*2);
		forceDofs.length(nLoads*2);
		
		CForce* pointLoad = m_femGrid->getFirstPointLoad();
		
		while (pointLoad!=NULL) 
		{
			pointLoad->getPosition(x, y);
			value = pointLoad->getValue();
			pointLoad->getDirection(vx, vy);
			m_femGrid->getNearestDofs((int)x, (int)y, dofs);
			
			if (dofs[0]>0)
			{
				cout << "\tLoad found at [ " << x << ", " << y << "],  dofs = [ ";
				cout << dofs[0] << ", " << dofs[1] << " ] " << endl;

				forces[forceCount] = vx*value;
				forceDofs[forceCount++] = dofs[0];
				forces[forceCount] = vy*value;
				forceDofs[forceCount++] = dofs[1];
			
				loadsDefined = true;
			}
			
			pointLoad = m_femGrid->getNextPointLoad();
		}
		cout << endl;
		
		if (!loadsDefined)
		{
			m_errorStatus = ET_LOAD_OUTSIDE_AE;
			throw 0;
		}

		femGrid->setForces(forces, forceDofs);
		
		//
		// Define constraints
		//
		
		ForcePadSolver::TBCVector bcs;
		ForcePadSolver::TBCDofVector bcDofs;
		int nBCs = m_femGrid->getPointConstraintsSize();

		cout << "Defining constraints." << endl << endl;
		
		if (nBCs==0)
		{
			m_errorStatus = ET_NO_BCS;
			throw 0;
		}

		int nConstraints = m_femGrid->getPointConstraintsSize();
		CConstraint* pointConstraint = m_femGrid->getFirstPointConstraint();
		
		set<int> uniqueDofs;
		set<int>::iterator si;
		//double* prescribedValues = new double[nDof+1];
		bool bcsDefined = false;
		
		while (pointConstraint!=NULL) 
		{
			pointConstraint->getPosition(x, y);
			value = pointConstraint->getValue();
			m_femGrid->getNearestDofs((int)x, (int)y, dofs);
			
			if (dofs[0]>0)
			{
				cout << "\tConstraint found at [ " << x << ", " << y << "],  dofs = [ ";
				cout << dofs[0] << ", " << dofs[1] << " ] " << endl;
				
				switch (pointConstraint->getConstraintType()) {
				case CConstraint::CT_XY:
					uniqueDofs.insert(dofs[0]);
					uniqueDofs.insert(dofs[1]);
					//prescribedValues[dofs[0]] = 0.0;
					//prescribedValues[dofs[1]] = 0.0;
					bcsDefined = true;
					break;
				case CConstraint::CT_X:
					uniqueDofs.insert(dofs[0]);
					//prescribedValues[dofs[0]] = 0.0;
					bcsDefined = true;
					break;
				case CConstraint::CT_Y:
					uniqueDofs.insert(dofs[1]);
					//prescribedValues[dofs[1]] = 0.0;
					bcsDefined = true;
					break;
				default:
					uniqueDofs.insert(dofs[0]);
					uniqueDofs.insert(dofs[1]);
					//prescribedValues[dofs[0]] = 0.0;
					//prescribedValues[dofs[1]] = 0.0;
					bcsDefined = true;
					break;
				}
			}
			
			pointConstraint = m_femGrid->getNextPointConstraint();
		}
		cout << endl;
		
		if (!bcsDefined)
		{
			m_errorStatus = ET_BC_OUTSIDE_AE;
			throw 0;
		}
		
		// 
		// Remove doubly defined dofs
		//
		
		cout << "\tUnique dofs:" << endl << endl;
		
		int bcCount = 0;

		bcs.length(uniqueDofs.size());
		bcDofs.length(uniqueDofs.size());
		
		for (si=uniqueDofs.begin(); si!=uniqueDofs.end(); si++)
		{
			int dof = (*si);
			cout << "\t" << dof << endl;

			bcs[bcCount] = 0.0;
			bcDofs[bcCount++] = dof;
		}
		
		uniqueDofs.clear();

		femGrid->setBCs(bcs, bcDofs);

		//
		// Solve system
		//

		ForcePadSolver::TErrorType error;

		femSolver->execute();
		error = femSolver->getLastError();

		//
		// Get displacements
		//

		ForcePadSolver::TDisplVector* displacements;

		femGrid->getDisplacements(displacements);

		m_maxNodeValue = -1.0e300;
		
		m_femGrid->setDisplacementSize(displacements->length());
		
		for (i=0; i<displacements->length(); i++)
		{
			m_femGrid->setDisplacement(i+1, (*displacements)[i]);
			//cout << (*displacements)[i] << endl;
			if (fabs((*displacements)[i])>m_maxNodeValue)
				m_maxNodeValue = fabs((*displacements)[i]);
		}

		delete displacements;
		//m_femGrid->setMaxStressValue(m_maxStressValue);
		
		m_femGrid->setMaxNodeValue(m_maxNodeValue);
/*
	for (i=0; i<rows; i++)
		for (j=0; j<cols; j++)
			for (k=0; k<2; k++)
			{
				m_femGrid->getResult(i, j, k, values);
				(*results)[l++] = values[0];
				(*results)[l++] = values[1];
				(*results)[l++] = values[2];
			}
*/
		//
		// Finally destroy all object references
		//

		//////////////////////////////////////////////////////
		// Release FEM system
		//

		femSystem->remove();
	}
	catch (const CORBA::Exception& e) 
	{
		cerr << "Uncaught CORBA exception: " 
			<< e << endl;

		try {
			femSystem->remove();
		}
		catch (const CORBA::Exception& e)
		{
			return;
		}
	}
	return;
#endif
}

void CFemGridSolverCORBA::setFemGrid(CFemGrid *femGrid)
{
	m_femGrid = femGrid;
}

CFemGridSolverCORBA::TErrorType CFemGridSolverCORBA::getLastError()
{
	return ET_NO_ERROR;	
}

void CFemGridSolverCORBA::setCommandLine(int argc, char **argv)
{
	m_argc = argc;
	m_argv = argv;
}
