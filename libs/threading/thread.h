#ifndef _thread_h_
#define _thread_h_

#include <windows.h>
#include "containers\string.h"
	typedef unsigned (WINAPI *threadfunc)(void*);

	struct thread
	{
		thread(const string& i_name=string(""));
		void start(threadfunc i_func, void* i_data);
		unsigned getid() const;
		void join() const;
		void set_name(const string& i_name);




		HANDLE m_handle;
		string m_name;
		unsigned m_id;
	};
#endif//_thread_h_

