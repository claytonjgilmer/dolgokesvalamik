#ifndef _queue_h_
#define _queue_h_

namespace ctr
{
	template<class utilstype, unsigned bufsize> //bufsize kettohatvany!!!
	class queue
	{
	public:
		queue();
		~queue();
		void push(const utilstype&);
		void pop();
		utilstype& front();
		const utilstype& front() const;
		void clear();

		unsigned size() const;
	protected:
		char	m_buf[sizeof(utilstype)*bufsize];
		int		m_first;
		int		m_end;
	};

	template<class utilstype, unsigned bufsize>
	MLINLINE queue<utilstype,bufsize>::queue()
	{
		m_first=0;
		m_end=0;
	}

	template<class utilstype, unsigned bufsize>
	MLINLINE queue<utilstype,bufsize>::~queue()
	{
		clear();
	}

	template<class utilstype, unsigned bufsize>
	MLINLINE void queue<utilstype,bufsize>::clear()
	{
		while (m_first!=m_end)
		{
			((utilstype*)m_buf)[m_first].~utilstype();
			m_first=(m_first+1)&(bufsize-1);
		}

		m_first=m_end=0;
	}

	template<class utilstype, unsigned bufsize>
	MLINLINE unsigned queue<utilstype,bufsize>::size() const
	{
		return (m_end-m_first+bufsize)&(bufsize-1);
	}

	template<class utilstype, unsigned bufsize>
	MLINLINE void queue<utilstype,bufsize>::push(const utilstype& i_elem)
	{
		utilstype* buf=(utilstype*)m_buf;
		new (buf+m_end) utilstype(i_elem);
		m_end=(m_end+1)&(bufsize-1);
	}

	template<class utilstype, unsigned bufsize>
	MLINLINE void queue<utilstype,bufsize>::pop()
	{
		((utilstype*)m_buf)[m_first].~utilstype();
		m_first=(m_first+1)&(bufsize-1);
	}

	template<class utilstype,unsigned bufsize>
	MLINLINE utilstype& queue<utilstype,bufsize>::front()
	{
		return ((utilstype*)m_buf)[m_first];
	}

	template<class utilstype,unsigned bufsize>
	MLINLINE const utilstype& queue<utilstype,bufsize>::front() const
	{
		return ((utilstype*)m_buf)[m_first];
	}
}
#endif//_dynqueue_h_