#ifndef _CPlatformInfo_h_
#define _CPlatformInfo_h_

#include "Base.h"
#include "SingletonDestroyer.h"

IvfStdPointer(CPlatformInfo);

class CPlatformInfo: public CBase {
private:
	static CPlatformInfo* m_instance;
	static CSingletonDestroyer<CPlatformInfo> m_destroyer;

	bool m_fakeTabletPC;
    std::string m_applicationExecutable;
public:
	/** Returns the PlatformInfo singleton */
	static CPlatformInfo* getInstance();

	IvfClassInfo("CPlatformInfo", CBase);

	bool isTabletPC();
	bool isVista();
	void setFakeTabletPC(bool flag);
	bool getFakeTabletPC();
    void setApplicationExecutable(std::string& appExecutable);
    const std::string getApplicationExecutable();
protected:
	/** Protected constructor (do not use) */
	CPlatformInfo();
	friend class CSingletonDestroyer<CPlatformInfo>;
};
#endif
