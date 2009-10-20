//
// Copyright 1999-2006 by Structural Mechanics, Lund University.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "ivf@byggmek.lth.se".
//
//
// Written by Jonas Lindemann
//

#ifndef _rfem_CPointer_h_
#define _rfem_CPointer_h_

#include "RFemDef.h"

/**
 * Smart pointer class
 *
 * CPointer handles the Ivf++ reference counting scheme of
 * the CBase class. To declare a Ivf++ smart pointer use the 
 * IvfSmartPointer() macro. See the following example:
 *
 * \code
 * int main()
 * {
 *    CPointer<CMaterial> material = new CMaterial(); // addReference() called.
 *    CPointer<CMaterial> material2;
 *    material2 = material; // addReference() called 
 *    .
 *    .
 *    
 *    return 0;
 * } 
 * // material calls deleteReference()
 * // material2 calls deleteRefernce() and deletes CMaterial object
 * \endcode
 */
template <class T,class R> class CPointerRefBase {
private:
	T* m_object;
public:
	CPointerRefBase(T* object = 0)
	{
		m_object = object;
		if (m_object)
		{
			RFemDbg1("CPointerRefBase: created for " << m_object->getClassName());
			m_object->R::addReference();
		}
		else
		{
			RFemDbg1("CPointerRefBase: assigned 0");
		}
	}

	CPointerRefBase(const CPointerRefBase& ivfObject)
	{
		m_object = ivfObject.m_object;
		if (m_object)
		{
			RFemDbg1("CPointerRefBase: assigned " << m_object->getClassName());
			m_object->R::addReference();
		}
		else
		{
			RFemDbg1("CPointerRefBase: assigned 0");
		}
	}

	virtual ~CPointerRefBase()
	{
		if (m_object)
		{
			m_object->R::deleteReference();
			RFemDbg1("CPointerRefBase: " << m_object->getClassName() << " dereferenced.");
			if (!m_object->R::referenced())
			{
				RFemDbg1("CPointerRefBase: " << m_object->getClassName() << " deleted.");
				delete m_object;
			}
		}
	}

	operator T* () const { return m_object; }
	T& operator* () const { return *m_object; }
	T* operator-> () const { return m_object; }

	CPointerRefBase& operator= (const CPointerRefBase& IvfPointerRefBase)
	{
		if (m_object!=IvfPointerRefBase.m_object)
		{
			if (m_object)
			{
				m_object->R::deleteReference();
				RFemDbg1("CPointerRefBase(=): " << m_object->getClassName() << " dereferenced.");
				if (!m_object->R::referenced())
				{
					RFemDbg1("CPointerRefBase(=): " << m_object->getClassName() << " deleted.");
					delete m_object;
				}
			}

			m_object = IvfPointerRefBase.m_object;

			if (m_object)
			{
				RFemDbg1("CPointerRefBase(=): assigned " << m_object->getClassName());
				m_object->R::addReference();
			}
			else
			{
				RFemDbg1("CPointerRefBase(=): assigned 0");
			}
		}
		return *this;
	}

	CPointerRefBase& operator= (T* ivfObject)
	{
		if (m_object!=ivfObject)
		{
			if (m_object)
			{
				m_object->R::deleteReference();
				RFemDbg1("CPointerRefBase(=): " << m_object->getClassName() << " dereferenced.");
				if (!m_object->R::referenced())
				{
					RFemDbg1("CPointerRefBase(=): " << m_object->getClassName() << " deleted.");
					delete m_object;
				}
			}

			m_object = ivfObject;

			if (m_object)
			{
				RFemDbg1("CPointerRefBase(=): assigned " << m_object->getClassName());
				m_object->R::addReference();
			}
			else
			{
				RFemDbg1("CPointerRefBase(=): assigned 0");
			}
		}
		return *this;
	}

	bool operator== (T* ivfObject) const { return m_object == ivfObject; }
	bool operator!= (T* ivfObject) const { return m_object != ivfObject; }
	bool operator== (const CPointerRefBase& IvfPointerRefBase) const 
	{
		return m_object == IvfPointerRefBase.m_object;
	}

	bool operator!= (const CPointerRefBase& IvfPointerRefBase) const
	{
		return m_object != IvfPointerRefBase.m_object;
	}
};

template <class T> class CPointer {
private:
	T* m_object;
public:
	CPointer(T* object = 0)
	{
		m_object = object;
		if (m_object)
		{
			RFemDbg1("CPointer: created for " << m_object->getClassName());
			m_object->addReference();
		}
		else
		{
			RFemDbg1("CPointer: assigned 0");
		}
	}

	CPointer(const CPointer& ivfObject)
	{
		m_object = ivfObject.m_object;
		if (m_object)
		{
			RFemDbg1("CPointer: assigned " << m_object->getClassName());
			m_object->addReference();
		}
		else
		{
			RFemDbg1("CPointer: assigned 0");
		}
	}

	virtual ~CPointer()
	{
		if (m_object)
		{
			m_object->deleteReference();
			RFemDbg1("CPointer: " << m_object->getClassName() << " dereferenced.");
			if (!m_object->referenced())
			{
				RFemDbg1("CPointer: " << m_object->getClassName() << " deleted.");
				delete m_object;
			}
		}
	}

	operator T* () const { return m_object; }
	T& operator* () const { return *m_object; }
	T* operator-> () const { return m_object; }

	CPointer& operator= (const CPointer& ivfPointer)
	{
		if (m_object!=ivfPointer.m_object)
		{
			if (m_object)
			{
				m_object->deleteReference();
				RFemDbg1("CPointer(=): " << m_object->getClassName() << " dereferenced.");
				if (!m_object->referenced())
				{
					RFemDbg1("CPointer(=): " << m_object->getClassName() << " deleted.");
					delete m_object;
				}
			}

			m_object = ivfPointer.m_object;

			if (m_object)
			{
				RFemDbg1("CPointer(=): assigned " << m_object->getClassName());
				m_object->addReference();
			}
			else
			{
				RFemDbg1("CPointer(=): assigned 0");
			}
		}
		return *this;
	}

	CPointer& operator= (T* ivfObject)
	{
		if (m_object!=ivfObject)
		{
			if (m_object)
			{
				m_object->deleteReference();
				RFemDbg1("CPointer(=): " << m_object->getClassName() << " dereferenced.");
				if (!m_object->referenced())
				{
					RFemDbg1("CPointer(=): " << m_object->getClassName() << " deleted.");
					delete m_object;
				}
			}

			m_object = ivfObject;

			if (m_object)
			{
				RFemDbg1("CPointer(=): assigned " << m_object->getClassName());
				m_object->addReference();
			}
			else
			{
				RFemDbg1("CPointer(=): assigned 0");
			}
		}
		return *this;
	}

	bool operator== (T* ivfObject) const { return m_object == ivfObject; }
	bool operator!= (T* ivfObject) const { return m_object != ivfObject; }
	bool operator== (const CPointer& ivfPointer) const 
	{
		return m_object == ivfPointer.m_object;
	}

	bool operator!= (const CPointer& ivfPointer) const
	{
		return m_object != ivfPointer.m_object;
	}
};

#endif 
