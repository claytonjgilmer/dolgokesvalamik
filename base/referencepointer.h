#ifndef _referencepointer_h_
#define _referencepointer_h_

#include "referencedobject.h"

namespace base
{
	class referencepointer
	{
	public:
		referencepointer();
		referencepointer(const referencepointer& i_other);
		referencepointer(referencedobject* i_object);

		void operator=(const referencepointer& i_other);
		void operator=(referencedobject* i_object);

		referencedobject* operator*();

		~referencepointer();
	//private:
		referencedobject* m_ptr;
	};

	inline referencepointer::referencepointer()
	{
		m_ptr=0;
	}

	inline referencepointer::referencepointer(const referencepointer& i_other)
	{
		m_ptr=i_other.m_ptr;

		if (m_ptr)
			m_ptr->add_ref();
	}

	inline referencepointer::referencepointer(referencedobject* i_object)
	{
		m_ptr=i_object;

		if (m_ptr)
			m_ptr->add_ref();
	}

	inline void referencepointer::operator =(const referencepointer& i_other)
	{
		if (m_ptr)
		{
			m_ptr->remove_ref();

			if (!m_ptr->m_refcount)
				delete m_ptr;
		}

		m_ptr=i_other.m_ptr;

		if (m_ptr)
			m_ptr->add_ref();
	}

	inline void referencepointer::operator =(referencedobject* i_object)
	{
		if (m_ptr)
		{
			m_ptr->remove_ref();

			if (!m_ptr->m_refcount)
				delete m_ptr;
		}

		m_ptr=i_object;

		if (m_ptr)
			m_ptr->add_ref();
	}

	inline referencedobject* referencepointer::operator *()
	{
		return m_ptr;
	}

	inline referencepointer::~referencepointer()
	{
		if (m_ptr)
		{
			m_ptr->remove_ref();

			if (!m_ptr->m_refcount)
				delete m_ptr;

			m_ptr=NULL;
		}
	}
}
#endif//_referencepointer_h_