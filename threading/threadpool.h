#ifndef _threadpool_h_
#define _threadpool_h_

#include "mutex.h"
#include "..\containers\vector.h"
#include "..\containers\queue.h"

namespace threading
{
	typedef int taskid;

	class task
	{
	}; //class task

	typedef void (task::*taskfunction)(void*, taskid);

	class taskdesc
	{
	public:
		task* m_task;
		taskfunction m_taskfn;
		void*		m_data;
		taskid		m_parenttask;
		ctr::vector<int> m_dependency;

		taskdesc()
		{
			m_task=NULL;
			m_taskfn=NULL;
			m_parenttask=-1;
			m_data=NULL;
		}
	};

/*
task bejon: kap egy id-t, ami egy index egy vectorban, kikeresi a fuggosegeket, ha letezik akkor beteszi oda magat, a sajat dependencynumjat noveli eggyel
ha a dependencynumja nem 0, akkor a semaphore megno eggyel
*/

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
		taskdescinternal()
		{
			m_task=NULL;
			m_func=NULL;
			m_dependentlist.reserve(20);
			m_dependencycounter=0;
			m_state=TASK_IDLE;
			m_runningchildnum=0;
			m_data=NULL;
		}

		task* m_task;
		taskfunction m_func;
		void* m_data;
		ctr::vector<int> m_dependentlist;
		int m_dependencycounter;
		int m_runningchildnum;
		taskid m_parenttask;
		taskstate m_state;
	};


	class threadpool
	{
	public:
		threadpool(int i_numproc);
		~threadpool();
		void flush();
		taskid addtask(const taskdesc& i_desc);
		void addtasks(taskid* o_taskid, const taskdesc* const i_desc, int i_tasknum);

		static unsigned WINAPI poolrun(void* i_param);

		unsigned getnumproc() const
		{
			return m_numproc;
		}

		void setnumthreads(unsigned i_numthreads);

	protected:
		void run();

		int gettask(unsigned& o_taskindex);


		ctr::vector<HANDLE> m_hwthread;
		unsigned m_numproc;
		unsigned m_threadid;
		ctr::vector<taskdescinternal> m_taskbuf;
		unsigned m_tasknum;
		ctr::queue<unsigned,100> m_idletask;

		mutex m_taskmutex;

		HANDLE m_workevent;
		HANDLE m_exitevent;
	}; //class threadpool

} //namespace threading

#endif//_dynthreadpool_h_