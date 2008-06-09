// NOTE: please use a PRESERVE:BEGIN/PRESERVE:END comment block
//       to preserve your hand-coding across code generations.

#ifndef _CElementProperty_H_
#define _CElementProperty_H_

#include "RFemBase.h"

SmartPointer(CElementProperty);

class CElementProperty : public CBase {
public:
	CElementProperty ();
	CElementProperty (const CElementProperty&);
	virtual ~CElementProperty ();
	CElementProperty& operator = (const CElementProperty &arg);

	ClassInfo("CElementProperty",CBase);
};

#endif
