#include "threadpool.h"
#include <process.h>
#include <stdio.h>
#include <Windows.h>

#ifdef WIN32
//#include <intrin.h>
#endif

#define  PRINT

namespace threading
{
#ifdef _DEBUG
#define MS_VC_EXCEPTION 0x406D1388
#pragma pack(push,8)
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType; // Must be 0x1000.
		LPCSTR szName; // Pointer to name (in user addr space).
		DWORD dwThreadID; // Thread ID (-1=caller thread).
		DWORD dwFlags; // Reserved for future use, must be zero.
	} THREADNAME_INFO;
#pragma pack(pop)

	void SetThreadName( DWORD dwThreadID, char* threadName)
	{
		Sleep(1);
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = threadName;
		info.dwThreadID = dwThreadID;
		info.dwFlags = 0;

		__try
		{
			RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
#endif

	threadpool::threadpool(int i_NumProc)
	{
		m_workevent= CreateSemaphore(NULL,0,1000,NULL);
		m_exitevent= CreateEvent(NULL,TRUE,FALSE,NULL);

		m_numproc=0;
		setnumthreads(i_NumProc);

		m_taskbuf.reserve(100);
		m_tasknum=0;
	}

	void threadpool::setnumthreads(unsigned i_NumProc)
	{
		if (m_numproc)
		{
			SetEvent(m_exitevent);
			WaitForMultipleObjects(m_hwthread.size(),&m_hwthread[0],TRUE,INFINITE);
			ResetEvent(m_exitevent);

			m_hwthread.resize(0);
		}


		m_numproc=i_NumProc;

		if (m_numproc)
		{
			m_hwthread.resize(i_NumProc);

			for (unsigned int n=0; n<i_NumProc; ++n)
			{
				m_hwthread[n]=(HANDLE)_beginthreadex( NULL, 0, poolrun, this, 0, &m_threadid);
#ifdef _DEBUG
				char name[256]; sprintf_s(name,"Working thread %d",n);
				SetThreadName(m_threadid,name);
#endif
			}
		}
	}

	void threadpool::flush()
	{
//		while (m_tasknum)
//			SwitchToThread();

		PRINT("vege\n");
		m_idletask.clear();
		m_taskbuf.resize(0);
	}

	threadpool::~threadpool()
	{
		setnumthreads(0);

		CloseHandle(m_exitevent);
		CloseHandle(m_workevent);
	}

	unsigned threadpool::poolrun(void* i_param)
	{
		threadpool* pool=(threadpool*)i_param;

		pool->run();

		return 0;
	}

	void threadpool::run()
	{
		int ret;
		unsigned taskindex;

		PRINT("thread elindult\n");

		while ((ret=gettask(taskindex))==1)
		{
			//bejott a taszk, elinditjuk
			taskdescinternal* task=&m_taskbuf[0]+taskindex;

			(task->m_task->*task->m_func)(task->m_data,taskindex);
			//vege a taszknak

			m_taskmutex.lock();
			int semaphore=0;

			//csokkentjuk a parenttaskban a futo gyerekek szamat,

			while (true)
			{
				if (!task->m_runningchildnum)
				{
					task->m_state=TASK_COMPLETED;

					for (unsigned int n=0; n<task->m_dependentlist.size(); ++n)
					{
						if (!(--m_taskbuf[task->m_dependentlist[n]].m_dependencycounter))
						{
							PRINT("elfogyott a dependency\n");
							m_taskbuf[task->m_dependentlist[n]].m_state=TASK_IDLE;
							m_idletask.push(task->m_dependentlist[n]);
							++semaphore;
						}
						else
						{
							PRINT("maradt %d dependency\n",m_taskbuf[task->m_dependentlist[n]].m_dependencycounter);
						}
					}
				}

				if (task->m_parenttask>=0)
				{
					task=&m_taskbuf[task->m_parenttask];
					--task->m_runningchildnum;
				}
				else
				{
					break;
				}


			}
			--m_tasknum;

			if (semaphore)
				ReleaseSemaphore(m_workevent,semaphore,NULL);

			m_taskmutex.unlock();

		}

//		PRINT("Thread befejezodott\n");
	}

	int threadpool::addtask(const taskdesc& i_desc)
	{
		m_taskmutex.lock();
		taskdescinternal newtask;
		newtask.m_task=i_desc.m_task;
		newtask.m_func=i_desc.m_taskfn;
		newtask.m_data=i_desc.m_data;
		int taskindex=m_taskbuf.size();
		for (unsigned int n=0; n<i_desc.m_dependency.size(); ++n)
		{
			if (m_taskbuf[i_desc.m_dependency[n]].m_state!=TASK_COMPLETED)
			{
				m_taskbuf[i_desc.m_dependency[n]].m_dependentlist.push_back(taskindex);
				newtask.m_dependencycounter++;
			}
		}

		newtask.m_runningchildnum=0;
		newtask.m_parenttask=i_desc.m_parenttask;

		m_taskbuf.push_back(newtask);

		if (!newtask.m_dependencycounter)
		{
			m_idletask.push(m_taskbuf.size()-1);
			ReleaseSemaphore(m_workevent,1,NULL);
		}

		++m_tasknum;
		m_taskmutex.unlock();

		return taskindex;
	}

	void threadpool::addtasks(taskid* o_taskid, const taskdesc* const i_desc, int i_tasknum)
	{
		m_taskmutex.lock();

		int semaphore=0;

		for (int taskc=0; taskc<i_tasknum; ++taskc)
		{
			taskdescinternal newtask;
			newtask.m_task=i_desc[taskc].m_task;
			newtask.m_func=i_desc[taskc].m_taskfn;
			newtask.m_data=i_desc[taskc].m_data;
			int taskindex=m_taskbuf.size();
			PRINT("uj task bejott\n");
			for (unsigned int n=0; n<i_desc[taskc].m_dependency.size(); ++n)
			{
				if (m_taskbuf[i_desc[taskc].m_dependency[n]].m_state!=TASK_COMPLETED)
				{
					m_taskbuf[i_desc[taskc].m_dependency[n]].m_dependentlist.push_back(taskindex);
					newtask.m_dependencycounter++;
				}
			}

			newtask.m_runningchildnum=0;
			newtask.m_parenttask=i_desc[taskc].m_parenttask;

			o_taskid[taskc]=m_taskbuf.size();
			m_taskbuf.push_back(newtask);

			if (!newtask.m_dependencycounter)
			{
				PRINT("uj feldolgozhato task\n");
				m_idletask.push(m_taskbuf.size()-1);
			}

			++m_tasknum;
		}

		if (semaphore)
			ReleaseSemaphore(m_workevent,semaphore,NULL);
		m_taskmutex.unlock();

	}

	int threadpool::gettask(unsigned& o_TaskIndex)
	{
		const HANDLE waitFor[]={m_exitevent,m_workevent};

		DWORD ret=WaitForMultipleObjects(2,waitFor,FALSE,INFINITE) - WAIT_OBJECT_0;

		if (!ret)
			return 0;

		//ha van uj task
		m_taskmutex.lock();
		PRINT("kiszedek egy taszkot a listabol\n");
		o_TaskIndex=m_idletask.front();
		m_idletask.pop();

		PRINT("maradt %d feldolgozhato task\n",m_idletask.size());
	//	PRINT("en vagyok a %d szamu\n")
//		PRINT("meg %d task van\n",m_task.size());
		m_taskmutex.unlock();

		return 1;
		
	}
}//namespace Dyn
