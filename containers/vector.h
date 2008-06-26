#ifndef _vector_h_
#define _vector_h_

#include <new.h>
#include "base\misc.h"
namespace ctr
{
	template <class basetype>
	class vector
	{
	public:
		vector();
		~vector();

		typedef basetype* iterator;
		typedef const basetype* const_iterator;

		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;

		basetype& back();
		const basetype& back() const;

		basetype& front();
		const basetype& front() const;

		static const vector& emptyvector()
		{
			static const vector e;

			return e;
		}

		void assign(unsigned i_count, const basetype& val);

		void push_back(const basetype& i_newelem);
		void pop_back();
		void insert(iterator i_where, const basetype& i_val);
		void resize(unsigned i_newsize);
		void clear();
		void erase(iterator i_where);
		void eraseunordered(iterator i_where);
		unsigned size() const;
		bool empty() const;
		void reserve(unsigned i_capacity);

		basetype& operator[](unsigned i_index);
		const basetype& operator[](unsigned i_index) const;

		void operator=(const vector& i_other);
		vector(const vector& i_other);
	protected:
		basetype* m_buf;
		unsigned m_size;
		unsigned m_capacity;

		void alloc();
		void alloc(int i_newcapacity);

	private:
	};








	template<class basetype>
	MLINLINE void vector<basetype>::operator =(const vector& i_other)
	{
		clear();
		reserve(i_other.size());
		m_size=i_other.size();

		for (unsigned int n=0; n<i_other.size(); ++n)
			new (m_buf+n) basetype(i_other[n]);

	}

	template<class basetype>
	MLINLINE vector<basetype>::vector(const vector& i_other)
	{
		if (i_other.m_size)
		{
			m_capacity=i_other.m_size;
			char* newbuf=new char[m_capacity*sizeof(basetype)];
			m_buf=(basetype*)newbuf;
			m_size=m_capacity;

			for (unsigned int n=0; n<m_size; ++n)
				new (m_buf+n) basetype(i_other.m_buf[n]);
		}
		else
		{
			m_buf=0;
			m_size=m_capacity=0;
		}
	}

	template<class basetype>
	MLINLINE vector<basetype>::vector()
	{
		m_buf=NULL;
		m_size=0;
		m_capacity=0;
	}

	template<class basetype>
	MLINLINE vector<basetype>::~vector()
	{
		if (!m_buf)
			return;

		for (unsigned int n=0; n<m_size; ++n)
			m_buf[n].~basetype();

		char* charbuf=(char*)m_buf;
		delete [] charbuf;
	}

	template<class basetype>
	MLINLINE typename vector<basetype>::iterator vector<basetype>::begin()
	{
		return m_buf;
	}

	template<class basetype>
	MLINLINE typename vector<basetype>::const_iterator vector<basetype>::begin() const
	{
		return m_buf;
	}

	template<class basetype>
	MLINLINE typename vector<basetype>::iterator vector<basetype>::end()
	{
		return m_buf+m_size;
	}

	template<class basetype>
	MLINLINE typename vector<basetype>::const_iterator vector<basetype>::end() const
	{
		return m_buf+m_size;
	}

	template<class basetype>
	MLINLINE void vector<basetype>::push_back(const basetype& i_newelem)
	{
		if (m_size==m_capacity)
			alloc();

		new (m_buf+m_size) basetype(i_newelem);
		++m_size;
	}

	template <class basetype>
	MLINLINE void vector<basetype>::pop_back()
	{
		m_buf[--m_size].~basetype();
	}


	template<class basetype>
	MLINLINE void vector<basetype>::insert(iterator i_where, const basetype& i_val)
	{
		unsigned where=i_where-m_buf;

		if (m_size==m_capacity)
		{
			m_capacity=2*m_capacity+1;
			char* charbuf=new char[m_capacity*sizeof(basetype)];
			basetype* newbuf=(basetype*)charbuf;

			for (unsigned n=0; n<where; ++n)
				new (newbuf+n) basetype(m_buf[n]);

			new (newbuf+where) basetype(i_val);

			for (unsigned n=where; n<m_size; ++n)
				new (newbuf+n+1) basetype(m_buf[n]);

			if (m_buf)
				delete [] (void*)m_buf;

			m_buf=newbuf;
		}
		else
		{
			new (m_buf+ m_size) basetype(m_buf[m_size-1]);

			for (unsigned int n=m_size-1; n>where; --n)
				m_buf[n]=m_buf[n-1];

			m_buf[where]=i_val;
		}

		m_size++;
	}

	template<class basetype>
	MLINLINE basetype& vector<basetype>::back()
	{
		return m_buf[m_size-1];
	}

	template<class basetype>
	MLINLINE const basetype& vector<basetype>::back() const
	{
		return m_buf[m_size-1];
	}

	template<class basetype>
	MLINLINE basetype& vector<basetype>::front()
	{
		return m_buf[0];
	}

	template<class basetype>
	MLINLINE const basetype& vector<basetype>::front() const
	{
		return m_buf[0];
	}

	template<class basetype>
	MLINLINE unsigned vector<basetype>::size() const
	{
		return m_size;
	}

	template<class basetype>
	MLINLINE bool vector<basetype>::empty() const
	{
		return m_size==0;
	}

	template<class basetype>
	MLINLINE void vector<basetype>::clear()
	{
		if (m_size)
		{
			for (iterator it=begin(); it!=end(); ++it)
			{
				it->~basetype();
			}

			m_size=0;
		}
	}

	template<class basetype>
	MLINLINE void vector<basetype>::assign(unsigned i_count, const basetype& val)
	{
		clear();
		alloc(i_count);
		for (unsigned int n=0; n<i_count; ++n)
		{
			new (m_buf+n) basetype(val);
		}

		m_size=i_count;
	}


	template<class basetype>
	MLINLINE void vector<basetype>::erase(iterator i_where)
	{
		for (iterator it=i_where; it!=end()-1; ++it)
			*it=*(it+1);

		back().~basetype();
		--m_size;
	}

	template<class basetype>
	MLINLINE void vector<basetype>::eraseunordered(iterator i_where)
	{

		*i_where=back();
		back().~basetype();
		--m_size;
	}

	template<class basetype>
	MLINLINE void vector<basetype>::resize(unsigned i_newsize)
	{
		if (i_newsize>m_size)
		{
			if (i_newsize>m_capacity)
			{
				alloc(i_newsize);
			}

			for (unsigned int n=m_size; n<i_newsize; ++n)
			{
				new (m_buf+n) basetype;
			}
		}
		else
		{
			for (unsigned n=i_newsize; n<m_size; ++n)
			{
				(m_buf+n)->~basetype();
			}
		}

		m_size=i_newsize;

	}

	template<class basetype>
	MLINLINE basetype& vector<basetype>::operator[](unsigned i_index)
	{
		return m_buf[i_index];
	}

	template<class basetype>
	MLINLINE const basetype& vector<basetype>::operator[](unsigned i_index) const
	{
		return m_buf[i_index];
	}


	template<class basetype>
	MLINLINE void vector<basetype>::alloc()
	{
		++m_capacity<<=1;
		char* newbuf=new char[m_capacity*sizeof(basetype)];

		for (unsigned int n=0; n<m_size; ++n)
		{
			new (newbuf+n*sizeof(basetype)) basetype(m_buf[n]);
			m_buf[n].~basetype();
		}

		if (m_buf)
		{
			char* charbuf=(char*)m_buf;
			delete [] charbuf;
		}

		m_buf=(basetype*)newbuf;
	}

	template<class basetype>
	MLINLINE void vector<basetype>::reserve(unsigned i_capacity)
	{
		if (m_capacity<i_capacity)
			alloc(i_capacity);
	}

	template<class basetype>
	MLINLINE void vector<basetype>::alloc(int i_newcapacity)
	{
		m_capacity=i_newcapacity;
		char* newbuf=new char[m_capacity*sizeof(basetype)];

		for (unsigned int n=0; n<m_size; ++n)
		{
			new (newbuf+n*sizeof(basetype)) basetype(m_buf[n]);
			m_buf[n].~basetype();
		}

		if (m_buf)
		{
			char* charbuf=(char*)m_buf;
			delete [] charbuf;
		}

		m_buf=(basetype*)newbuf;
	}
}//namespace ctr
#endif//_vector_h_