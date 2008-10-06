#ifndef _MUTEX_H_
#define _MUTEX_H_

#ifdef _XENON
#include <xtl.h>
#else
#include <Windows.h>
#endif

#ifdef WIN32
//#include <intrin.h>
#endif

#define mutextype 1

namespace threading
{
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
} //namespace threading
#endif//_MUTEX_H_