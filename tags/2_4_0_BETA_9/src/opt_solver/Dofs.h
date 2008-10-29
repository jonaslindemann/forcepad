// NOTE: please use a PRESERVE:BEGIN/PRESERVE:END comment block
//       to preserve your hand-coding across code generations.

#ifndef _CDofs_H_
#define _CDofs_H_

#include "RFemBase.h"

SmartPointer(CDofs);

class CDofs : public CBase {
private:
	std::vector<int> m_dofs;
public:
	CDofs ();
	virtual ~CDofs ();

	ClassInfo("CDofs",CBase);

	void reset();

	void setSize(int nDofs);
	size_t getSize();

	int getDof(unsigned int dof);
	void setDof(unsigned int dof, int number);

	void print(std::ostream& out);
};

#endif
