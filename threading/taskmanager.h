#ifndef _taskmanager_h_
#define _taskmanager_h_

#include "thread.h"
#include "task.h"

namespace threading
{
	class taskmanager
	{
	public:
		taskmanager(unsigned i_threadnum=-1);
		void spawntask(const taskdesc& i_desc);
		
		
	private:
		ctr::vector<thread> m_threadbuf;
	};
}
#endif//_taskmanager_h_