#ifndef _MUTEX_H_
#define _MUTEX_H_

#ifdef _XENON
#include <xtl.h>
#else
#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0502	// Change this to the appropriate value to target other versions of Windows.
#endif
#include <Windows.h>
#endif

#ifdef WIN32
//#include <intrin.h>
#endif

#define mutextype 0

	struct mutex
	{
		mutex();
		~mutex();

		void lock();
		BOOL try_lock();
		void unlock();

#if mutextype==0
		CRITICAL_SECTION m_cs;
#else
		long m_data;
#endif
	};



	struct mutex2
	{
		mutex2();
		~mutex2();

		void lock();
		void unlock();

		long m_data;
	};

	struct blocklocker
	{
		mutex& lock;
		blocklocker(mutex& i_lock):lock(i_lock){lock.lock();}
		~blocklocker(){lock.unlock();}
	};
#endif//_MUTEX_H_
