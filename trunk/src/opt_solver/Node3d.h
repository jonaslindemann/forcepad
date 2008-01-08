#ifndef _CNode3d_h_
#define _CNode3d_h_

#include "Vec3d.h"
#include "Dofs.h"
#include "Constraint.h"

SmartPointer(CNode3d);

class CNode3d : public CVec3d {
private:
	CDofsPtr m_dofs;
	CConstraintPtr m_constraint;
public:
	CNode3d();
	virtual ~CNode3d();

	ClassInfo("CNode3d",CVec3d);

	int enumerateDofs(int start);

	void prescribeDof(unsigned int dof, double value);
	void releaseDof(unsigned int dof);
	bool isDofPrescribed(unsigned int dof);

	CDofs* getDofs();
	void setDofs(CDofs* dofs);

	void setConstraint(CConstraint* constraint);
	CConstraint* getConstraint();

	virtual void print(std::ostream& output);
};

#endif
