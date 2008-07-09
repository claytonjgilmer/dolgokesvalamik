#ifndef _referencedobject_h_
#define _referencedobject_h_

#include "utils/referencepointer.h"
#include "utils/misc.h"

namespace utils
{

	class referencedobject
	{
		template <class T> friend class referencepointer;
	public:
		int get_ref() const;
	protected:
		referencedobject();
		virtual ~referencedobject(){}

	private:
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
}
#endif//_referencedobject_h_