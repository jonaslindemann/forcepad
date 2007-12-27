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
	CDofs (CDofs&);
	virtual ~CDofs ();
	CDofs& operator = (const CDofs &arg);

	ClassInfo("CDofs",CBase);

	void setSize(int nDofs);
	size_t getSize();
	int enumerate(int start);
	int getDof(unsigned int dof);
	void reset();
	void print(std::ostream& out);
};

#endif
