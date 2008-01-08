// NOTE: please use a PRESERVE:BEGIN/PRESERVE:END comment block
//       to preserve your hand-coding across code generations.

#ifndef _CConstraint_H_
#define _CConstraint_H_

#include "RFemBase.h"

SmartPointer(CConstraint);

class CConstraint : public CBase {
private:
	std::vector<bool> m_prescribed;
	std::vector<double> m_value;
public:
	CConstraint ();
	virtual ~CConstraint ();

	void prescribeDof(unsigned int dof, double value);
	void releaseDof(unsigned int dof);
	bool isDofPrescribed(unsigned int dof);

	bool isActive();

	ClassInfo("CConstraint",CBase);
};

#endif
