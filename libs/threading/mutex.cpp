#include "mutex.h"

namespace threading
{
	mutex::mutex()
	{
#if mutextype==0
//		InitializeCriticalSectionAndSpinCount(&m_cs,100);
		InitializeCriticalSection(&m_cs);
#else
		m_data=0;
#endif
	}

	mutex::~mutex()
	{
#if mutextype==0
		DeleteCriticalSection(&m_cs);
#else
#endif
	}

	BOOL mutex::try_lock()
	{
#if mutextype==0
		return (TryEnterCriticalSection(&m_cs));
#else
		return (_InterlockedCompareExchange(&m_data,1,0));
#endif
	}

	void mutex::lock()
	{
#if mutextype==0
		EnterCriticalSection(&m_cs);
#else
		while (_InterlockedCompareExchange(&m_data,1,0))
			SwitchToThread();
#endif
	}

	void mutex::unlock()
	{
#if mutextype==0
		LeaveCriticalSection(&m_cs);
#else
		_InterlockedExchange(&m_data,0);
#endif
	}





	mutex2::mutex2()
	{
		m_data=0;
	}

	mutex2::~mutex2()
	{
	}

	void mutex2::lock()
	{
		while (_InterlockedCompareExchange(&m_data,1,0))
			SwitchToThread();
	}

	void mutex2::unlock()
	{
		_InterlockedExchange(&m_data,0);
	}

}//namespace threading
