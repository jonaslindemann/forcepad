// NOTE: please use a PRESERVE:BEGIN/PRESERVE:END comment block
//       to preserve your hand-coding across code generations.

#ifndef _CElementList_H_
#define _CElementList_H_

#include "RFemBase.h"

SmartPointer(CElementList);

class CElementList : public CBase {
public:
	CElementList ();
	CElementList (const CElementList&);
	virtual ~CElementList ();
	CElementList& operator = (const CElementList &arg);

	ClassInfo("CElementList",CBase);
};

#endif
