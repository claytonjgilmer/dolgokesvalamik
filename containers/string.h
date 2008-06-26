#ifndef _string_h_
#define _string_h_

#include <string.h>
#include "base\assert.h"
namespace ctr
{
	template <unsigned TSTRING_MIN_CAPACITY=16>
	class tstring
	{
	public:
		tstring();
		~tstring();

		explicit tstring(const char* i_str);
		tstring(const tstring<unsigned MINCAP2>& i_str);

		void operator=(const tstring& i_str);
		void operator=(const char* i_str);
		const char* c_str() const;
		unsigned size() const;

		bool operator==(const tstring& i_str) const;
		bool operator==(const char* i_str) const;
		char operator[](unsigned i_index) const;

	private:
		char* m_buf;
		char m_innerbuf[TSTRING_MIN_CAPACITY];
		unsigned m_capacity;
		unsigned m_length;

		void grow(unsigned i_capacity);
	};

	template <unsigned TSTRING_MIN_CAPACITY>
	inline void tstring<TSTRING_MIN_CAPACITY>::grow(unsigned i_capacity)
	{
		if (m_buf!=m_innerbuf)
			delete [] m_buf;

		m_buf=new char[i_capacity];
//		m_buf[0]=0;

		m_capacity=i_capacity;
//		m_length=0;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	inline tstring<TSTRING_MIN_CAPACITY>::tstring()
	{
		m_buf=m_innerbuf;
		m_buf[0]=0;
		m_capacity=TSTRING_MIN_CAPACITY;
		m_length=0;
	}

	template <unsigned TSTRING_MIN_CAPACITY,unsigned MINCAP2>
	inline tstring<TSTRING_MIN_CAPACITY>::tstring(const tstring<MINCAP2>& i_str)
	{
		m_buf=m_innerbuf;
		m_buf[0]=0;
		m_capacity=TSTRING_MIN_CAPACITY;

		if (i_str.m_capacity>TSTRING_MIN_CAPACITY)
			grow(i_str.m_capacity);

		strcpy(m_buf,i_str.m_buf);
		m_length=i_str.m_length;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	inline tstring<TSTRING_MIN_CAPACITY>::tstring(const char* i_str)
	{
		base::assertion(i_str!=NULL,"ctr::tstringnek nem illik nullpointert atadni");

		unsigned len=(unsigned)strlen(i_str);

		m_buf=m_innerbuf;
		m_buf[0]=0;
		m_capacity=TSTRING_MIN_CAPACITY;

		if (len>=TSTRING_MIN_CAPACITY)
			grow(len+1);

		m_length=len;
		strcpy(m_buf,i_str);
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	inline tstring<TSTRING_MIN_CAPACITY>::~tstring()
	{
		if (m_buf!=m_innerbuf)
			delete [] m_buf;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	inline const char* tstring<TSTRING_MIN_CAPACITY>::c_str() const
	{
		return m_buf;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	inline void tstring<TSTRING_MIN_CAPACITY>::operator =(const tstring& i_str)
	{
		if (m_capacity<=i_str.m_length)
			grow(i_str.m_length+1);

		strcpy(m_buf,i_str.m_buf);
		m_length=i_str.m_length;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	inline void tstring<TSTRING_MIN_CAPACITY>::operator =(const char* i_str)
	{
		unsigned len=(unsigned)strlen(i_str);

		if (m_capacity<=len)
			grow(len+1);

		strcpy(m_buf,i_str);
		m_length=len;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	inline unsigned tstring<TSTRING_MIN_CAPACITY>::size() const
	{
		return m_length;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	inline bool tstring<TSTRING_MIN_CAPACITY>::operator ==(const tstring& i_str) const
	{
		return !strcmp(m_buf,i_str.m_buf);
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	inline bool tstring<TSTRING_MIN_CAPACITY>::operator ==(const char* i_str) const
	{
		return !strcmp(m_buf,i_str);
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	inline char tstring<TSTRING_MIN_CAPACITY>::operator [](unsigned i_index) const
	{
		return m_buf[i_index];
	}

	typedef tstring<16> string;
}
#endif//_string_h_
