#include "task.h"

namespace threading
{
	task::task()
	{
	}
	
	int task::getID() const
	{
		return m_taskID;
	}
	
	
	taskdesc::taskdesc()
	{
		m_task=NULL;
		m_parentID=0;
	}
}