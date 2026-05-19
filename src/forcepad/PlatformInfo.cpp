#include "PlatformInfo.h"

PlatformInfo* PlatformInfo::m_instance = 0;
ivf2d::SingletonDestroyer<PlatformInfo> PlatformInfo::m_destroyer;

#ifdef WIN32
#include <windows.h>
#include "SystemInfo.h"
#endif 

PlatformInfo* PlatformInfo::getInstance () 
{
    if (m_instance == 0)  
    {  
		m_instance = new PlatformInfo(); 
		m_destroyer.setSingleton(m_instance);
    }
    return m_instance; 
}

PlatformInfo::PlatformInfo()
{
	// Add protected construction code here
	m_fakeTabletPC = false;
}

bool PlatformInfo::isTabletPC()
{
	if (m_fakeTabletPC)
		return true;

#ifdef WIN32
	return GetSystemMetrics(SM_TABLETPC)!=0;
#else
	return false;
#endif
}

bool PlatformInfo::isVista()
{
#ifdef WIN32
	SystemInfo sysInfo;
	WindowsVersion winVer = sysInfo.GetWindowsVersion();
	if (winVer == WinVista)
		return true;
#endif
	return true;
}

void PlatformInfo::setFakeTabletPC(bool flag)
{
	m_fakeTabletPC = flag;
}

bool PlatformInfo::getFakeTabletPC()
{
	return m_fakeTabletPC;
}

void PlatformInfo::setApplicationExecutable(std::string& appExecutable)
{
    m_applicationExecutable = appExecutable;
}

const std::string PlatformInfo::getApplicationExecutable()
{
    return m_applicationExecutable;
}

