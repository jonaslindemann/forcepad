#include "UiSettings.h"

namespace fp {

UiSettings* UiSettings::getInstance()
{
	// Meyers singleton: initialised on first use, destroyed at exit, and
	// thread-safe since C++11 without any explicit teardown helper.
	static UiSettings instance;
	return &instance;
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
