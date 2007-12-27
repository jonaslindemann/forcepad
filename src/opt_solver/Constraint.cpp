#include "Constraint.h"

// ------------------------------------------------------------
CConstraint::CConstraint ()
		:CBase()
{
}

// ------------------------------------------------------------
CConstraint::~CConstraint ()
{
}

// ------------------------------------------------------------
void CConstraint::setNode(CNode3d* node)
{
	m_node = node;
	m_prescribed.resize(m_node->getDofs()->getSize());
	m_value.resize(m_node->getDofs()->getSize());
}

// ------------------------------------------------------------
CNode3d* CConstraint::getNode()
{
	return m_node;
}

// ------------------------------------------------------------
void CConstraint::prescribeDof(unsigned int dof, double value)
{
	if (m_node!=NULL)
	{
		if ((dof>0)&&(dof<=m_node->getDofs()->getSize()))
		{
			m_prescribed[dof-1] = true;
			m_value[dof-1] = value;
		}
	}
}

// ------------------------------------------------------------
void CConstraint::releaseDof(unsigned int dof)
{
	if (m_node!=NULL)
	{
		if ((dof>0)&&(dof<=m_node->getDofs()->getSize()))
		{
			m_prescribed[dof-1] = false;
			m_value[dof-1] = 0.0;
		}
	}
}

