#ifndef _thread_h_
#define _thread_h_

#include <windows.h> 
#include "..\containers\string.h"
namespace threading
{
	typedef unsigned (WINAPI *threadfunc)(void*);
	
	class thread
	{
	public:
		thread(const ctr::string& i_name="");
		void start(threadfunc i_func, void* i_data);
		unsigned getid() const;
		void join() const;
	private:
		HANDLE m_handle;
		ctr::string m_name;
		unsigned m_id;
	};
}
#endif//_thread_h_