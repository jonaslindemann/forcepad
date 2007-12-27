// NOTE: please use a PRESERVE:BEGIN/PRESERVE:END comment block
//       to preserve your hand-coding across code generations.

#ifndef _CNodeList_H_
#define _CNodeList_H_

#include "RFemBase.h"
#include "Node3d.h"

SmartPointer(CNodeList);

class CNodeList : public CBase {
private:
	vector<CNode3dPtr> m_nodeList;
public:
	CNodeList ();
	virtual ~CNodeList ();



	ClassInfo("CNodeList",CBase);
};

#endif
