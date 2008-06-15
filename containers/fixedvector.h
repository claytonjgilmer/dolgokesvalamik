#ifndef _fixedvector_h_
#define _fixedvector_h_

#include <new>

namespace ctr
{
	template <class T, unsigned bufsize> class fixedvector
	{
	public:
#define TBuf ((T*)m_buf)
		fixedvector()
		{
			m_size=0;
		}

		unsigned size() const
		{
			return m_size;
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
			new (&TBuf[m_size++]) T(i_elem);
		}

		void pop_back()
		{
			TBuf[--m_size].~T();
		}

		T& operator[](int i_index)
		{
			return TBuf[i_index];
		}

		const T& operator[](int i_index) const
		{
			return TBuf[i_index];
		}

		static const fixedvector& emptyvector()
		{
			static const fixedvector v; return v;
		}

	private:
		char m_buf[bufsize*sizeof(T)];
		unsigned m_size;
	};
		
		
		
}
#endif//_fixedvector_h_