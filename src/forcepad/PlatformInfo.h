#ifndef _CPlatformInfo_h_
#define _CPlatformInfo_h_

#include "Base.h"
#include "SingletonDestroyer.h"

IvfStdPointer(CPlatformInfo);

class CPlatformInfo: public CBase {
private:
	static CPlatformInfo* m_instance;
	static CSingletonDestroyer<CPlatformInfo> m_destroyer;
public:
	/** Returns the PlatformInfo singleton */
	static CPlatformInfo* getInstance();

	IvfClassInfo("CPlatformInfo", CBase);

	bool isTabletPC();
	bool isVista();
protected:
	/** Protected constructor (do not use) */
	CPlatformInfo();
	friend class CSingletonDestroyer<CPlatformInfo>;
};
#endif
