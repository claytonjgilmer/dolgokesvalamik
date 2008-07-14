#include "task.h"
#include "taskmanager.h"

namespace threading
{
	task::task()
	{
	}
	
	int task::get_id() const
	{
		return m_taskID;
	}

	void* task::operator new(size_t i_sixe)
	{
		return threading::taskmanager::instance()->get_allocator().allocate();
	}

}