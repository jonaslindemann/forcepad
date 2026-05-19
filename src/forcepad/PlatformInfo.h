#ifndef _PlatformInfo_h_
#define _PlatformInfo_h_

#include "Base.h"
#include "SingletonDestroyer.h"

IvfStdPointer(PlatformInfo);

class PlatformInfo: public ivf2d::Base {
private:
	static PlatformInfo* m_instance;
	static ivf2d::SingletonDestroyer<PlatformInfo> m_destroyer;

	bool m_fakeTabletPC;
    std::string m_applicationExecutable;
public:
	/** Returns the PlatformInfo singleton */
	static PlatformInfo* getInstance();

	IvfClassInfo("PlatformInfo", ivf2d::Base);

	bool isTabletPC();
	bool isVista();
	void setFakeTabletPC(bool flag);
	bool getFakeTabletPC();
    void setApplicationExecutable(std::string& appExecutable);
    const std::string getApplicationExecutable();
protected:
	/** Protected constructor (do not use) */
	PlatformInfo();
	friend class ivf2d::SingletonDestroyer<PlatformInfo>;
};
#endif
