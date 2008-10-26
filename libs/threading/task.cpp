#include "task.h"
#include "taskmanager2.h"

	task::task()
	{
	}
	
	int task::get_id() const
	{
		return m_ref_index;
	}

	void* task::operator new(size_t i_sixe)
	{
		return taskmanager::ptr()->get_allocator().allocate();
	}
