#include "Constraint.h"

CConstraint::CConstraint ()
		:CBase()
{
	m_prescribed.resize(2);
	m_value.resize(2);
}

CConstraint::~CConstraint ()
{
}

void CConstraint::prescribeDof(unsigned int dof, double value)
{
	if ((dof>0)&&(dof<=2))
	{
		m_prescribed[dof-1] = true;
		m_value[dof-1] = value;
	}
}

void CConstraint::releaseDof(unsigned int dof)
{
	if ((dof>0)&&(dof<=2))
	{
		m_prescribed[dof-1] = false;
		m_value[dof-1] = 0.0;
	}
}

bool CConstraint::isDofPrescribed(unsigned int dof)
{
	if ((dof>0)&&(dof<=2))
		return m_prescribed[dof-1];
	return false;
}

bool CConstraint::isActive()
{
	unsigned int i;
	bool active = false;

	for (i=0; i<m_prescribed.size(); i++)
		if (m_prescribed[i])
			active = true;
	
	return active;
}
