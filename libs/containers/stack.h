#ifndef _stack_h_
#define _stack_h_

#include "utils/assert.h"

namespace ctr
{
	template<class T, unsigned bufsize>
	class stack
	{
	public:
		stack()
		{
			m_elemcount=0;
		}
		~stack()
		{

		}

		void clear()
		{
			m_elemcount=0;
		}

		void push(const T& i_elem)
		{
//			threading::blocklocker b(m_mutex);
			m_mutex.lock();
			utils::assertion(m_elemcount<=bufsize);
			m_buf[m_elemcount]=i_elem;
			++m_elemcount;
			m_mutex.unlock();
		}

		T pop()
		{
			m_mutex.lock();
//			threading::blocklocker b(m_mutex);
			--m_elemcount;
			utils::assertion(m_elemcount>=0);
			T ret=m_buf[m_elemcount];
			m_mutex.unlock();
			return ret;
		}

		unsigned size() const
		{
			return m_elemcount;
		}

	private:
		T m_buf[bufsize];
		long m_elemcount;
		threading::mutex m_mutex;
	};
}
#endif//_stack_h_