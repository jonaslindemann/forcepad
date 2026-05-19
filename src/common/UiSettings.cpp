#include "UiSettings.h"

namespace fp {

UiSettings* UiSettings::m_instance = 0;
ivf2d::SingletonDestroyer<UiSettings> UiSettings::m_destroyer;

UiSettings* UiSettings::getInstance () 
{
    if (m_instance == 0)  
    {  
		m_instance = new UiSettings(); 
		m_destroyer.setSingleton(m_instance);
    }
    return m_instance; 
}

UiSettings::UiSettings()
{
	m_symbolLength = 50.0;
	m_lineThickness = 3.0;
	m_devicePixelRatio = 1.0;
}

void UiSettings::setDevicePixelRatio(double dpr)
{
	m_devicePixelRatio = dpr > 0.0 ? dpr : 1.0;
}

double UiSettings::getDevicePixelRatio()
{
	return m_devicePixelRatio;
}

void UiSettings::setLineThickness(double thickness)
{
	m_lineThickness = thickness;
}

double UiSettings::getLineThickness()
{
	return m_lineThickness;
}

void UiSettings::setSymbolLength(double length)
{
	m_symbolLength = length;
}

double UiSettings::getSymbolLength()
{
	return m_symbolLength;
}

} // namespace fp
