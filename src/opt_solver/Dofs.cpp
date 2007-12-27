// NOTE: please use a PRESERVE:BEGIN/PRESERVE:END comment block
//       to preserve your hand-coding across code generations.


#include "Dofs.h"

// ------------------------------------------------------------
CDofs::CDofs ()
		:CBase()
{
	this->setSize(3);
}

// ------------------------------------------------------------
CDofs::CDofs (CDofs& dof)
		:CBase()
{
	unsigned int i;

	for (i=0; i<m_dofs.size(); i++)
		m_dofs[i] = dof.getDof(i+1);
}

// ------------------------------------------------------------
CDofs::~CDofs ()
{
	// PRESERVE:BEGIN
	// Insert your preservable code here...
	// PRESERVE:END
}

// ------------------------------------------------------------
CDofs& CDofs::operator = (const CDofs &arg)
{
	// PRESERVE:BEGIN
	// Insert your preservable code here...
	return *this;
	// PRESERVE:END
}

// ------------------------------------------------------------
void CDofs::setSize(int nDofs)
{
	m_dofs.resize(nDofs);
	this->reset();
}

// ------------------------------------------------------------
size_t CDofs::getSize()
{
	return m_dofs.size();
}

// ------------------------------------------------------------
void CDofs::reset()
{
	unsigned int i;

	for (i=0; i<m_dofs.size(); i++)
		m_dofs[i] = -1;
}


// ------------------------------------------------------------
int CDofs::enumerate(int start)
{
	unsigned int i;
	int dofCount = start;

	for (i=0; i<m_dofs.size(); i++)
		m_dofs[i] = dofCount++;

	return dofCount;
}

// ------------------------------------------------------------
int CDofs::getDof(unsigned int dof)
{
	if ((dof>0)&&(dof<=m_dofs.size()))
		return m_dofs[dof-1];
	else
		return -1;
}

// ------------------------------------------------------------
void CDofs::print(std::ostream& out)
{
	unsigned int i;

	out << "Dofs = [ ";

	for (i=0; i<m_dofs.size(); i++)
		out << m_dofs[i] << " ";

	out << "]" << std::endl;
}
