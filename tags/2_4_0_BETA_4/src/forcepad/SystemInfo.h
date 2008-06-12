#ifndef _CSystemInfo_h_
#define _CSystemInfo_h_

#include "Base.h"
#include "SingletonDestroyer.h"

IvfStdPointer(CSystemInfo);

class CSystemInfo: public CBase {
private:
	static CSystemInfo* m_instance;
	static CSingletonDestroyer<CSystemInfo> m_destroyer;
public:
	/** Returns the SystemInfo singleton */
	static CSystemInfo* getInstance();

	IvfClassInfo("CSystemInfo", CBase);

	bool isTabletPC();
protected:
	/** Protected constructor (do not use) */
	CSystemInfo();
	friend class CSingletonDestroyer<CSystemInfo>;
};
#endif
