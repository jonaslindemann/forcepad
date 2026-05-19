#ifndef _UiSettings_h_
#define _UiSettings_h_

#include "Base.h"
#include "SingletonDestroyer.h"

namespace fp {

IvfStdPointer(UiSettings);

class UiSettings: public ivf2d::Base {
private:
	static UiSettings* m_instance;
	static ivf2d::SingletonDestroyer<UiSettings> m_destroyer;

	double m_symbolLength;
	double m_lineThickness;
	double m_devicePixelRatio;
public:
	/** Returns the PlatformInfo singleton */
	static UiSettings* getInstance();

	IvfClassInfo("UiSettings", ivf2d::Base);

	void setSymbolLength(double length);
	double getSymbolLength();

	void setLineThickness(double thickness);
	double getLineThickness();

	void setDevicePixelRatio(double dpr);
	double getDevicePixelRatio();

protected:
	/** Protected constructor (do not use) */
	UiSettings();
	friend class ivf2d::SingletonDestroyer<UiSettings>;
};

} // namespace fp

#endif
