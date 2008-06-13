#ifndef _thread_h_
#define _thread_h_

namespace threading
{
	typedef void (*threadfunc)(void*);
	
	class thread
	{
	public:
		thread(const std::string& i_name="");
		void start(threadfunc i_func);
		unsigned getid() const;
	private:
		HANDLE m_handle;
		std::string m_name;
		unsigned m_id;
	}
}
#endif//_thread_h_