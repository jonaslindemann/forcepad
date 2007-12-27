#ifndef _CNode3d_h_
#define _CNode3d_h_

#include "Vec3d.h"
#include "Dofs.h"

SmartPointer(CNode3d);

class CNode3d : public CVec3d {
private:
	CDofsPtr m_dofs;
public:
	CNode3d();
	virtual ~CNode3d();

	ClassInfo("CNode3d",CVec3d);

	CDofs* getDofs();
	void setDofs(CDofs* dofs);

	virtual void print(std::ostream& output);
};

#endif
