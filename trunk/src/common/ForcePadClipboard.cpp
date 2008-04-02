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

#include "ForcePadClipboard.h"

CForcePadClipboard::CForcePadClipboard()
{
	m_grid = NULL;

	m_forceSelection = new CForceSelection();
	m_constraintSelection = new CConstraintSelection();

	m_drawingOffsetX = 0;
	m_drawingOffsetY = 0;
}

CForcePadClipboard::~CForcePadClipboard()
{
	delete m_forceSelection;
	delete m_constraintSelection;
}

void CForcePadClipboard::copy(int x1, int y1, int x2, int y2)
{
	CClipboard::copy(x1, y1, x2, y2);

	if (m_grid!=NULL)
	{
		int i; 

		// Clear forces on clipboard

		for (i=0; i<m_forceSelection->getSize(); i++)
		{
			CForce* force = m_forceSelection->getForce(i);
			force->delReference();
			
			if (!force->isReferenced())
				delete force;
		}

		m_forceSelection->clear();

		// Clear constraints on clipboard

		for (i=0; i<m_constraintSelection->getSize(); i++)
		{
			CConstraint* constraint = m_constraintSelection->getConstraint(i);
			constraint->delReference();

			if (!constraint->isReferenced())
				delete constraint;
		}

		m_constraintSelection->clear();

		// Find existing forces in selection area
		
		m_grid->getForces(x1, y1, x2, y2, m_forceSelection);
		m_grid->getConstraints(x1, y1, x2, y2, m_constraintSelection);

		// Move forces to clipboard

		for (i=0; i<m_forceSelection->getSize(); i++)
		{
			// Add force to selection

			CForce* force = m_forceSelection->getForce(i);
			force->addReference();
		}

		// Move constraints to clipboard

		for (i=0; i<m_constraintSelection->getSize(); i++)
		{
			// Add constraint to selection

			CConstraint* constraint = m_constraintSelection->getConstraint(i);
			constraint->addReference();
		}
	}
}

void CForcePadClipboard::cut(int x1, int y1, int x2, int y2)
{
	CClipboard::cut(x1, y1, x2, y2);

	if (m_grid!=NULL)
	{
		int i; 

		// Clear forces on clipboard

		for (i=0; i<m_forceSelection->getSize(); i++)
		{
			CForce* force = m_forceSelection->getForce(i);
			force->delReference();
			
			if (!force->isReferenced())
				delete force;
		}

		m_forceSelection->clear();

		// Clear constraints on clipboard

		for (i=0; i<m_constraintSelection->getSize(); i++)
		{
			CConstraint* constraint = m_constraintSelection->getConstraint(i);
			constraint->delReference();

			if (!constraint->isReferenced())
				delete constraint;
		}

		// Find existing forces in selection area
		
		m_grid->getForces(x1, y1, x2, y2, m_forceSelection);
		m_grid->getConstraints(x1, y1, x2, y2, m_constraintSelection);

		// Move forces to clipboard

		for (i=0; i<m_forceSelection->getSize(); i++)
		{
			// Add force to selection

			CForce* force = m_forceSelection->getForce(i);
			force->addReference();

			// Remove force from grid

			m_grid->removePointForce(force);
		}

		// Move constraints to clipboard

		for (i=0; i<m_constraintSelection->getSize(); i++)
		{
			// Add constraint to selection

			CConstraint* constraint = m_constraintSelection->getConstraint(i);
			constraint->addReference();

			// Remove constraint from grid
		
			m_grid->removePointConstraint(constraint);
		}
	}
}

void CForcePadClipboard::paste(int x, int y)
{
	CClipboard::paste(x, y);

	if (m_grid!=NULL)
	{
		int i;

		for (i=0; i<m_forceSelection->getSize(); i++)
		{
			// Add force to to grid

			double dx, dy;
			int x1, y1, x2, y2;

			CForce* force = m_forceSelection->getForce(i);
			force->getPosition(dx, dy);

			this->getSelection(x1, y1, x2, y2);

			dx = dx - (double)x1;
			dy = dy - (double)y1;

			CForce* newForce = new CForce();
			newForce->assignFrom(force);
			newForce->setPosition(dx + (double)x, dy + (double)y);

			m_grid->addForce(newForce);
		}

		// Move constraints to clipboard

		for (i=0; i<m_constraintSelection->getSize(); i++)
		{
			// Add constraint to to grid

			double dx, dy;
			int x1, y1, x2, y2;

			CConstraint* constraint = m_constraintSelection->getConstraint(i);
			constraint->getPosition(dx, dy);

			this->getSelection(x1, y1, x2, y2);

			dx = dx - (double)x1;
			dy = dy - (double)y1;

			CConstraint* newConstraint = new CConstraint();
			newConstraint->assignFrom(constraint);
			newConstraint->setPosition(dx + (double)x, dy + (double)y);

			m_grid->addConstraint(newConstraint);
		}
	}
}

void CForcePadClipboard::setFemGrid(CFemGrid2 *grid)
{
	m_grid = grid;
}

void CForcePadClipboard::setDrawingOffset(int x, int y)
{
	m_drawingOffsetX = x;
	m_drawingOffsetY = y;
}

void CForcePadClipboard::render(int x, int y)
{
	int i;

	CForce* newForce = new CForce();
	CConstraint* newConstraint = new CConstraint();

	for (i=0; i<m_forceSelection->getSize(); i++)
	{
			double dx, dy;
			int x1, y1, x2, y2;
			CForce* force = m_forceSelection->getForce(i);
			force->getPosition(dx, dy);

			newForce->assignFrom(force);
			this->getSelection(x1, y1, x2, y2);
			dx = dx - (double)x1;
			dy = dy - (double)y1;

			newForce->setPosition(dx + (double)x, dy + (double)y);
			newForce->render();
	}

	for (i=0; i<m_constraintSelection->getSize(); i++)
	{
			double dx, dy;
			int x1, y1, x2, y2;
			CConstraint* constraint = m_constraintSelection->getConstraint(i);
			constraint->getPosition(dx, dy);

			newConstraint->assignFrom(constraint);
			this->getSelection(x1, y1, x2, y2);
			dx = dx - (double)x1;
			dy = dy - (double)y1;

			newConstraint->setPosition(dx + (double)x, dy + (double)y);
			newConstraint->render();
	}

	delete newForce;
	delete newConstraint;




}
