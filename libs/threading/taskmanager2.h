#ifndef _taskmanager2_h_
#define _taskmanager2_h_

#define _WIN32_WINNT 0x0502
#define task_pool_size 65536
#include "utils/scratchpad.h"
#include "thread.h"
#include "containers/lockfreequeue.h"
#include "containers/vector.h"
#include "utils/singleton.h"

struct task2_t
{
	task2_t()
	{
//		nextsk=0;
	}

	virtual void run() =0;
	void* operator new(size_t i_size);
	void* operator new(size_t i_size, void* where){return where;}
	void* operator new[](size_t i_size);
	void* operator new[](size_t i_size, void* where){return where;}

	void* ref_index;
	task2_t* next;
};

struct taskmanagerdesc
{
public:
	unsigned m_threadnum;
	taskmanagerdesc(): m_threadnum(1){}
};


struct taskmanager2_t
{
	DECLARE_SINGLETON_DESC(taskmanager2_t,taskmanagerdesc);
	taskmanager2_t(const taskmanagerdesc*);
//	taskmanager2_t(int thread_num);
	~taskmanager2_t();

	void spawn_tasks(task2_t* task_array[], int task_count);
	void flush();
	void exit();

	template <typename S>
	struct proc_range:task2_t
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

			spawn_tasks((task2_t**)tasks,tnum);
		}



//	static taskmanager2_t* ptr;

	void run();
	task2_t* get_task();
	void post_process(task2_t*);

	void* alloc_task(uint32 size)
	{
		return task_buf.alloc(size);
	}

	int exit_event;
	long incomplete_task_count;


//	lockfree_queue_t<task2_t> pending_queue;
	lockfree_array_queue_t<task2_t,512> pending_queue;
	vector<thread> thread_array;

	scratch_pad_t<task_pool_size> task_buf;
};

MLINLINE void* task2_t::operator new(size_t size)
{
	return taskmanager2_t::ptr->alloc_task(size);
}

MLINLINE void* task2_t::operator new[](size_t size)
{
	return taskmanager2_t::ptr->alloc_task(size);
}
#endif//_taskmanager2_h_