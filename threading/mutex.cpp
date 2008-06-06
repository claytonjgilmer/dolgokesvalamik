#include "mutex.h"

namespace threading
{
	mutex::mutex()
	{
#if mutextype==0
		InitializeCriticalSectionAndSpinCount(&m_cs,10000);
#else
		m_data=0;
#endif
//		InitializeCriticalSection(&m_cs);
	}

	mutex::~mutex()
	{
#if mutextype==0
		DeleteCriticalSection(&m_cs);
#else
#endif
	}

	void mutex::lock()
	{
#if mutextype==0
		EnterCriticalSection(&m_cs);
#else
		while (true)
		{
			int n;
			for (n=0; n<1000; ++n)
			{
				if (!_InterlockedCompareExchange(&m_data,1,0))
					return;
			}

			SwitchToThread();
		}
//			while (_InterlockedCompareExchange(&m_data,1,0));
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
		while (_InterlockedCompareExchange(&m_data,1,0));
	}

	void mutex2::unlock()
	{
		_InterlockedExchange(&m_data,0);
	}



}//namespace threading