#include <process.h>
#include "thread.h"

#define MS_VC_EXCEPTION 0x406D1388
#pragma pack(push,8)
	typedef struct //tagTHREADNAME_INFO
	{
		unsigned m_type; // Must be 0x1000.
		const char* m_name; // Pointer to name (in user addr space).
		unsigned m_threadid; // Thread ID (-1=caller thread).
		unsigned m_flags; // Reserved for future use, must be zero.
	} threadname_info;
#pragma pack(pop)

	void setthreadname( unsigned i_threadID, const string& i_threadname)
	{
		Sleep(1);
		threadname_info info;
		info.m_type=0x1000;
		info.m_name=i_threadname.c_str();
		info.m_threadid=i_threadID;
		info.m_flags=0;

		__try
		{
			RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
	
	thread::thread(const string& i_name):
	m_name(i_name)
	{
	}
	
	void thread::start(threadfunc i_func, void* i_data)
	{
				m_handle=(HANDLE)_beginthreadex( NULL, 0, i_func, i_data, 0, &m_id);

				if (m_name.size())
					setthreadname(m_id,m_name);
	}

	void thread::join() const
	{
		WaitForSingleObject(m_handle,INFINITE);
	}

	unsigned thread::getid() const
	{
		return m_id;
	}

	void thread::set_name(const string& i_name)
	{
		m_name=i_name;
		if (m_name.size())
			setthreadname(m_id,i_name);
	}
