#ifndef _task_h_
#define _task_h_

#include "containers/string.h"

	struct task_t
	{
		task_t();
		virtual void run() =0;
		int get_id() const;

		void* operator new(size_t i_size);

		unsigned m_ref_index;

#ifdef _DEBUG
		string m_name;
#endif
	};
#endif//_task_h_