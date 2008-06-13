#ifndef _task_h_
#define _task_h_

#include <string>
#include "containers/vector.h"

namespace threading
{
	class task
	{
		public:
			task();
			virtual void run() =0;
			int getID() const;
		private:
			int m_taskID;
#ifdef _DEBUG
			std::string m_name;
#endif
	};
	
	class taskdesc
	{
		task* m_task;
		int m_parentID;
		ctr::vector<int> m_dependency;
			
		taskdesc();
	};
	

};
#endif//_task_h_