#ifndef _referencedobject_h_
#define _referencedobject_h_

#include "base/referencepointer.h"
#include "base/misc.h"

namespace base
{

	class referencedobject
	{
	public:
		void add_ref();
		void remove_ref();
		int get_ref() const;
	protected:
		referencedobject();
		virtual ~referencedobject(){}

	private:
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
}
#endif//_referencedobject_h_