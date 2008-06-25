#ifndef _referencedobject_h_
#define _referencedobject_h_

namespace base
{
	class referencepointer;
	class referencedobject
	{
		friend class referencepointer;
	public:
		referencedobject();
	private:
		virtual ~referencedobject(){}
		int m_refcount;

		void add_ref();
		void remove_ref();
	};


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