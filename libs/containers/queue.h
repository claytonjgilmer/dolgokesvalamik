#ifndef _queue_h_
#define _queue_h_

#include "utils/assert.h"

	template<typename basetype, unsigned bufsize> //bufsize kettohatvany!!!
	class queue
	{
	public:
		queue();
		~queue();
		void push(const basetype&);
//		void threadsafe_push(const basetype&);
		basetype pop();
//		basetype threadsafe_pop();
		void clear();

		unsigned size() const;
	protected:
		char	m_buf[sizeof(basetype)*bufsize];
		int		m_first;
		int		m_end;
	};

	template<class basetype, unsigned bufsize>
	MLINLINE queue<basetype,bufsize>::queue()
	{
		m_first=0;
		m_end=0;
	}

	template<class basetype, unsigned bufsize>
	MLINLINE queue<basetype,bufsize>::~queue()
	{
		clear();
	}

	template<class basetype, unsigned bufsize>
	MLINLINE void queue<basetype,bufsize>::clear()
	{
		m_first=m_end=0;
	}

	template<class basetype, unsigned bufsize>
	MLINLINE void queue<basetype,bufsize>::push(const basetype& i_elem)
	{
		int index=InterlockedExchangeAdd((long*)&m_end,1) & (bufsize-1);
		basetype* buf=(basetype*)m_buf;
		new (buf+index) basetype(i_elem);
	}

	template<class basetype, unsigned bufsize>
	MLINLINE basetype queue<basetype,bufsize>::pop()
	{
		assertion(m_first!=m_end);
		int index=InterlockedExchangeAdd((long*)&m_first,1);
		index&=(bufsize-1);
		return ((basetype*)m_buf)[index];
	}
#endif//_dynqueue_h_
