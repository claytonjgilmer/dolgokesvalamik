#include "task.h"
#include "taskmanager2.h"

namespace threading
{
	task::task()
	{
	}
	
	int task::get_id() const
	{
		return m_ref_index;
	}

	void* task::operator new(size_t i_sixe)
	{
		return threading::taskmanager::instance()->get_allocator().allocate();
	}

}