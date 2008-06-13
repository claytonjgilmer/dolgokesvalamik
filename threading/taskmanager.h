#ifndef _taskmanager_h_
#define _taskmanager_h_

#include "task.h"

namespace threading
{
	class taskmanager
	{
		taskmanager();
		void spawntask(const taskdesc& i_desc);
	};
}
#endif//_taskmanager_h_