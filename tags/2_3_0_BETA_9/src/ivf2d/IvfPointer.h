//
// ForcePAD - Educational Finite Element Software
// Copyright (C) 2000-2003 Division of Structural Mecahnics, Lund University
//
// Written by Jonas Lindemann
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
// Comments and suggestions to jonas.lindemann@byggmek.lth.se
//

#ifndef _CIvfPointer_h_
#define _CIvfPointer_h_

#undef IVF2D_DEBUG

template <class T> class CIvfPointer {
private:
	T* m_object;
public:
	CIvfPointer(T* object = 0)
	{
		m_object = object;
		if (m_object)
			m_object->addReference();
	}

	CIvfPointer(const CIvfPointer& ivfObject)
	{
		m_object = ivfObject.m_object;
		if (m_object)
			m_object->addReference();
	}

	virtual ~CIvfPointer()
	{
		if (m_object)
		{
			m_object->delReference();
			if (!m_object->isReferenced())
			{
#ifdef IVF2D_DEBUG
				cout << m_object->getClassName() << " deleted." << endl;
#endif
				delete m_object;
			}
		}
	}

	operator T* () const { return m_object; }
	T& operator* () const { return *m_object; }
	T* operator-> () const { return m_object; }

	CIvfPointer& operator= (const CIvfPointer& ivfPointer)
	{
		if (m_object!=ivfPointer.m_object)
		{
			if (m_object)
			{
				m_object->delReference();
				if (!m_object->isReferenced())
				{
#ifdef IVF2D_DEBUG
					cout << m_object->getClassName() << " deleted." << endl;
#endif
					delete m_object;
				}
			}

			m_object = ivfPointer.m_object;

			if (m_object)
				m_object->addReference();
		}
		return *this;
	}

	CIvfPointer& operator= (T* ivfObject)
	{
		if (m_object!=ivfObject)
		{
			if (m_object)
			{
				m_object->delReference();
				if (!m_object->isReferenced())
				{
#ifdef IVF2D_DEBUG
					cout << m_object->getClassName() << " deleted." << endl;
#endif
					delete m_object;
				}
			}

			m_object = ivfObject;

			if (m_object)
				m_object->addReference();
		}
		return *this;
	}

	bool operator== (T* ivfObject) const { return m_object == ivfObject; }
	bool operator!= (T* ivfObject) const { return m_object != ivfObject; }
	bool operator== (const CIvfPointer& ivfPointer) const 
	{
		return m_object == ivfPointer.m_object;
	}

	bool operator!= (const CIvfPointer& ivfPointer) const
	{
		return m_object != ivfPointer.m_object;
	}
};


#endif 
