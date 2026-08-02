#ifndef _UiSettings_h_
#define _UiSettings_h_

#include "Base.h"

namespace fp {

class UiSettings;
using UiSettingsPtr = UiSettings*;

class UiSettings: public ivf2d::Base {
private:
	double m_symbolLength{50.0};
	double m_lineThickness{3.0};
	double m_devicePixelRatio{1.0};
public:
	/**
	 * Returns the UiSettings singleton.
	 *
	 * Backed by a function-local static, which C++11 guarantees is initialised
	 * exactly once (thread-safely) and destroyed at exit -- replacing the
	 * hand-rolled SingletonDestroyer that used to manage the teardown.
	 */
	static UiSettings* getInstance();

	void setSymbolLength(double length);
	double getSymbolLength();

	void setLineThickness(double thickness);
	double getLineThickness();

	void setDevicePixelRatio(double dpr);
	double getDevicePixelRatio();

protected:
	/** Protected constructor -- use getInstance(). */
	UiSettings() = default;
};

} // namespace fp

#endif
