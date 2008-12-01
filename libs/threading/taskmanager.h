#ifndef _taskmanager_h_
#define _taskmanager_h_

#include <malloc.h>
#include "mutex.h"
#include "thread.h"
#include "task.h"
#include "taskallocator.h"
#include "containers/fixedvector.h"
#include "containers/queue.h"
//#include "containers/stack.h"
#include "math/math.h"
#include "utils/singleton.h"

/************************************************************************/
/*                taskmanager                                           */
/************************************************************************/

	struct taskmanagerdesc
	{
	public:
		unsigned m_threadnum;
		taskmanagerdesc(): m_threadnum(1){}
	};

	struct taskmanager
	{
		DECLARE_SINGLETON_DESC(taskmanager,taskmanagerdesc);
		taskmanager(const taskmanagerdesc*);
		~taskmanager();

		void flush();
		void exit();
		void spawn_task(task_t* i_task);
		void spawn_tasks(task_t* i_tasks[], unsigned i_tasknum);


		taskallocator& get_allocator()
		{
			return m_allocator;
		}

		template<class T,class S> void process_buffer(T* i_buf, unsigned i_elemnum, unsigned i_grainsize, S i_process)
		{
			class proc_range:public task_t
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
				S m_process;
			};

			unsigned start=0;
			unsigned elemnumpertask=max((unsigned)(i_elemnum/(m_threadbuf.size()+5)),(unsigned)i_grainsize);

			unsigned tnum=0;
			const unsigned n=i_elemnum/elemnumpertask+1;
//			proc_range** tasks=(proc_range**)alloca(n*sizeof(proc_range*));
			proc_range* tasks[n];

			while (start<i_elemnum)
			{
				unsigned actnum=min(elemnumpertask,i_elemnum-start);

				tasks[tnum++]=new proc_range(i_buf+start,actnum,i_process);
				start+=actnum;
			}

			assertion(tnum>0 && tnum<=n);

			spawn_tasks((task_t**)tasks,tnum);
		}

		template <typename S>
		class proc_range:public task_t
		{
		public:
			proc_range(unsigned i_startindex, unsigned i_num, const S& i_process):
			  m_startindex(i_startindex),
				  m_num(i_num),
				  m_process(i_process)
			  {
			  }

			  void run()
			  {
				  m_process(m_startindex,m_num);
			  }

		private:
			unsigned m_startindex;
			unsigned m_num;
			S m_process;
		};

		template<typename S> void process_buffer(unsigned i_elemnum, unsigned i_grainsize, S i_process)
		{

			unsigned start=0;
			unsigned elemnumpertask=max((unsigned)(i_elemnum/(m_threadbuf.size()+5)),(unsigned)i_grainsize);

			unsigned tnum=0;
			const unsigned n=i_elemnum/elemnumpertask+1;
//			proc_range<S>** tasks=(proc_range<S>**)alloca(n*sizeof(proc_range<S>*));
			proc_range<S>* tasks[n];

			while (start<i_elemnum)
			{
				unsigned actnum=min(elemnumpertask,i_elemnum-start);

				tasks[tnum++]=new proc_range<S>(start,actnum,i_process);
				start+=actnum;
			}

			assertion(tnum>0 && tnum<=n);

			spawn_tasks((task_t**)tasks,tnum);
		}






#define REF_COUNT 64
		unsigned m_ref_buf[REF_COUNT];
		HANDLE	m_ref_event[REF_COUNT];

//		stack<unsigned,REF_COUNT> m_ref_index;
		queue<unsigned,REF_COUNT> m_ref_index;
		vector<thread> m_threadbuf;
		taskallocator m_allocator;

		queue<task_t*,16384> m_taskbuf;
		volatile unsigned m_incompletetasknum;

		HANDLE m_workevent;
		HANDLE m_exitevent;

		void run();
		int wait_for_task_or_exit();
		task_t* get_task();
		void post_process(task_t* i_task);
	};
#endif//_taskmanager_h_
