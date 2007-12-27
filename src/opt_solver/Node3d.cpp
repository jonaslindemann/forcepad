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


void CNode3d::print(std::ostream& out)
{
	out << "Node3d = [ " << std::endl;
	out << "\t";
	CVec3d::print(out);
	out << "\t";
	m_dofs->print(out);
	out << "]" << std::endl;
}