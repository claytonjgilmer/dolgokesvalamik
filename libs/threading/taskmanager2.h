#ifndef _taskmanager2_h_
#define _taskmanager2_h_

#include "mutex.h"
#include "thread.h"
#include "task.h"
#include "taskallocator.h"
#include "containers/fixedvector.h"
#include "containers/queue.h"
#include "containers/stack.h"
#include "math/math.h"
#include "utils/singleton.h"

namespace threading
{
/************************************************************************/
/*                taskmanager                                           */
/************************************************************************/

	class taskmanagerdesc
	{
	public:
		unsigned m_threadnum;
		taskmanagerdesc(): m_threadnum(3){}
	};

	class tmimpl;

	class taskmanager
	{
		DECLARE_SINGLETON_DESC(taskmanager,taskmanagerdesc);
	public:
		friend unsigned WINAPI poolrun(void*);
		taskmanager(const taskmanagerdesc*);
		~taskmanager();
		void flush();
		void exit();
		void spawn_task(task* i_task);
		void spawn_tasks(task* i_tasks[], unsigned i_tasknum);


		taskallocator& get_allocator()
		{
			return m_allocator;
		}

		template<class T,class S> void process_buffer(T* i_buf, unsigned i_elemnum, unsigned i_grainsize, const S& i_process)
		{
			class proc_range:public task
			{
			public:
				proc_range(T* i_buf, unsigned i_num, const S& i_process):
				  m_buf(i_buf),
					  m_num(i_num),
					  m_process(i_process)
						{
						}

				  void run()
						{
							m_process(m_buf,m_num);
						}

			private:
				T* const m_buf;
				unsigned m_num;
				const S& m_process;
			};

			unsigned start=0;
			unsigned elemnumpertask=math::Max((unsigned)(i_elemnum/(m_threadbuf.size()+5)),(unsigned)i_grainsize);

			proc_range* tasks[20];
			unsigned tnum=0;

			while (start<i_elemnum)
			{
				unsigned actnum=math::Min(elemnumpertask,i_elemnum-start);

				tasks[tnum++]=new proc_range(i_buf+start,actnum,i_process);
				start+=actnum;
			}

			spawn_tasks((task**)tasks,tnum);
		}

	private:
#define REF_COUNT 100
		unsigned m_ref_buf[REF_COUNT];
		HANDLE	m_ref_event[REF_COUNT];

#ifdef _DEBUG
		int m_threadid[REF_COUNT];
#endif//_DEBUG
		ctr::stack<unsigned,128> m_ref_index;
		ctr::vector<thread> m_threadbuf;
		taskallocator m_allocator;

		ctr::queue<task*,128> m_taskbuf;
		volatile unsigned m_incompletetasknum;

		mutex m_taskmutex;

		HANDLE m_workevent;
		HANDLE m_exitevent;


		void run();
		int wait_for_task_or_exit();
		task* get_task();
		void post_process(task* i_task);
	};
}
#endif//_taskmanager2_h_