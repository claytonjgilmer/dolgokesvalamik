#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#include "taskmanager2.h"
#include <Windows.h>

namespace threading
{
	DEFINE_SINGLETON(taskmanager);

	unsigned WINAPI poolrun(void* i_param)
	{
		taskmanager* tm=(taskmanager*)i_param;
		tm->run();
		return 0;
	}

	taskmanager::taskmanager(const taskmanagerdesc* i_desc)
	{
		m_incompletetasknum=0;

		m_workevent= CreateSemaphore(NULL,0,1000,NULL);
		m_exitevent= CreateEvent(NULL,TRUE,FALSE,NULL);

		for (unsigned n=0; n<REF_COUNT;++n)
		{
			m_ref_index.push(n);
			m_ref_event[n]=CreateEvent(NULL,TRUE,FALSE,NULL);
#ifdef _DEBUG
			m_threadid[n]=-100;
#endif
		}

		ResetEvent(m_exitevent);

		if (i_desc->m_threadnum)
			m_threadbuf.resize(i_desc->m_threadnum);

		for (unsigned n=0; n<m_threadbuf.size();++n)
		{
			m_threadbuf[n].start(&poolrun,this);

			char str[256]; sprintf(str,"working thread #%d",n+1);
			m_threadbuf[n].set_name(ctr::string(str));
		}

	}

	taskmanager::~taskmanager()
	{
		exit();
	}

	void taskmanager::flush()
	{
		while (m_incompletetasknum) SwitchToThread();
		m_taskbuf.clear();
	}

	void taskmanager::exit()
	{
		if (m_threadbuf.size())
		{
			SetEvent(m_exitevent);

			for (unsigned n=0; n<m_threadbuf.size(); ++n)
				m_threadbuf[n].join();
		}
		CloseHandle(m_exitevent);
		CloseHandle(m_workevent);
	}

	void taskmanager::run()
	{
		for(;;)
		{
			int exit=wait_for_task_or_exit();

			if (exit)
				return;

			task* t=get_task();

			t->run();
			post_process(t);
		}
	}

	int taskmanager::wait_for_task_or_exit()
	{
		const HANDLE waitFor[]={m_workevent,m_exitevent};
		return WaitForMultipleObjects(2,waitFor,FALSE,INFINITE) - WAIT_OBJECT_0;
	}

	task* taskmanager::get_task()
	{
		m_taskmutex.lock();
		task* t=m_taskbuf.front();
		m_taskbuf.pop();
		m_taskmutex.unlock();

		return t;
	}

	void taskmanager::post_process(task* i_task)
	{
		m_allocator.deallocate(i_task);
		m_taskmutex.lock();
		--m_incompletetasknum;
		--m_ref_buf[i_task->m_ref_index];

		if (!m_ref_buf[i_task->m_ref_index])
		{
#ifdef _DEBUG
			m_threadid[i_task->m_ref_index]=-100;
#endif
			SetEvent(m_ref_event[i_task->m_ref_index]);
		}

		m_taskmutex.unlock();
	}

	unsigned stack_min_size=INT_MAX;


	void taskmanager::spawn_task(task* i_task)
	{

		unsigned ref_index=m_ref_index.pop();

#ifdef _DEBUG
		unsigned s=m_ref_index.size();
		if (s<stack_min_size)
			s=stack_min_size;
#endif

		m_taskmutex.lock();
		m_ref_buf[ref_index]=1;

		i_task->m_ref_index=ref_index;
		m_taskbuf.push(i_task);
		++m_incompletetasknum;
		m_taskmutex.unlock();

		ReleaseSemaphore(m_workevent,1,NULL);


		for(;;)
		{
			const HANDLE waitFor[]={m_ref_event[ref_index],m_workevent};
			unsigned ret=WaitForMultipleObjects(2,waitFor,FALSE,INFINITE) - WAIT_OBJECT_0;

			if (!ret)
			{
				ResetEvent(m_ref_event[ref_index]);
				m_ref_index.push(ref_index);
				return;
			}

			task* t=get_task();

			t->run();
			post_process(t);
		}
	}

	void taskmanager::spawn_tasks(task* i_tasks[], unsigned i_tasknum)
	{
		m_taskmutex.lock();
		unsigned ref_index=m_ref_index.pop();

#ifdef _DEBUG
		unsigned s=m_ref_index.size();
		if (s<stack_min_size)
			s=stack_min_size;

		utils::assertion(m_threadid[ref_index]==-100);
		m_threadid[ref_index]=GetCurrentThreadId();
#endif

		m_ref_buf[ref_index]=i_tasknum;
		m_incompletetasknum+=i_tasknum;

		for (unsigned n=0; n<i_tasknum; ++n)
		{
			i_tasks[n]->m_ref_index=ref_index;
			m_taskbuf.push(i_tasks[n]);
		}
		ReleaseSemaphore(m_workevent,i_tasknum,NULL);
		m_taskmutex.unlock();

		for(;;)
		{
			const HANDLE waitFor[]={m_ref_event[ref_index],m_workevent};
			unsigned ret=WaitForMultipleObjects(2,waitFor,FALSE,INFINITE) - WAIT_OBJECT_0;

			if (!ret)
			{
				ResetEvent(m_ref_event[ref_index]);
				m_ref_index.push(ref_index);
				return;
			}

			task* t=get_task();

			t->run();
			post_process(t);
		}
	}
}