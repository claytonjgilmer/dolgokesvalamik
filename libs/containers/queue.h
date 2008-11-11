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
		void threadsafe_push(const basetype&);
		void pop();
		basetype threadsafe_pop();
		basetype& front();
		const basetype& front() const;
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
		while (m_first!=m_end)
		{
			((basetype*)m_buf)[m_first].~basetype();
			m_first=(m_first+1)&(bufsize-1);
		}

		m_first=m_end=0;
	}

/*
	template<class basetype, unsigned bufsize>
	MLINLINE unsigned queue<basetype,bufsize>::size() const
	{
		return (m_end-m_first+bufsize)&(bufsize-1);
	}

	template<class basetype, unsigned bufsize>
	MLINLINE void queue<basetype,bufsize>::push(const basetype& i_elem)
	{
		basetype* buf=(basetype*)m_buf;
		new (buf+m_end) basetype(i_elem);
		m_end=(m_end+1)&(bufsize-1);
	}

	template<class basetype, unsigned bufsize>
	MLINLINE void queue<basetype,bufsize>::pop()
	{
		assertion(m_first!=m_end);
		((basetype*)m_buf)[m_first].~basetype();
		m_first=(m_first+1)&(bufsize-1);
	}
*/
	template<class basetype, unsigned bufsize>
	MLINLINE void queue<basetype,bufsize>::threadsafe_push(const basetype& i_elem)
	{
		int index=InterlockedExchangeAdd((long*)&m_end,1) & (bufsize-1);
		basetype* buf=(basetype*)m_buf;
		new (buf+index) basetype(i_elem);
//		m_end=(m_end+1)&(bufsize-1);
	}

	template<class basetype, unsigned bufsize>
	MLINLINE basetype queue<basetype,bufsize>::threadsafe_pop()
	{
		assertion(m_first!=m_end);
//		int index=_InterlockedIncrement((long*)&m_first);
//		--index;
		int index=InterlockedExchangeAdd((long*)&m_first,1);
//		--index;
		index&=(bufsize-1);
		return ((basetype*)m_buf)[index];
	}
/*
	template<class basetype,unsigned bufsize>
	MLINLINE basetype& queue<basetype,bufsize>::front()
	{
		assertion(m_first!=m_end);
		return ((basetype*)m_buf)[m_first];
	}

	template<class basetype,unsigned bufsize>
	MLINLINE const basetype& queue<basetype,bufsize>::front() const
	{
		assertion(m_first!=m_end);
		return ((basetype*)m_buf)[m_first];
	}
*/
#endif//_dynqueue_h_
