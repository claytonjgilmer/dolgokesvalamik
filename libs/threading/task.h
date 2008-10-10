#ifndef _task_h_
#define _task_h_

#include "containers/string.h"
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

			void* operator new(size_t i_sixe);

			unsigned m_ref_index;

#ifdef _DEBUG
			ctr::string m_name;
#endif
	};
};
#endif//_task_h_