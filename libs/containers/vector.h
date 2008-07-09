#ifndef _vector_h_
#define _vector_h_

#include <new.h>
#include "utils/misc.h"
#include "utils/assert.h"
namespace ctr
{
	template <class utilstype>
	class vector
	{
	public:
		vector();
		~vector();

		typedef utilstype* iterator;
		typedef const utilstype* const_iterator;

		iterator begin();
		iterator end();
		const_iterator begin() const;
		const_iterator end() const;

		utilstype& back();
		const utilstype& back() const;

		utilstype& front();
		const utilstype& front() const;

		static const vector& emptyvector()
		{
			static const vector e;

			return e;
		}

		void assign(unsigned i_count, const utilstype& val);

		void push_back(const utilstype& i_newelem);
		void pop_back();
		void insert(iterator i_where, const utilstype& i_val);
		void resize(unsigned i_newsize);
		void clear();
		void erase(iterator i_where);
		void eraseunordered(iterator i_where);
		unsigned size() const;
		bool empty() const;
		void reserve(unsigned i_capacity);

#if 1//ndef _DEBUG
		operator utilstype* ();
		operator const utilstype* () const;
#else
		utilstype& operator[](unsigned i_index);
		const utilstype& operator[](unsigned i_index) const;
#endif//_DEBUG

		void operator=(const vector& i_other);
		vector(const vector& i_other);
	protected:
		utilstype* m_buf;
		unsigned m_size;
		unsigned m_capacity;

		void alloc();
		void alloc(int i_newcapacity);

	private:
	};








	template<class utilstype>
	MLINLINE void vector<utilstype>::operator =(const vector& i_other)
	{
		clear();
		reserve(i_other.size());
		m_size=i_other.size();

		for (unsigned int n=0; n<i_other.size(); ++n)
			new (m_buf+n) utilstype(i_other[n]);

	}

	template<class utilstype>
	MLINLINE vector<utilstype>::vector(const vector& i_other)
	{
		if (i_other.m_size)
		{
			m_capacity=i_other.m_size;
			char* newbuf=new char[m_capacity*sizeof(utilstype)];
			m_buf=(utilstype*)newbuf;
			m_size=m_capacity;

			for (unsigned int n=0; n<m_size; ++n)
				new (m_buf+n) utilstype(i_other.m_buf[n]);
		}
		else
		{
			m_buf=0;
			m_size=m_capacity=0;
		}
	}

	template<class utilstype>
	MLINLINE vector<utilstype>::vector()
	{
		m_buf=NULL;
		m_size=0;
		m_capacity=0;
	}

	template<class utilstype>
	MLINLINE vector<utilstype>::~vector()
	{
		if (!m_buf)
			return;

		for (unsigned int n=0; n<m_size; ++n)
			m_buf[n].~utilstype();

		char* charbuf=(char*)m_buf;
		delete [] charbuf;
	}

	template<class utilstype>
	MLINLINE typename vector<utilstype>::iterator vector<utilstype>::begin()
	{
		utils::assertion(m_buf!=NULL,"vector<utilstype>::m_buf nem lehet null");
		return m_buf;
	}

	template<class utilstype>
	MLINLINE typename vector<utilstype>::const_iterator vector<utilstype>::begin() const
	{
		utils::assertion(m_buf!=NULL,"vector<utilstype>::m_buf nem lehet null");
		return m_buf;
	}

	template<class utilstype>
	MLINLINE typename vector<utilstype>::iterator vector<utilstype>::end()
	{
		utils::assertion(m_buf!=NULL,"vector<utilstype>::m_buf nem lehet null");
		return m_buf+m_size;
	}

	template<class utilstype>
	MLINLINE typename vector<utilstype>::const_iterator vector<utilstype>::end() const
	{
		utils::assertion(m_buf!=NULL,"vector<utilstype>::m_buf nem lehet null");
		return m_buf+m_size;
	}

	template<class utilstype>
	MLINLINE void vector<utilstype>::push_back(const utilstype& i_newelem)
	{
		if (m_size==m_capacity)
			alloc();

		new (m_buf+m_size) utilstype(i_newelem);
		++m_size;
	}

	template <class utilstype>
	MLINLINE void vector<utilstype>::pop_back()
	{
		utils::assertion(m_buf!=NULL,"vector<utilstype>::m_buf nem lehet null");
		utils::assertion(m_size>0,"vector<utilstype>::size() <=0!!!");
		m_buf[--m_size].~utilstype();
	}


	template<class utilstype>
	MLINLINE void vector<utilstype>::insert(iterator i_where, const utilstype& i_val)
	{
		unsigned where=i_where-m_buf;

		if (m_size==m_capacity)
		{
			m_capacity=2*m_capacity+1;
			char* charbuf=new char[m_capacity*sizeof(utilstype)];
			utilstype* newbuf=(utilstype*)charbuf;

			for (unsigned n=0; n<where; ++n)
				new (newbuf+n) utilstype(m_buf[n]);

			new (newbuf+where) utilstype(i_val);

			for (unsigned n=where; n<m_size; ++n)
				new (newbuf+n+1) utilstype(m_buf[n]);

			if (m_buf)
				delete [] (void*)m_buf;

			m_buf=newbuf;
		}
		else
		{
			new (m_buf+ m_size) utilstype(m_buf[m_size-1]);

			for (unsigned int n=m_size-1; n>where; --n)
				m_buf[n]=m_buf[n-1];

			m_buf[where]=i_val;
		}

		m_size++;
	}

	template<class utilstype>
	MLINLINE utilstype& vector<utilstype>::back()
	{
		utils::assertion(m_buf!=NULL,"vector<utilstype>::m_buf nem lehet null");
		utils::assertion(m_size>0,"vector<utilstype>::size() <=0!!!");
		return m_buf[m_size-1];
	}

	template<class utilstype>
	MLINLINE const utilstype& vector<utilstype>::back() const
	{
		utils::assertion(m_buf!=NULL,"vector<utilstype>::m_buf nem lehet null");
		utils::assertion(m_size>0,"vector<utilstype>::size() <=0!!!");
		return m_buf[m_size-1];
	}

	template<class utilstype>
	MLINLINE utilstype& vector<utilstype>::front()
	{
		utils::assertion(m_buf!=NULL,"vector<utilstype>::m_buf nem lehet null");
		utils::assertion(m_size>0,"vector<utilstype>::size() <=0!!!");
		return m_buf[0];
	}

	template<class utilstype>
	MLINLINE const utilstype& vector<utilstype>::front() const
	{
		utils::assertion(m_buf!=NULL,"vector<utilstype>::m_buf nem lehet null");
		utils::assertion(m_size>0,"vector<utilstype>::size() <=0!!!");
		return m_buf[0];
	}

	template<class utilstype>
	MLINLINE unsigned vector<utilstype>::size() const
	{
		return m_size;
	}

	template<class utilstype>
	MLINLINE bool vector<utilstype>::empty() const
	{
		return m_size==0;
	}

	template<class utilstype>
	MLINLINE void vector<utilstype>::clear()
	{
		if (m_size)
		{
			for (iterator it=begin(); it!=end(); ++it)
			{
				it->~utilstype();
			}

			m_size=0;
		}
	}

	template<class utilstype>
	MLINLINE void vector<utilstype>::assign(unsigned i_count, const utilstype& val)
	{
		clear();
		alloc(i_count);
		for (unsigned int n=0; n<i_count; ++n)
		{
			new (m_buf+n) utilstype(val);
		}

		m_size=i_count;
	}


	template<class utilstype>
	MLINLINE void vector<utilstype>::erase(iterator i_where)
	{
		for (iterator it=i_where; it!=end()-1; ++it)
			*it=*(it+1);

		back().~utilstype();
		--m_size;
	}

	template<class utilstype>
	MLINLINE void vector<utilstype>::eraseunordered(iterator i_where)
	{

		*i_where=back();
		back().~utilstype();
		--m_size;
	}

	template<class utilstype>
	MLINLINE void vector<utilstype>::resize(unsigned i_newsize)
	{
		if (i_newsize>m_size)
		{
			if (i_newsize>m_capacity)
			{
				alloc(i_newsize);
			}

			for (unsigned int n=m_size; n<i_newsize; ++n)
			{
				new (m_buf+n) utilstype;
			}
		}
		else
		{
			for (unsigned n=i_newsize; n<m_size; ++n)
			{
				(m_buf+n)->~utilstype();
			}
		}

		m_size=i_newsize;

	}
#if 0//def _DEBUG
	template<class utilstype>
	MLINLINE utilstype& vector<utilstype>::operator[](unsigned i_index)
	{
		utils::assertion(i_index<m_size,"vector<utilstype>::size() <=i_index!!!");
		return m_buf[i_index];
	}

	template<class utilstype>
	MLINLINE const utilstype& vector<utilstype>::operator[](unsigned i_index) const
	{
		utils::assertion(i_index<m_size,"vector<utilstype>::size() <=i_index!!!");
		return m_buf[i_index];
	}
#else
	template<class utilstype>
	MLINLINE vector<utilstype>::operator utilstype* ()
	{
		return m_buf;
	}

	template<class utilstype>
	MLINLINE vector<utilstype>::operator const utilstype* () const
	{
		return m_buf;
	}

#endif//_DEBUG

	template<class utilstype>
	MLINLINE void vector<utilstype>::alloc()
	{
		++m_capacity<<=1;
		char* newbuf=new char[m_capacity*sizeof(utilstype)];

		for (unsigned int n=0; n<m_size; ++n)
		{
			new (newbuf+n*sizeof(utilstype)) utilstype(m_buf[n]);
			m_buf[n].~utilstype();
		}

		if (m_buf)
		{
			char* charbuf=(char*)m_buf;
			delete [] charbuf;
		}

		m_buf=(utilstype*)newbuf;
	}

	template<class utilstype>
	MLINLINE void vector<utilstype>::reserve(unsigned i_capacity)
	{
		if (m_capacity<i_capacity)
			alloc(i_capacity);
	}

	template<class utilstype>
	MLINLINE void vector<utilstype>::alloc(int i_newcapacity)
	{
		m_capacity=i_newcapacity;
		char* newbuf=new char[m_capacity*sizeof(utilstype)];

		for (unsigned int n=0; n<m_size; ++n)
		{
			new (newbuf+n*sizeof(utilstype)) utilstype(m_buf[n]);
			m_buf[n].~utilstype();
		}

		if (m_buf)
		{
			char* charbuf=(char*)m_buf;
			delete [] charbuf;
		}

		m_buf=(utilstype*)newbuf;
	}
}//namespace ctr
#endif//_vector_h_