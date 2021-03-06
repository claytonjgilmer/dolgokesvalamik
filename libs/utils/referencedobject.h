#ifndef _referencedobject_h_
#define _referencedobject_h_

//#include "utils/ref_ptr.h"
#include "utils/misc.h"
#include "utils/assert.h"

//    template <typename T> struct ref_ptr;
	struct referencedobject
	{
		int get_ref() const;


		referencedobject();
		virtual ~referencedobject()
		{
			assertion(m_refcount==0,"referencedobject refcountja nem nulla!");
		}

		void add_ref();
		void remove_ref();
		int m_refcount;
	};

	MLINLINE int referencedobject::get_ref() const
	{
		return m_refcount;
	}

	inline referencedobject::referencedobject()
	{
		m_refcount=0;
	}

	inline void referencedobject::add_ref()
	{
		++m_refcount;
	}

	inline void referencedobject::remove_ref()
	{
		--m_refcount;
	}
#endif//_referencedobject_h_
