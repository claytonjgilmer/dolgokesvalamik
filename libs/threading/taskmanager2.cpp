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
		while (true)
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
		task* t=m_taskbuf[m_idletask.front()].m_task;
		m_taskbuf[m_idletask.front()].m_state=TASK_WORKING;
		m_idletask.pop();
		m_taskmutex.unlock();

		return t;
	}

	void taskmanager::post_process(task* i_task)
	{
		m_allocator.deallocate(i_task);

		m_taskmutex.lock();

		unsigned taskindex=i_task->m_taskID;
		m_taskbuf[taskindex].m_state=TASK_COMPLETED;

		while (true)
		{
			if (m_taskbuf[taskindex].m_childnum)
				break;

			//ha nincs gyereke, indulhatnak a dependensek
			for (unsigned int n=0; n<m_taskbuf[taskindex].m_dependentlist.size();++n)
			{
				m_idletask.push(m_taskbuf[taskindex].m_dependentlist[n]);
				m_taskbuf[taskindex].m_state=TASK_IDLE;
				ReleaseSemaphore(m_workevent,1,NULL);
			}

			taskindex=m_taskbuf[taskindex].m_parenttask;

			if (taskindex==-1)
				break;

			--m_taskbuf[taskindex].m_childnum;
		}

		--m_incompletetasknum;
		m_taskmutex.unlock();
	}

	void taskmanager::spawn_task(task* i_task)
	{
		m_taskmutex.lock();

		unsigned taskindex=m_taskbuf.size();
		m_taskbuf.push_back(taskdescinternal(i_task));
		taskdescinternal& newtaskdesc=m_taskbuf.back();

		ReleaseSemaphore(m_workevent,1,NULL);
		++m_incompletetasknum;
		i_task->m_taskID=taskindex;
		m_taskmutex.unlock();
	}

	void taskmanager::spawn_tasks(task** i_tasks, unsigned i_tasknum)
	{
		m_taskmutex.lock();
		m_taskbuf.resize(m_taskbuf.size()+i_tasknum);

		for (unsigned n=0; n<i_tasknum; ++n)
		{
			m_taskbuf.push_back(taskdescinternal(i_tasks[n]));
			++m_incompletetasknum;
		}

		i_task->m_taskID=taskindex;
		m_taskmutex.unlock();
		return taskindex;
	}
}