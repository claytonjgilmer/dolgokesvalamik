#ifndef _task_h_
#define _task_h_

#include <string>
#include "containers/vector.h"

namespace threading
{
	class taskmanager;
	class task
	{
		friend class taskmanager;
		public:
			task();
			virtual void run() =0;
			int get_id() const;

		private:
			int m_taskID;
#ifdef _DEBUG
			std::string m_name;
#endif
	};

	class empty_task:public task
	{
	public:
		virtual void run(){}
	};
	
};
#endif//_task_h_