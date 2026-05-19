#include "UiSettings.h"

CUiSettings* CUiSettings::m_instance = 0;
CSingletonDestroyer<CUiSettings> CUiSettings::m_destroyer;

CUiSettings* CUiSettings::getInstance () 
{
    if (m_instance == 0)  
    {  
		m_instance = new CUiSettings(); 
		m_destroyer.setSingleton(m_instance);
    }
    return m_instance; 
}

CUiSettings::CUiSettings()
{
	m_symbolLength = 50.0;
	m_lineThickness = 3.0;
	m_devicePixelRatio = 1.0;
}

void CUiSettings::setDevicePixelRatio(double dpr)
{
	m_devicePixelRatio = dpr > 0.0 ? dpr : 1.0;
}

double CUiSettings::getDevicePixelRatio()
{
	return m_devicePixelRatio;
}

void CUiSettings::setLineThickness(double thickness)
{
	m_lineThickness = thickness;
}

double CUiSettings::getLineThickness()
{
	return m_lineThickness;
}

void CUiSettings::setSymbolLength(double length)
{
	m_symbolLength = length;
}

double CUiSettings::getSymbolLength()
{
	return m_symbolLength;
}

