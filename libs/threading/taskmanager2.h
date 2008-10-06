#ifndef _taskmanager2_h_
#define _taskmanager2_h_

#include "mutex.h"
#include "thread.h"
#include "task.h"
#include "taskallocator.h"
#include "containers/fixedvector.h"
#include "containers/queue.h"
#include "math/math.h"
#include "utils/singleton.h"

namespace threading
{
	enum taskstate
	{
		TASK_DEPENDENT,
		TASK_IDLE,
		TASK_WORKING,
		TASK_COMPLETED
	};

	class taskdescinternal
	{
	public:
		taskdescinternal(task* i_task):
		m_task=i_task;
		{
		}

		task* m_task;
	};

/************************************************************************/
/*                taskmanager                                           */
/************************************************************************/

	class taskmanagerdesc
	{
	public:
		unsigned m_threadnum;
		taskmanagerdesc(): m_threadnum(4){}
	};

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
		void spawn_tasks(task** i_tasks, unsigned i_tasknum);


		taskallocator& get_allocator()
		{
			return m_allocator;
		}

		template<class T,class S> unsigned process_buffer(T* i_buf, unsigned i_elemnum, unsigned i_grainsize, const S& i_process)
		{
			class proc_task:public task
			{
			public:
				proc_task(unsigned i_threadnum, T* i_buf, unsigned i_elemnum, unsigned i_grainsize, const S& i_process, taskmanager* i_tm):
				m_threadnum(i_threadnum),
				m_buf(i_buf),
				m_elemnum(i_elemnum),
				m_grainsize(i_grainsize),
				m_process(i_process),
				m_tm(i_tm)
				{
				}

				virtual void run()
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
					unsigned elemnumpertask=math::Max((unsigned)(m_elemnum/m_threadnum),(unsigned)m_grainsize);

					proc_range* tasks[20];
					unsigned tnum=0;

					while (start<m_elemnum)
					{
						unsigned actnum=math::Min(elemnumpertask,m_elemnum-start);

						tasks[tnum++]=new(m_tm->get_allocator()) proc_range(m_buf+start,actnum,m_process);
						start+=actnum;
					}

					m_tm->spawn_tasks(tasks,tnum);
				}
			private:
				const unsigned m_threadnum;
				T* const m_buf;
				const unsigned m_elemnum;
				const unsigned m_grainsize;
				const S m_process;
				taskmanager* m_tm;
			};

			proc_task* t=new (m_allocator) proc_task(m_threadbuf.size(),i_buf,i_elemnum,i_grainsize,i_process,this);
			return spawn_task(t);
		}

	private:
		ctr::vector<thread> m_threadbuf;
		taskallocator m_allocator;

		ctr::fixedvector<taskdescinternal,128> m_taskbuf;
		volatile unsigned m_incompletetasknum;

		mutex m_taskmutex;

		HANDLE m_workevent;
		HANDLE m_exitevent;


		void run();
		int wait_for_task_or_exit();
		task* get_task();
		void post_process(task*);
	};
}
#endif//_taskmanager2_h_