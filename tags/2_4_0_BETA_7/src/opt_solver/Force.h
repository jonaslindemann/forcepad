#ifndef _CForce_H_
#define _CForce_H_

#include "Vec3d.h"

SmartPointer(CForce);

class CForce : public CVec3d {
private:

public:
	CForce ();
	virtual ~CForce ();

	ClassInfo("CForce", CVec3d);
};

#endif
