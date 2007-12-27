#ifndef _CPoint3d_h_
#define _CPoint3d_h_

#include "Vec3d.h"

SmartPointer(CPoint3d);

class CPoint3d : public CVec3d {
public:
	ClassInfo("CPoint3d",CVec3d);
};

#endif
