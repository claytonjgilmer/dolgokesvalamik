#ifndef _fixedvector_h_
#define _fixedvector_h_

#include "utils/assert.h"

	template <class T, unsigned bufsize> class fixedvector
	{
	public:
//#define TBuf ((T*)m_buf)
		fixedvector():
		TBuf((T*)m_buf)
		{
			m_size=0;
		}

		~fixedvector()
		{
			for (unsigned int n=0; n<m_size;++n)
				TBuf[n].~T();
		}

		fixedvector(const fixedvector& i_other):
		TBuf((T*)m_buf)
		{
			m_size=i_other.m_size;

			for (unsigned n=0; n<m_size; ++n)
				new (&(((T*)m_buf)[n])) T(((T*)i_other.m_buf)[n]);
		}

		void operator=(const fixedvector& i_other)
		{
			for (unsigned int n=0; n<m_size;++n)
				TBuf[n].~T();
			
			m_size=i_other.m_size;

			for (unsigned n=0; n<m_size; ++n)
				new ((&((T*)m_buf)[n])) T(((T*)i_other.m_buf)[n]);
		}

		unsigned size() const
		{
			return m_size;
		}

		unsigned capacity() const
		{
			return bufsize;
		}

		void resize(unsigned i_newsize)
		{
			if (m_size<i_newsize)
				for (unsigned n=m_size; n<i_newsize; ++n)
					new (&TBuf[n]) T();
			else
				for (unsigned n=i_newsize; n<m_size; ++n)
					TBuf[n].~T();

			m_size=i_newsize;
		}

		void clear()
		{
#if 1
			for (unsigned int n=0; n<m_size;++n)
				TBuf[n].~T();

			m_size=0;
#endif
		}

		void push_back(const T& i_elem)
		{
			assertion(m_size<bufsize,"vector is full");
			new (&TBuf[m_size++]) T(i_elem);
		}

		void pop_back()
		{
			assertion(m_size>0,"vector is empty");
			TBuf[--m_size].~T();
		}

		T& operator[](unsigned i_index)
		{
			assertion(i_index<m_size,"wrong index");
			return TBuf[i_index];
		}

		const T& operator[](unsigned i_index) const
		{
			assertion(i_index<m_size,"wrong index");
			return TBuf[i_index];
		}

		T& back()
		{
			assertion(m_size>0,"vector is empty");
			return TBuf[m_size-1];
		}

		const T& back() const
		{
			assertion(m_size>0,"vector is empty");
			return TBuf[m_size-1];
		}


		static const fixedvector& emptyvector()
		{
			static const fixedvector v; return v;
		}

	private:
		char m_buf[bufsize*sizeof(T)];
		T* TBuf;
		unsigned m_size;
	};
#endif//_fixedvector_h_