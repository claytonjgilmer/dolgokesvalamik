#ifndef _taskallocator_h_
#define _taskallocator_h_

#include <assert.h>
#include "containers/listallocator.h"


	class taskallocator
	{
	public:
		void* allocate()
		{
			m.lock();
			void* r=(void*)m_taskalloc.allocate_place();
			m.unlock();
			return r;
		}


		void deallocate(void* i_buf)
		{
			m.lock();
			m_taskalloc.deallocate((tplaceholder*)i_buf);
			m.unlock();
		}
	private:
		class tplaceholder{char c[256];};
		list_allocator<tplaceholder> m_taskalloc;
		mutex m;
	};

inline void* operator new(size_t i_size, taskallocator& i_alloc)
{
	assert(i_size<=256);

	return i_alloc.allocate();
}

inline void operator delete(void* i_task, taskallocator& i_alloc)
{
	i_alloc.deallocate(i_task);
}
#endif//_taskallocator_h_
