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

#include "Base.h"

// ------------------------------------------------------------
CBase::CBase ()
{
	m_ref = 0;
	m_parent = NULL;
}

// ------------------------------------------------------------
CBase::~CBase ()
{
}

// ------------------------------------------------------------
void CBase::addReference()
{
	m_ref++;
}

// ------------------------------------------------------------
void CBase::delReference()
{
	if (m_ref>0)
		m_ref--;
}

// ------------------------------------------------------------
bool CBase::isReferenced()
{
	return m_ref>0;
}

// ------------------------------------------------------------
int CBase::getReferenceCount()
{
	return m_ref;
}

// ------------------------------------------------------------
CBase* CBase::getParent()
{
	return m_parent;
}

// ------------------------------------------------------------
void CBase::setParent(CBase *parent)
{
	m_parent = parent;
}

// ------------------------------------------------------------
void CBase::saveToStream(ostream &out)
{

}

// ------------------------------------------------------------
void CBase::readFromStream(istream &in)
{

}

