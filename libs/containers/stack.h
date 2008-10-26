#ifndef _stack_h_
#define _stack_h_

#include "utils/assert.h"

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
//			blocklocker b(m_mutex);
			m_mutex.lock();
			assertion(m_elemcount<=bufsize);
			m_buf[m_elemcount]=i_elem;
			++m_elemcount;
			m_mutex.unlock();
		}

		T pop()
		{
			m_mutex.lock();
//			blocklocker b(m_mutex);
			--m_elemcount;
			assertion(m_elemcount>=0);
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
		mutex m_mutex;
	};
#endif//_stack_h_