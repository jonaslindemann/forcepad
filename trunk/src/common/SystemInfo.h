/* 
 * copyright (c) 2005 - 2007 Marius Bancila
 * http://www.mariusbancila.ro
 * http://www.mariusbancila.ro/blog
 */

//////////////////////////////////////////////////////////////////////
// SystemInfo.h: interface for the SystemInfo class.
//////////////////////////////////////////////////////////////////////
#pragma once

#include <Windows.h>

typedef enum WindowsVersion
{
	Win_Unknown = 0,
	Win95,
	Win95_OSR2,
	Win98,
	Win98_SE,
	WinMe,

	WinNT,
	WinNT_351,
	WinNT_40_Workstation,
	WinNT_40_Server,
	WinNT_40_Server_Enterprise,

	Win2000_Unknown,
	Win2000_Professional,
	Win2000_Server,
	Win2000_Server_Datacenter,
	Win2000_Server_Advanced,

	WinXP,
	WinXP_Home,
	WinXP_Professional,

	WinServer2003,
	WinServer2003_Enterprise,
	WinServer2003_Datacenter,
	WinServer2003_WebEdition,
	WinServer2003_IA64,
	WinServer2003_IA64_Enterprise,
	WinServer2003_IA64_Datacenter,
	WinServer2003_AMD64,
	WinServer2003_AMD64_Enterprise,
	WinServer2003_AMD64_Datacenter,

	WinVista,
	WinVistaUnlinsed,
	WinVistaUltimate,
	WinVistaBusiness,
	WinVistaEnterprise,
	WinVistaHome,
	WinVistaHomeBasic,
	WinVistaHomePremium,
	WinVistaStarter,

	WinServer2008,
	WinServer2008Unlinsed,
	WinServer2008_Enterprise,
	WinServer2008_Enterprise_Core,
	WinServer2008_Enterprise_Full,
	WinServer2008_Enterprise_IA64,
	WinServer2008_Datacenter,
	WinServer2008_Datacenter_Core,
	WinServer2008_Datacenter_Full,
	WinServer2008_Standard,
	WinServer2008_Standard_Core,
	WinServer2008_Standard_Full,
	WinServer2008_WebServer,

	Win32s,	
};

class SystemInfo  
{
	WindowsVersion	m_nWinVersion;
	TCHAR			m_szServicePack[128];
	OSVERSIONINFOEX m_osvi;
	SYSTEM_INFO		m_SysInfo;
	BOOL			m_bOsVersionInfoEx;

private:
	bool DetectWindowsVersion();
	void DetectProductInfo();

public:
	SystemInfo();
	virtual ~SystemInfo();

	WindowsVersion GetWindowsVersion() const;		// returns the windows version
	bool IsNTPlatform() const;						// true if NT platform
	bool IsWindowsPlatform() const;					// true is Windows platform
	bool IsWin32sPlatform() const;					// true is Win32s platform
	DWORD GetMajorVersion() const;					// returns major version
	DWORD GetMinorVersion() const;					// returns minor version
	DWORD GetBuildNumber() const;					// returns build number
	DWORD GetPlatformID() const;					// returns platform ID
	void GetServicePackInfo(TCHAR* szServicePack) const;// additional information about service pack
	bool Is32bitPlatform() const;					// true if platform is 32-bit
	bool Is64bitPlatform() const;					// true if platform is 64-bit
};
