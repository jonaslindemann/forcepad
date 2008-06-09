#ifndef _CRFemBase_h_
#define _CRFemBase_h_

#include "RFemDef.h"
#include "Pointer.h"

SmartPointer(CBase);

class CBase {
private:
	int m_ref;
public:
	CBase();
	virtual ~CBase();

	ClassInfoTop("CBase");

	int getRefCount();
	bool referenced();
	void addReference();
	void deleteReference();

	virtual void print(std::ostream& out);
};

#endif 
