// NOTE: please use a PRESERVE:BEGIN/PRESERVE:END comment block
//       to preserve your hand-coding across code generations.


#include "Force.h"

// ------------------------------------------------------------
CForce::CForce ()
		:CVec3d()
{
}

// ------------------------------------------------------------
CForce::~CForce ()
{

}

// ------------------------------------------------------------
void CForce::setNode(CNode3d* node)
{
	m_node = node;
}

// ------------------------------------------------------------
CNode3d* CForce::getNode()
{
	return m_node;
}


