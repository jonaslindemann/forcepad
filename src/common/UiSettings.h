#ifndef _CUiSettings_h_
#define _CUiSettings_h_

#include "Base.h"
#include "SingletonDestroyer.h"

IvfStdPointer(CUiSettings);

class CUiSettings: public ivf2d::Base {
private:
	static CUiSettings* m_instance;
	static ivf2d::SingletonDestroyer<CUiSettings> m_destroyer;

	double m_symbolLength;
	double m_lineThickness;
	double m_devicePixelRatio;
public:
	/** Returns the PlatformInfo singleton */
	static CUiSettings* getInstance();

	IvfClassInfo("CUiSettings", ivf2d::Base);

	void setSymbolLength(double length);
	double getSymbolLength();

	void setLineThickness(double thickness);
	double getLineThickness();

	void setDevicePixelRatio(double dpr);
	double getDevicePixelRatio();

protected:
	/** Protected constructor (do not use) */
	CUiSettings();
	friend class ivf2d::SingletonDestroyer<CUiSettings>;
};
#endif
