#ifndef _CForce_H_
#define _CForce_H_

#include "Vec3d.h"
#include "Node3d.h"

SmartPointer(CForce);

class CForce : public CVec3d {
private:
	CNode3dPtr m_node;
public:
	CForce ();
	virtual ~CForce ();

	void setNode(CNode3d* node);
	CNode3d* getNode();

	ClassInfo("CForce", CVec3d);
};

#endif
