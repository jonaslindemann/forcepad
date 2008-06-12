// NOTE: please use a PRESERVE:BEGIN/PRESERVE:END comment block
//       to preserve your hand-coding across code generations.

#ifndef _CForceList_H_
#define _CForceList_H_

#include "RFemBase.h"

SmartPointer(CForceList);

class CForceList : public CBase {
public:
	CForceList ();
	CForceList (const CForceList&);
	virtual ~CForceList ();
	CForceList& operator = (const CForceList &arg);

	ClassInfo("CForceList",CBase);
};

#endif
