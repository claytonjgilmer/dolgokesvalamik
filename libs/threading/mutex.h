#ifndef _MUTEX_H_
#define _MUTEX_H_

#ifdef _XENON
#include <xtl.h>
#else
#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif
#include <Windows.h>
#endif

#ifdef WIN32
//#include <intrin.h>
#endif

#define mutextype 0

	class mutex
	{
	public:
		mutex();
		~mutex();

		void lock();
		BOOL try_lock();
		void unlock();
	private:

#if mutextype==0
		CRITICAL_SECTION m_cs;
#else
		volatile long m_data;
#endif
	};



	class mutex2
	{
	public:
		mutex2();
		~mutex2();

		void lock();
		void unlock();
	private:
		volatile long m_data;
	};

	struct blocklocker
	{
		mutex& lock;
		blocklocker(mutex& i_lock):lock(i_lock){lock.lock();}
		~blocklocker(){lock.unlock();}
	};
#endif//_MUTEX_H_
