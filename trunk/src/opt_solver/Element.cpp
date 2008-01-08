// NOTE: please use a PRESERVE:BEGIN/PRESERVE:END comment block
//       to preserve your hand-coding across code generations.


#include "Element.h"

// ------------------------------------------------------------
CElement::CElement ()
		:CBase()
{
	this->setNodeDofSize(2);
	this->setNodeCount(4);
}

// ------------------------------------------------------------
CElement::~CElement ()
{
}

// ------------------------------------------------------------
void CElement::clear()
{
	m_nodes.clear();
}

// ------------------------------------------------------------
void CElement::setNodeCount(unsigned int count)
{
	this->clear();

	unsigned int i;

	for (i=0; i<count; i++)
	{
		CNode3dPtr node = new CNode3d();
		node->getDofs()->setSize(m_nodeDofSize);
		m_nodes.push_back(node);
	}
}

// ------------------------------------------------------------
size_t CElement::getNodeCount()
{
	return m_nodes.size();
}

// ------------------------------------------------------------
void CElement::setNode(unsigned int idx, CNode3d* node)
{
	if ((idx>0)&&(idx<=m_nodes.size()))
	{
		m_nodes[idx-1] = node;
	}
}

// ------------------------------------------------------------
CNode3d* CElement::getNode(unsigned int idx)
{
	if ((idx>0)&&(idx<=m_nodes.size()))
		return m_nodes[idx-1];
	else
		return NULL;
}

// ------------------------------------------------------------
void CElement::print(std::ostream& out)
{
	unsigned int i;

	out << "Element = [" << std::endl;
	out << std::endl;
	for (i=0; i<m_nodes.size(); i++)
	{
		CNode3dPtr node = m_nodes[i];
		node->print(out);
	}

	out << "]" << std::endl;
	out << std::endl;
}

// ------------------------------------------------------------
ReturnMatrix CElement::getStiffnessMatrix()
{
	unsigned int keSize = this->getDofCount();

	Matrix Ke(keSize, keSize);

	Ke.release(); return Ke;
}

// ------------------------------------------------------------
void CElement::setNodeDofSize(unsigned int size)
{
	unsigned int i;

	for (i=0; i<m_nodes.size(); i++)
		m_nodes[i]->getDofs()->setSize(size);

	m_nodeDofSize = size;
}

// ------------------------------------------------------------
unsigned int CElement::getDofCount()
{
	unsigned int i;
	unsigned int dofCount = 0;

	for (i=0; i<m_nodes.size(); i++)
		dofCount += (unsigned int)m_nodes[i]->getDofs()->getSize();

	return dofCount;
}
