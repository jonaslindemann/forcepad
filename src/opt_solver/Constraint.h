// NOTE: please use a PRESERVE:BEGIN/PRESERVE:END comment block
//       to preserve your hand-coding across code generations.

#ifndef _CConstraint_H_
#define _CConstraint_H_

#include "RFemBase.h"
#include "Node3d.h"

SmartPointer(CConstraint);

class CConstraint : public CBase {
private:
	CNode3dPtr m_node;
	std::vector<bool> m_prescribed;
	std::vector<double> m_value;
public:
	CConstraint ();
	virtual ~CConstraint ();

	void setNode(CNode3d* node);
	CNode3d* getNode();

	void prescribeDof(unsigned int dof, double value);
	void releaseDof(unsigned int dof);

	ClassInfo("CConstraint",CBase);
};

#endif
