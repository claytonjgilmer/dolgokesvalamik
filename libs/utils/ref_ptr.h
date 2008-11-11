#ifndef _ref_ptr_h_
#define _ref_ptr_h_

#include "referencedobject.h"
#include "utils/misc.h"

	template <class REF>
	class ref_ptr
	{
	public:
		ref_ptr();
		ref_ptr(const ref_ptr& i_other);
		ref_ptr(REF* i_object);

		void operator=(const ref_ptr& i_other);
		void operator=(REF* i_object);

		const REF* get() const
		{
			return m_ptr;
		}

		REF* get()
		{
			return m_ptr;
		}

		REF& operator*()
		{
			return *m_ptr;
		}

		const REF& operator*() const
		{
			return *m_ptr;
		}

		REF* operator->()
		{
			return m_ptr;
		}

		const REF* operator->() const
		{
			return m_ptr;
		}

		~ref_ptr();
	//private:
		REF* m_ptr;
	};

	template <class REF>
	inline ref_ptr<REF>::ref_ptr()
	{
		m_ptr=0;
	}

	template <class REF>
	inline ref_ptr<REF>::ref_ptr(const ref_ptr& i_other)
	{
		m_ptr=i_other.m_ptr;

		if (m_ptr)
			m_ptr->add_ref();
	}

	template <class REF>
	inline ref_ptr<REF>::ref_ptr(REF* i_object)
	{
		m_ptr=i_object;

		if (m_ptr)
			((referencedobject*)m_ptr)->add_ref();
	}

	template <class REF>
	inline void ref_ptr<REF>::operator =(const ref_ptr& i_other)
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
	inline void ref_ptr<REF>::operator =(REF* i_object)
	{
		if (m_ptr)
		{
			m_ptr->remove_ref();

			if (!m_ptr->m_refcount)
				delete (referencedobject*)m_ptr;
		}

		m_ptr=i_object;

		if (m_ptr)
			m_ptr->add_ref();
	}
/*
	template <class REF>
	inline REF& ref_ptr<REF>::operator *()
	{
		return *m_ptr;
	}

	template <class REF>
	inline const REF& ref_ptr<REF>::operator *() const
	{
		return m_ptr;
	}
*/
	template <class REF>
	inline ref_ptr<REF>::~ref_ptr()
	{
		if (m_ptr)
		{
			m_ptr->remove_ref();

			if (!m_ptr->get_ref())
				delete ((referencedobject*)m_ptr);

			m_ptr=NULL;
		}
	}
#endif//_ref_ptr_h_
