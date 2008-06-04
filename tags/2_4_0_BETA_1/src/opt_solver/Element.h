// NOTE: please use a PRESERVE:BEGIN/PRESERVE:END comment block
//       to preserve your hand-coding across code generations.

#ifndef _CElement_H_
#define _CElement_H_

#include "RFemBase.h"
#include "Node3d.h"

SmartPointer(CElement);

class CElement : public CBase {
private:
	std::vector<CNode3dPtr> m_nodes;
	unsigned int m_nodeDofSize;
public:
	CElement ();
	virtual ~CElement ();

	ClassInfo("CElement",CBase);

	void clear();
	void setNodeCount(unsigned int count);
	size_t getNodeCount();
	void setNode(unsigned int idx, CNode3d* node);
	CNode3d* getNode(unsigned int idx);
	void setNodeDofSize(unsigned int size);
	unsigned int getDofCount();

	virtual ReturnMatrix getStiffnessMatrix();

	void print(std::ostream& out);
};

#endif
