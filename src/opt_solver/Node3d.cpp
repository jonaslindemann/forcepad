#include "Node3d.h"

CNode3d::CNode3d()
{
	m_dofs = new CDofs();
}

CNode3d::~CNode3d()
{

}

CDofs* CNode3d::getDofs()
{
	return m_dofs;
}

void CNode3d::setDofs(CDofs* dofs)
{
	m_dofs = dofs;
}

void CNode3d::setConstraint(CConstraint* constraint)
{
	m_constraint = constraint;
}

CConstraint* CNode3d::getConstraint()
{
	if (m_constraint == NULL)
		m_constraint = new CConstraint();
	return m_constraint;
}

void CNode3d::setForce(CForce* force)
{
	m_force = force;
}

CForce* CNode3d::getForce()
{
	if (m_force == NULL)
		m_force = new CForce();
	return m_force;
}

int CNode3d::enumerateDofs(int start)
{
	m_dofs->reset();

	if (m_constraint==NULL)
	{
		unsigned int i;
		int count = start;

		for (i=1; i<=m_dofs->getSize(); i++)
			m_dofs->setDof(i, count++);

		return count;
	}
	else
	{
		unsigned int i;
		int count = start;

		for (i=1; i<=m_dofs->getSize(); i++)
		{
			if (!m_constraint->isDofPrescribed(i))
				m_dofs->setDof(i, count++);
		}

		return count;		
	}
}

void CNode3d::prescribeDof(unsigned int dof, double value)
{
	CConstraintPtr constraint = this->getConstraint();
	constraint->prescribeDof(dof, value);
}

void CNode3d::releaseDof(unsigned int dof)
{
	if (m_constraint!=NULL)
	{
		m_constraint->releaseDof(dof);
		if (!m_constraint->isActive())
			m_constraint = NULL;
	}
}

bool CNode3d::isDofPrescribed(unsigned int dof)
{
	if (m_constraint==NULL)
		return false;
	else
		return m_constraint->isDofPrescribed(dof);
}

void CNode3d::applyForce(double fx, double fy)
{
	CForcePtr force = this->getForce();
	force->setComponents(fx, fy, 0.0);
}

void CNode3d::clearForce()
{
	if (m_force!=NULL)
		m_force = NULL;
}

void CNode3d::print(std::ostream& out)
{
	out << "Node3d = [ " << std::endl;
	out << "\t";
	CVec3d::print(out);
	out << "\t";
	m_dofs->print(out);
	out << "]" << std::endl;
}