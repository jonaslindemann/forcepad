#ifndef _CUiSettings_h_
#define _CUiSettings_h_

#include "Base.h"
#include "SingletonDestroyer.h"

IvfStdPointer(CUiSettings);

class CUiSettings: public CBase {
private:
	static CUiSettings* m_instance;
	static CSingletonDestroyer<CUiSettings> m_destroyer;

	double m_symbolLength;
	double m_lineThickness;
public:
	/** Returns the PlatformInfo singleton */
	static CUiSettings* getInstance();

	IvfClassInfo("CUiSettings", CBase);

	void setSymbolLength(double length);
	double getSymbolLength();

	void setLineThickness(double thickness);
	double getLineThickness(); 

protected:
	/** Protected constructor (do not use) */
	CUiSettings();
	friend class CSingletonDestroyer<CUiSettings>;
};
#endif
