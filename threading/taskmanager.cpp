#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#include "taskmanager.h"
//#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
//#endif						
#include <Windows.h>

namespace threading
{
	unsigned WINAPI poolrun(void* i_param)
	{
		taskmanager* tm=(taskmanager*)i_param;

		tm->run();

		return 0;
	}

	taskmanager::taskmanager(unsigned i_threadnum/*=0*/)
	{
		m_incompletetasknum=0;
//		m_taskbuf.reserve(128);

		m_workevent= CreateSemaphore(NULL,0,1000,NULL);
		m_exitevent= CreateEvent(NULL,TRUE,FALSE,NULL);

		ResetEvent(m_exitevent);

		if (i_threadnum)
			m_threadbuf.resize(i_threadnum);

		for (unsigned n=0; n<m_threadbuf.size();++n)
		{
			m_threadbuf[n].start(&poolrun,this);
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
			{
				m_threadbuf[n].join();
			}
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
		HANDLE waitFor[]={m_exitevent,m_workevent};
		DWORD ret=WaitForMultipleObjects(2,waitFor,FALSE,INFINITE) - WAIT_OBJECT_0;

		return 1-ret;//WaitForMultipleObjects(2,waitFor,FALSE,INFINITE) - WAIT_OBJECT_0;
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

	unsigned taskmanager::spawn_task(task* i_task, unsigned i_parentID/* =-1 */, const ctr::fixedvector<unsigned,10>& i_dependency/* =ctr::fixedvector<unsigned>::emptyvector */)
	{
		m_taskmutex.lock();

		unsigned taskindex=m_taskbuf.size();
//		printf_s("%d. taszk akar spawnolni\n",taskindex);
		m_taskbuf.resize(taskindex+1);

		taskdescinternal& newtaskdesc=m_taskbuf.back();

		newtaskdesc.m_parenttask=i_parentID;
		newtaskdesc.m_task=i_task;

		if (i_parentID!=-1)
			m_taskbuf[i_parentID].m_childnum++;
		
		for (unsigned int n=0; n<i_dependency.size();++n)
		{
			taskdescinternal& td=m_taskbuf[i_dependency[n]];

			if (td.m_state==TASK_COMPLETED)
				continue;

			td.m_dependentlist.push_back(taskindex);
			++newtaskdesc.m_dependencycounter;
		}

		if (!newtaskdesc.m_dependencycounter)
		{
			m_idletask.push(taskindex);
			newtaskdesc.m_state=TASK_IDLE;
			ReleaseSemaphore(m_workevent,1,NULL);
		}
		else
		{
			newtaskdesc.m_state=TASK_DEPENDENT;
		}

		++m_incompletetasknum;
		i_task->m_taskID=taskindex;
		m_taskmutex.unlock();
		return taskindex;
	}
	unsigned taskmanager::spawn_task(task* i_task, unsigned i_parentID/* =-1 */, unsigned i_dependency/*=-1*/)
	{
		m_taskmutex.lock();

		unsigned taskindex=m_taskbuf.size();
		m_taskbuf.resize(taskindex+1);

		taskdescinternal& newtaskdesc=m_taskbuf.back();

		newtaskdesc.m_parenttask=i_parentID;
		newtaskdesc.m_task=i_task;

		if (i_parentID!=-1)
			m_taskbuf[i_parentID].m_childnum++;

		if (i_dependency!=-1)
		{
			taskdescinternal& td=m_taskbuf[i_dependency];

			if (td.m_state!=TASK_COMPLETED)
			{
				td.m_dependentlist.push_back(taskindex);
				++newtaskdesc.m_dependencycounter;
			}
		}

		if (!newtaskdesc.m_dependencycounter)
		{
			m_idletask.push(taskindex);
			newtaskdesc.m_state=TASK_IDLE;
			ReleaseSemaphore(m_workevent,1,NULL);
		}
		else
		{
			newtaskdesc.m_state=TASK_DEPENDENT;
		}

		++m_incompletetasknum;
		i_task->m_taskID=taskindex;
		m_taskmutex.unlock();
		return taskindex;
	}
}