#ifndef _taskmanager_h_
#define _taskmanager_h_

#define task_pool_size 65536
#include "utils/scratchpad.h"
#include "thread.h"
#include "containers/lockfreequeue.h"
#include "containers/vector.h"
#include "utils/singleton.h"

struct task_t
{
	virtual void run() =0;
	void* operator new(size_t i_size);
	void* operator new(size_t i_size, void* where){return where;}
	void* operator new[](size_t i_size);
	void* operator new[](size_t i_size, void* where){return where;}

	void* ref_index;
	task_t* next;
};

struct taskmanagerdesc_t
{
	unsigned m_threadnum;
	taskmanagerdesc_t(): m_threadnum(1){}
};


struct taskmanager_t
{
	DECLARE_SINGLETON_DESC(taskmanager_t,taskmanagerdesc_t);
	taskmanager_t(const taskmanagerdesc_t*);
//	taskmanager_t(int thread_num);
	~taskmanager_t();

	void spawn_tasks(task_t* task_array[], int task_count);
	void flush();
	void exit();

	template <typename S>
	struct proc_range:task_t
	{
		proc_range(unsigned i_startindex, unsigned i_num, const S& i_process):
		m_startindex(i_startindex),
		m_num(i_num),
		m_process(i_process)
		{
		}

		void set(unsigned i_startindex, unsigned i_num, const S& i_process)
		{
			m_startindex=i_startindex;
			m_num=i_num;
			m_process=i_process;
		}

		void run()
		{
		  m_process(m_startindex,m_num);
		}

		unsigned m_startindex;
		unsigned m_num;
		S m_process;
	};


		template<typename S> void process_buffer(unsigned i_elemnum, unsigned i_grainsize, S i_process)
		{

			unsigned start=0;
			unsigned elemnumpertask=max((unsigned)(i_elemnum/(thread_array.size()+1)),(unsigned)i_grainsize);

			unsigned tnum=0;
			const unsigned n=i_elemnum/elemnumpertask+1;
#ifdef _MSC_VER
			proc_range<S>* tasks[100];//=(proc_range<S>**)alloca(n*sizeof(proc_range<S>*));
#else
			proc_range<S>* tasks[n];
#endif
			proc_range<S>* taskbuf=(proc_range<S>*)this->alloc_task(n*sizeof(proc_range<S>));// new proc_range<S>[n];

			while (start<i_elemnum)
			{
				unsigned actnum=min(elemnumpertask,i_elemnum-start);

				tasks[tnum]=taskbuf+tnum;
				new ((void*)tasks[tnum++]) proc_range<S>(start,actnum,i_process);
				start+=actnum;
			}

			assertion(tnum>0 && tnum<=n);

			spawn_tasks((task_t**)tasks,tnum);
		}



//	static taskmanager_t* ptr;

	void run();
	task_t* get_task();
	void post_process(task_t*);

	void* alloc_task(uint32 size)
	{
		return task_buf.alloc(size);
	}

	int exit_event;
	long incomplete_task_count;


//	lockfree_queue_t<task2_t> pending_queue;
	lockfree_array_queue_t<task_t,512> pending_queue;
	vector<thread> thread_array;

	scratch_pad_t<task_pool_size> task_buf;
};

MLINLINE void* task_t::operator new(size_t size)
{
	return taskmanager_t::ptr->alloc_task(size);
}

MLINLINE void* task_t::operator new[](size_t size)
{
	return taskmanager_t::ptr->alloc_task(size);
}
#endif//_taskmanager_h_