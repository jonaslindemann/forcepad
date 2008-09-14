#include "PlatformInfo.h"

CPlatformInfo* CPlatformInfo::m_instance = 0;
CSingletonDestroyer<CPlatformInfo> CPlatformInfo::m_destroyer;

#ifdef WIN32
#include <windows.h>
#include "SystemInfo.h"
#endif 

CPlatformInfo* CPlatformInfo::getInstance () 
{
    if (m_instance == 0)  
    {  
		m_instance = new CPlatformInfo(); 
		m_destroyer.setSingleton(m_instance);
    }
    return m_instance; 
}

CPlatformInfo::CPlatformInfo()
{
	// Add protected construction code here
}

bool CPlatformInfo::isTabletPC()
{
#ifdef WIN32
	return GetSystemMetrics(SM_TABLETPC)!=0;
#else
	return false;
#endif
}

bool CPlatformInfo::isVista()
{
#ifdef WIN32
	SystemInfo sysInfo;
	WindowsVersion winVer = sysInfo.GetWindowsVersion();
	if (winVer == WinVista)
		return true;
#endif
	return true;
}
