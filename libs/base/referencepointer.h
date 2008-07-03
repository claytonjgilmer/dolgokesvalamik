#ifndef _referencepointer_h_
#define _referencepointer_h_

#include "referencedobject.h"
#include "base/misc.h"

namespace base
{
	template <class REF>
	class referencepointer
	{
	public:
		referencepointer();
		referencepointer(const referencepointer& i_other);
		referencepointer(REF* i_object);

		void operator=(const referencepointer& i_other);
		void operator=(REF* i_object);

		REF* operator*();

		~referencepointer();
	//private:
		REF* m_ptr;
	};

	template <class REF>
	inline referencepointer<REF>::referencepointer()
	{
		m_ptr=0;
	}

	template <class REF>
	inline referencepointer<REF>::referencepointer(const referencepointer& i_other)
	{
		m_ptr=i_other.m_ptr;

		if (m_ptr)
			m_ptr->add_ref();
	}

	template <class REF>
	inline referencepointer<REF>::referencepointer(REF* i_object)
	{
		m_ptr=i_object;

		if (m_ptr)
			m_ptr->add_ref();
	}

	template <class REF>
	inline void referencepointer<REF>::operator =(const referencepointer& i_other)
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

	template <class REF>
	inline void referencepointer<REF>::operator =(REF* i_object)
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

	template <class REF>
	inline REF* referencepointer<REF>::operator *()
	{
		return m_ptr;
	}

	template <class REF>
	inline referencepointer<REF>::~referencepointer()
	{
		if (m_ptr)
		{
			m_ptr->remove_ref();

			if (!m_ptr->get_ref())
				delete m_ptr;

			m_ptr=NULL;
		}
	}
}
#endif//_referencepointer_h_