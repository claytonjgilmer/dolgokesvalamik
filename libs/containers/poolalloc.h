#ifndef _poolalloc_h_
#define _poolalloc_h_

	template<typename T, unsigned poolsize=128> //legyen mar kettohatvany
	struct poolalloc
	{
		poolalloc();
		~poolalloc();

		void push_back(const T& i_elem);
		void clear();

		T& operator[](unsigned i_index);
		const T& operator[](unsigned i_index) const;

		void erase(unsigned i_index);

		vector<fixedvector<T,poolsize> > m_poolbuf;
		poolalloc(const poolalloc& i_other){}
		void operator=(const poolalloc& i_other){}
	};

	template<typename T, unsigned poolsize>
	poolalloc<T,poolsize>::poolalloc()
	{
	}

	template<typename T, unsigned poolsize>
	poolalloc<T,poolsize>::~poolalloc()
	{
	}

	template<typename T, unsigned poolsize>
	void poolalloc<T,poolsize>::clear()
	{
		m_poolbuf.clear();
	}
	template<typename T, unsigned poolsize>
	T& poolalloc<T,poolsize>::operator[](unsigned i_index)
	{
		return m_poolbuf[i_index & poolsize][i_index >> (logn<poolsize>())];
	}

	template<typename T, unsigned poolsize>
	const T& poolalloc<T,poolsize>::operator[](unsigned i_index) const
	{
		return m_poolbuf[i_index & poolsize][i_index >> (logn<poolsize>())];
	}

	template<typename T, unsigned poolsize>
	void poolalloc<T,poolsize>::erase(unsigned i_index)
	{
		m_poolbuf[i_index & poolsize][i_index >> (logn<poolsize>())]=m_poolbuf.back().back();

		if (m_poolbuf.back().size()>1)
//			m_poolbuf.back().resize(m_poolbuf.back().size()-1);
			m_poolbuf.back().pop_back();
		else
//			m_poolbuf.resize(m_poolbuf.size()-1);
			m_poolbuf.pop_back();
	}

	template<typename T, unsigned poolsize>
	void poolalloc<T,poolsize>::push_back(const T& i_elem)
	{
		if (!m_poolbuf.size() || m_poolbuf.back().size()==poolsize)
			m_poolbuf.push_back(fixedvector<T,poolsize>());

		m_poolbuf.back().push_back(i_elem);
	}
#endif//_poolalloc_h_
