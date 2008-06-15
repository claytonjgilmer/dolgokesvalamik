#include "task.h"

namespace threading
{
	task::task()
	{
	}
	
	int task::get_id() const
	{
		return m_taskID;
	}
}