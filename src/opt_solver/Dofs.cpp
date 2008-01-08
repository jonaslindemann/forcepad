#include "Dofs.h"

CDofs::CDofs ()
		:CBase()
{
	this->setSize(2);
}

CDofs::~CDofs ()
{
}

void CDofs::setSize(int nDofs)
{
	m_dofs.resize(nDofs);
	this->reset();
}

size_t CDofs::getSize()
{
	return m_dofs.size();
}

void CDofs::reset()
{
	unsigned int i;

	for (i=0; i<m_dofs.size(); i++)
		m_dofs[i] = -1;
}

int CDofs::getDof(unsigned int dof)
{
	if ((dof>0)&&(dof<=m_dofs.size()))
		return m_dofs[dof-1];
	else
		return -1;
}

void CDofs::setDof(unsigned int dof, int number)
{
	if ((dof>0)&&(dof<=m_dofs.size()))
		m_dofs[dof-1] = number;
}

void CDofs::print(std::ostream& out)
{
	unsigned int i;

	out << "Dofs = [ ";

	for (i=0; i<m_dofs.size(); i++)
		out << m_dofs[i] << " ";

	out << "]" << std::endl;
}
