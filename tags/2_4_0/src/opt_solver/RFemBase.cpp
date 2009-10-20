#include "RFemBase.h"

// ------------------------------------------------------------
CBase::CBase()
{
	// Set initial reference count

	m_ref = 0;
}

// ------------------------------------------------------------
CBase::~CBase()
{

}

// ------------------------------------------------------------
void CBase::addReference()
{
	m_ref++;
}

// ------------------------------------------------------------
void CBase::deleteReference()
{
	if (m_ref>0)
		m_ref--;
}

// ------------------------------------------------------------
bool CBase::referenced()
{
	if (m_ref>0)
		return true;
	else
		return false;
}

// ------------------------------------------------------------
int CBase::getRefCount()
{
	return m_ref;
}

// ------------------------------------------------------------
void CBase::print(std::ostream& out)
{
	out << "Base class." << std::endl;
}
