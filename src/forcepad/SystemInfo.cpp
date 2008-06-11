#include "SystemInfo.h"

CSystemInfo* CSystemInfo::m_instance = 0;
CSingletonDestroyer<CSystemInfo> CSystemInfo::m_destroyer;

#ifdef WIN32
#include <windows.h>
#endif 

CSystemInfo* CSystemInfo::getInstance () 
{
    if (m_instance == 0)  
    {  
		m_instance = new CSystemInfo(); 
		m_destroyer.setSingleton(m_instance);
    }
    return m_instance; 
}

CSystemInfo::CSystemInfo()
{
	// Add protected construction code here
}

bool CSystemInfo::isTabletPC()
{
#ifdef WIN32
	//return true;
	return GetSystemMetrics(SM_TABLETPC)!=0;
#else
	return false;
#endif
}
