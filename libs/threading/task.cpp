#include "task.h"
#include "taskmanager2.h"

	task_t::task_t()
	{
	}
	
	int task_t::get_id() const
	{
		return m_ref_index;
	}

	void* task_t::operator new(size_t i_size)
	{
		return taskmanager::ptr()->get_allocator().allocate();
	}
