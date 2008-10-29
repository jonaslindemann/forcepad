// NOTE: please use a PRESERVE:BEGIN/PRESERVE:END comment block
//       to preserve your hand-coding across code generations.

#ifndef _CConstraintList_H_
#define _CConstraintList_H_

#include "RFemBase.h"

SmartPointer(CConstraintList);

class CConstraintList : public CBase {
public:
	CConstraintList ();
	CConstraintList (const CConstraintList&);
	virtual ~CConstraintList ();
	CConstraintList& operator = (const CConstraintList &arg);

	ClassInfo("CConstraintList", CBase);
};

#endif
