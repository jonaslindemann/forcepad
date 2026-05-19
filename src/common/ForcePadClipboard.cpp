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

namespace fp {

ForcePadClipboard::ForcePadClipboard()
{
	m_grid = nullptr;

	m_forceSelection = std::make_unique<ForceSelection>();
	m_constraintSelection = std::make_unique<ConstraintSelection>();

	m_drawingOffsetX = 0;
	m_drawingOffsetY = 0;
}

ForcePadClipboard::~ForcePadClipboard()
{
}

void ForcePadClipboard::copy(int x1, int y1, int x2, int y2)
{
	ivf2d::Clipboard::copy(x1, y1, x2, y2);

	if (m_grid!=nullptr)
	{
		m_forceSelection->clear();
		m_constraintSelection->clear();

		m_grid->getForces(x1, y1, x2, y2, m_forceSelection.get());
		m_grid->getConstraints(x1, y1, x2, y2, m_constraintSelection.get());
	}
}

void ForcePadClipboard::cut(int x1, int y1, int x2, int y2)
{
	ivf2d::Clipboard::cut(x1, y1, x2, y2);

	if (m_grid!=nullptr)
	{
		m_forceSelection->clear();
		m_constraintSelection->clear();

		m_grid->getForces(x1, y1, x2, y2, m_forceSelection.get());
		m_grid->getConstraints(x1, y1, x2, y2, m_constraintSelection.get());

		for (int i=0; i<m_forceSelection->getSize(); i++)
			m_grid->removePointForce(m_forceSelection->getForce(i));

		for (int i=0; i<m_constraintSelection->getSize(); i++)
			m_grid->removePointConstraint(m_constraintSelection->getConstraint(i));
	}
}

void ForcePadClipboard::paste(int x, int y)
{
	ivf2d::Clipboard::paste(x, y);

	if (m_grid!=nullptr)
	{
		int x1, y1, x2, y2;
		this->getSelection(x1, y1, x2, y2);

		for (int i=0; i<m_forceSelection->getSize(); i++)
		{
			double dx, dy;
			Force* force = m_forceSelection->getForce(i);
			force->getPosition(dx, dy);
			dx = dx - (double)x1;
			dy = dy - (double)y1;

			auto newForce = Force::create();
			newForce->assignFrom(force);
			newForce->setPosition(dx + (double)x, dy + (double)y);
			m_grid->addForce(newForce);
		}

		for (int i=0; i<m_constraintSelection->getSize(); i++)
		{
			double dx, dy;
			Constraint* constraint = m_constraintSelection->getConstraint(i);
			constraint->getPosition(dx, dy);
			dx = dx - (double)x1;
			dy = dy - (double)y1;

			auto newConstraint = Constraint::create();
			newConstraint->assignFrom(constraint);
			newConstraint->setPosition(dx + (double)x, dy + (double)y);
			m_grid->addConstraint(newConstraint);
		}
	}
}

void ForcePadClipboard::setFemGrid(FemGrid2 *grid)
{
	m_grid = grid;
}

void ForcePadClipboard::setDrawingOffset(int x, int y)
{
	m_drawingOffsetX = x;
	m_drawingOffsetY = y;
}

void ForcePadClipboard::render(int x, int y)
{
	int x1, y1, x2, y2;
	this->getSelection(x1, y1, x2, y2);

	auto newForce = Force::create();
	auto newConstraint = Constraint::create();

	for (int i=0; i<m_forceSelection->getSize(); i++)
	{
		double dx, dy;
		Force* force = m_forceSelection->getForce(i);
		force->getPosition(dx, dy);
		newForce->assignFrom(force);
		dx = dx - (double)x1;
		dy = dy - (double)y1;
		newForce->setPosition(dx + (double)x, dy + (double)y);
		newForce->render();
	}

	for (int i=0; i<m_constraintSelection->getSize(); i++)
	{
		double dx, dy;
		Constraint* constraint = m_constraintSelection->getConstraint(i);
		constraint->getPosition(dx, dy);
		newConstraint->assignFrom(constraint);
		dx = dx - (double)x1;
		dy = dy - (double)y1;
		newConstraint->setPosition(dx + (double)x, dy + (double)y);
		newConstraint->render();
	}
}

} // namespace fp
