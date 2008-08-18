// SystemInfo.cpp: implementation of the SystemInfo class.
//////////////////////////////////////////////////////////////////////

#include "SystemInfo.h"

#include <tchar.h>
#include <stdio.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define BUFSIZE 80

typedef void (WINAPI *PGetNativeSystemInfo)(LPSYSTEM_INFO);

typedef BOOL (WINAPI *PGetProductInfo)(DWORD, DWORD, DWORD, DWORD, PDWORD);


SystemInfo::SystemInfo()
{
	DetectWindowsVersion();
}

SystemInfo::~SystemInfo()
{

}

bool SystemInfo::DetectWindowsVersion()
{
	PGetNativeSystemInfo pGNSI = NULL;

	m_bOsVersionInfoEx = FALSE;
	m_nWinVersion = Win_Unknown;
	memset(m_szServicePack, 0, sizeof(m_szServicePack));

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	ZeroMemory(&m_osvi, sizeof(OSVERSIONINFOEX));
	m_osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if( !(m_bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &m_osvi)) )
	{
		// If that fails, try using the OSVERSIONINFO structure.
		m_osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (! GetVersionEx ( (OSVERSIONINFO *) &m_osvi) ) 
			return false;
	}

	pGNSI = (PGetNativeSystemInfo) GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetNativeSystemInfo");
	if(NULL != pGNSI) pGNSI(&m_SysInfo);
	else GetSystemInfo(&m_SysInfo);

	switch (m_osvi.dwPlatformId)
	{

		// Test for the Windows NT product family.
	case VER_PLATFORM_WIN32_NT:
		// Test for the specific product family
		if(m_osvi.dwMajorVersion == 6)
			m_nWinVersion = WinVista;

		else if ( m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 2 )
			m_nWinVersion = WinServer2003;

		else if ( m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 1 )
			m_nWinVersion = WinXP;

		else if ( m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 0 )
			m_nWinVersion = Win2000_Unknown;

		else if ( m_osvi.dwMajorVersion <= 4 )
			m_nWinVersion = WinNT;

		// Test for specific product on Windows NT 4.0 SP6 and later.
		if( m_bOsVersionInfoEx )
		{
			// Test for the workstation type.
			if ( m_osvi.wProductType == VER_NT_WORKSTATION &&
				m_SysInfo.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_AMD64)
			{
				switch(m_osvi.dwMajorVersion)
				{
				case 4:
					m_nWinVersion = WinNT_40_Workstation;
					break;
				case 5:
					if( m_osvi.wSuiteMask & VER_SUITE_PERSONAL )
					{
						if(m_osvi.dwMinorVersion == 1)
							m_nWinVersion = WinXP_Home;
					}
					else
					{
						if(m_osvi.dwMinorVersion == 1)
							m_nWinVersion = WinXP_Professional;
						else if(m_osvi.dwMinorVersion == 0)
							m_nWinVersion = Win2000_Professional;
					}
					break;
				case 6:
					if( m_osvi.wSuiteMask & VER_SUITE_PERSONAL )
					{
						m_nWinVersion = WinVistaHome;
					}
					break;
				}
			}

			// Test for the server type.
			else if ( m_osvi.wProductType == VER_NT_SERVER || m_osvi.wProductType == VER_NT_DOMAIN_CONTROLLER)
			{
				// Windows Server 2008
				if( m_osvi.dwMajorVersion == 6 && m_osvi.dwMinorVersion == 0 )
				{
					if( m_osvi.wSuiteMask & VER_SUITE_DATACENTER )
						m_nWinVersion = WinServer2008_Datacenter;
					else if( m_osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						m_nWinVersion = WinServer2008_Enterprise;
					else
						m_nWinVersion = WinServer2008;
				}

				// Windows Server 2003
				else if( m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 2 )
				{
					if(m_SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
					{
						if( m_osvi.wSuiteMask & VER_SUITE_DATACENTER )
							m_nWinVersion = WinServer2003_IA64_Datacenter;
						else if( m_osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
							m_nWinVersion = WinServer2003_IA64_Enterprise;
						else
							m_nWinVersion = WinServer2003_IA64;
					}
					else if(m_SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					{
						if( m_osvi.wSuiteMask & VER_SUITE_DATACENTER )
							m_nWinVersion = WinServer2003_AMD64_Datacenter;
						else if( m_osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
							m_nWinVersion = WinServer2003_AMD64_Enterprise;
						else
							m_nWinVersion = WinServer2003_AMD64;
					}
					else
					{
						if( m_osvi.wSuiteMask & VER_SUITE_DATACENTER )
							m_nWinVersion = WinServer2003_Datacenter;
						else if( m_osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
							m_nWinVersion = WinServer2003_Enterprise;
						else if ( m_osvi.wSuiteMask == VER_SUITE_BLADE )
							m_nWinVersion = WinServer2003_WebEdition;
						else
							m_nWinVersion = WinServer2003;
					}
				}

				// Windows 2000
				else if( m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 0 )
				{
					if( m_osvi.wSuiteMask & VER_SUITE_DATACENTER )
						m_nWinVersion = Win2000_Server_Datacenter;
					else if( m_osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						m_nWinVersion = Win2000_Server_Advanced;
					else
						m_nWinVersion = Win2000_Server;
				}

				// Windows NT 4.0 
				else 
				{
					if( m_osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
						m_nWinVersion = WinNT_40_Server_Enterprise;					
					else
						m_nWinVersion = WinNT_40_Server;
				}
			}
		}
		else  // Test for specific product on Windows NT 4.0 SP5 and earlier
		{
			HKEY hKey;
			char szProductType[BUFSIZE];
			DWORD dwBufLen=BUFSIZE;
			LONG lRet;

			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				"SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
				0, KEY_QUERY_VALUE, &hKey );
			if( lRet != ERROR_SUCCESS )
				return false;

			lRet = RegQueryValueEx( hKey, "ProductType", NULL, NULL,
				(LPBYTE) szProductType, &dwBufLen);
			if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
				return false;

			RegCloseKey( hKey );

			if ( lstrcmpi( "WINNT", szProductType) == 0 )
			{
				if ( m_osvi.dwMajorVersion <= 4 )
					m_nWinVersion = WinNT_40_Workstation;
			}
			if ( lstrcmpi( "LANMANNT", szProductType) == 0 )
			{
				if ( m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 2 )
					m_nWinVersion = WinServer2003;

				if ( m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 0 )
					m_nWinVersion = Win2000_Server;

				if ( m_osvi.dwMajorVersion <= 4 )
					m_nWinVersion = WinNT_40_Server;
			}
			if ( lstrcmpi( "SERVERNT", szProductType) == 0 )
			{
				if ( m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 2 )
					m_nWinVersion = WinServer2003_Enterprise;

				if ( m_osvi.dwMajorVersion == 5 && m_osvi.dwMinorVersion == 0 )
					m_nWinVersion = Win2000_Server_Advanced;

				if ( m_osvi.dwMajorVersion <= 4 )
					m_nWinVersion = WinNT_40_Server_Enterprise;
			}
		}

		// Display service pack (if any) and build number.

		if( m_osvi.dwMajorVersion == 4 && 
			lstrcmpi( m_osvi.szCSDVersion, "Service Pack 6" ) == 0 )
		{
			HKEY hKey;
			LONG lRet;

			// Test for SP6 versus SP6a.
			lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
				"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Hotfix\\Q246009",
				0, KEY_QUERY_VALUE, &hKey );
			if( lRet == ERROR_SUCCESS )
				sprintf(m_szServicePack, "Service Pack 6a (Build %d)\n", m_osvi.dwBuildNumber & 0xFFFF );         
			else // Windows NT 4.0 prior to SP6a
			{
				sprintf(m_szServicePack, "%s (Build %d)\n",
					m_osvi.szCSDVersion,
					m_osvi.dwBuildNumber & 0xFFFF);
			}

			RegCloseKey( hKey );
		}
		else // Windows NT 3.51 and earlier or Windows 2000 and later
		{
			sprintf(m_szServicePack, "%s (Build %d)\n",
				m_osvi.szCSDVersion,
				m_osvi.dwBuildNumber & 0xFFFF);
		}
		break;

		// Test for the Windows 95 product family.
	case VER_PLATFORM_WIN32_WINDOWS:
		if (m_osvi.dwMajorVersion == 4 && m_osvi.dwMinorVersion == 0)
		{
			m_nWinVersion = Win95;
			if ( m_osvi.szCSDVersion[1] == 'C' || m_osvi.szCSDVersion[1] == 'B' )
				m_nWinVersion = Win95_OSR2;
		} 

		if (m_osvi.dwMajorVersion == 4 && m_osvi.dwMinorVersion == 10)
		{
			m_nWinVersion = Win98;
			if ( m_osvi.szCSDVersion[1] == 'A' )
				m_nWinVersion = Win98_SE;
		} 

		if (m_osvi.dwMajorVersion == 4 && m_osvi.dwMinorVersion == 90)
		{
			m_nWinVersion = WinMe;
		} 
		break;

	case VER_PLATFORM_WIN32s:
		m_nWinVersion = Win32s;
		break;
	}

	// try to acquire additional information about the operating system
	// works only for Vista and above
	DetectProductInfo();

	return true;
}

void SystemInfo::DetectProductInfo()
{
#if _WIN32_WINNT >= 0x0600 
	if(m_osvi.dwMajorVersion >= 6)
	{
		PGetProductInfo lpProducInfo = (PGetProductInfo)GetProcAddress(
			GetModuleHandle(_T("kernel32.dll")), "GetProductInfo");
		if(NULL != lpProducInfo)
		{
			DWORD prodType = 0;
			if(lpProducInfo(m_osvi.dwMajorVersion, 
							m_osvi.dwMinorVersion, 
							m_osvi.wServicePackMajor, 
							m_osvi.wServicePackMinor, 
							&prodType))
			{
				switch(prodType)
				{
				case PRODUCT_UNLICENSED:
					switch(m_nWinVersion)
					{
					case WinVista:
					case WinVistaHome:
						m_nWinVersion = WinVistaUnlinsed;
						break;
					case WinServer2008:
					case WinServer2008_Enterprise:
					case WinServer2008_Datacenter:
						m_nWinVersion = WinServer2008Unlinsed;
						break;
					}
					break;
				case PRODUCT_BUSINESS: case PRODUCT_BUSINESS_N:
					m_nWinVersion = WinVistaBusiness;
					break;
				case PRODUCT_ULTIMATE:
					m_nWinVersion = WinVistaUltimate;
					break;
				case PRODUCT_ENTERPRISE:
					m_nWinVersion = WinVistaEnterprise;
					break;
				case PRODUCT_HOME_BASIC: case PRODUCT_HOME_BASIC_N:
					m_nWinVersion = WinVistaHomeBasic;
					break;
				case PRODUCT_HOME_PREMIUM:
					m_nWinVersion = WinVistaHomePremium;
					break;
				case PRODUCT_STARTER:
					m_nWinVersion = WinVistaStarter;
					break;
				case PRODUCT_DATACENTER_SERVER:
					m_nWinVersion = WinServer2008_Datacenter_Full;
					break;
				case PRODUCT_DATACENTER_SERVER_CORE:
					m_nWinVersion = WinServer2008_Datacenter_Core;
					break;
				case PRODUCT_ENTERPRISE_SERVER:
					m_nWinVersion = WinServer2008_Enterprise_Full;
					break;
				case PRODUCT_ENTERPRISE_SERVER_CORE:
					m_nWinVersion = WinServer2008_Enterprise_Core;
					break;
				case PRODUCT_ENTERPRISE_SERVER_IA64:
					m_nWinVersion = WinServer2008_Enterprise_IA64;
					break;
				case PRODUCT_STANDARD_SERVER:
					m_nWinVersion = WinServer2008_Standard_Full;
					break;
				case PRODUCT_STANDARD_SERVER_CORE:
					m_nWinVersion = WinServer2008_Standard_Core;
					break;
				case PRODUCT_WEB_SERVER:
					m_nWinVersion = WinServer2008_WebServer;
					break;
				}
			}
		}
	}
#endif
}

WindowsVersion SystemInfo::GetWindowsVersion() const
{
	return m_nWinVersion;
}

bool SystemInfo::IsNTPlatform() const
{
	return m_osvi.dwPlatformId == VER_PLATFORM_WIN32_NT;
}

bool SystemInfo::IsWindowsPlatform() const
{
	return m_osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS;
}

bool SystemInfo::IsWin32sPlatform() const
{
	return m_osvi.dwPlatformId == VER_PLATFORM_WIN32s;
}
	
DWORD SystemInfo::GetMajorVersion() const
{
	return m_osvi.dwMajorVersion;
}

DWORD SystemInfo::GetMinorVersion() const
{
	return m_osvi.dwMinorVersion;
}

DWORD SystemInfo::GetBuildNumber() const
{
	return m_osvi.dwBuildNumber;
}

DWORD SystemInfo::GetPlatformID() const
{
	return m_osvi.dwPlatformId;	
}

// PARAMETER szServicePack must not be NULL
void SystemInfo::GetServicePackInfo(TCHAR* szServicePack) const
{
	if(szServicePack == NULL) return;
	
	_tcscpy(szServicePack, m_szServicePack);
}

bool SystemInfo::Is32bitPlatform() const
{
	return !Is64bitPlatform();
}

bool SystemInfo::Is64bitPlatform() const
{
	return (
		m_SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 || 
		m_SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		m_SysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ALPHA64);
}
