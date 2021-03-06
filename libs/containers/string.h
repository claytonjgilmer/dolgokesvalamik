#ifndef _string_h_
#define _string_h_

//#include <string.h>
#include <ctype.h>
#include <float.h>
#include "utils/assert.h"
#include "utils/misc.h"
#include "math/math.h"

	template <unsigned TSTRING_MIN_CAPACITY=16>
	struct tstring
	{
		tstring();
		~tstring();

		tstring(const char* i_str);
		tstring(const tstring& i_str);
		template <unsigned MINCAP2> tstring(const tstring<MINCAP2>& i_str);

		template <unsigned MINCAP2> void operator=(const tstring<MINCAP2>& i_str);
		void operator=(const tstring& i_str);
		void operator=(const char* i_str);
		const char* c_str() const;
		unsigned size() const;
		unsigned capacity() const;

		tstring substring(unsigned i_pos, unsigned i_length) const;
		int match(unsigned i_pos, const char* i_str) const;

		unsigned find(const char* i_substring) const;

		template <unsigned MINCAP2> int operator==(const tstring<MINCAP2>& i_str) const;
		int operator==(const char* i_str) const;
		int operator!=(const char* i_str) const;
		char operator[](unsigned i_index) const;

		tstring& operator+=(const char* i_str);
		tstring& operator+=(int i_num);
		tstring& operator+=(f32 i_num);

		tstring operator+(const char* i_str) const;
		tstring operator+(int i_num) const;
		tstring operator+(f32 i_num) const;

		friend
		tstring operator+(const char* i_str, const tstring& i_ts)
		{
			return tstring(i_str)+i_ts.c_str();
		}

		void to_upper();
		void to_lower();

		char* m_buf;
		char m_innerbuf[TSTRING_MIN_CAPACITY];
		unsigned m_capacity;
		unsigned m_length;

		void alloc(unsigned i_capacity);
		void grow(unsigned i_newcapacity);
	};

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE void tstring<TSTRING_MIN_CAPACITY>::alloc(unsigned i_capacity)
	{
		if (m_buf!=m_innerbuf)
			delete [] m_buf;

		m_buf=new char[i_capacity];
		m_capacity=i_capacity;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE void tstring<TSTRING_MIN_CAPACITY>::grow(unsigned i_newcapacity)
	{

		char* tmpbuf=new char[i_newcapacity];
		strcpy(tmpbuf,m_buf);

		if (m_buf!=m_innerbuf)
			delete [] m_buf;

		m_buf=tmpbuf;
		m_capacity=i_newcapacity;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE tstring<TSTRING_MIN_CAPACITY>::tstring()
	{
		m_buf=m_innerbuf;
		m_buf[0]=0;
		m_capacity=TSTRING_MIN_CAPACITY;
		m_length=0;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE tstring<TSTRING_MIN_CAPACITY>::tstring(const tstring& i_str)
	{
		m_buf=m_innerbuf;
		m_buf[0]=0;
		m_capacity=TSTRING_MIN_CAPACITY;

		if (i_str.capacity()>TSTRING_MIN_CAPACITY)
			alloc(i_str.capacity());

		strcpy(m_buf,i_str.c_str());
		m_length=i_str.size();
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	template <unsigned MINCAP2>
	MLINLINE tstring<TSTRING_MIN_CAPACITY>::tstring(const tstring<MINCAP2>& i_str)
	{
		m_buf=m_innerbuf;
		m_buf[0]=0;
		m_capacity=TSTRING_MIN_CAPACITY;

		if (i_str.capacity()>TSTRING_MIN_CAPACITY)
			alloc(i_str.capacity());

		strcpy(m_buf,i_str.c_str());
		m_length=i_str.size();
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE tstring<TSTRING_MIN_CAPACITY>::tstring(const char* i_str)
	{
		assertion(i_str!=NULL,"tstringnek nem illik nullpointert atadni");

		unsigned len=(unsigned)strlen(i_str);

		m_buf=m_innerbuf;
		m_buf[0]=0;
		m_capacity=TSTRING_MIN_CAPACITY;

		if (len>=TSTRING_MIN_CAPACITY)
			alloc(len+1);

		m_length=len;
		strcpy(m_buf,i_str);
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE tstring<TSTRING_MIN_CAPACITY>::~tstring()
	{
		if (m_buf!=m_innerbuf)
			delete [] m_buf;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE const char* tstring<TSTRING_MIN_CAPACITY>::c_str() const
	{
		return m_buf;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE void tstring<TSTRING_MIN_CAPACITY>::operator =(const tstring& i_str)
	{
		if (m_capacity<=i_str.size())
			alloc(i_str.size()+1);

		strcpy(m_buf,i_str.c_str());
		m_length=i_str.size();
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	template <unsigned MINCAP2>
	MLINLINE void tstring<TSTRING_MIN_CAPACITY>::operator =(const tstring<MINCAP2>& i_str)
	{
		if (m_capacity<=i_str.size())
			alloc(i_str.size()+1);

		strcpy(m_buf,i_str.c_str());
		m_length=i_str.size();
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE void tstring<TSTRING_MIN_CAPACITY>::operator =(const char* i_str)
	{
		unsigned len=(unsigned)strlen(i_str);

		if (m_capacity<=len)
			alloc(len+1);

		strcpy(m_buf,i_str);
		m_length=len;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE unsigned tstring<TSTRING_MIN_CAPACITY>::size() const
	{
		return m_length;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	template <unsigned MINCAP2>
	MLINLINE int tstring<TSTRING_MIN_CAPACITY>::operator ==(const tstring<MINCAP2>& i_str) const
	{
		return !strcmp(m_buf,i_str.c_str());
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE int tstring<TSTRING_MIN_CAPACITY>::operator ==(const char* i_str) const
	{
		return !strcmp(m_buf,i_str);
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE int tstring<TSTRING_MIN_CAPACITY>::operator !=(const char* i_str) const
	{
		return strcmp(m_buf,i_str)!=0;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE char tstring<TSTRING_MIN_CAPACITY>::operator [](unsigned i_index) const
	{
		return m_buf[i_index];
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE unsigned tstring<TSTRING_MIN_CAPACITY>::capacity() const
	{
		return m_capacity;
	}


	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE tstring<TSTRING_MIN_CAPACITY>& tstring<TSTRING_MIN_CAPACITY>::operator+=(const char* i_str)
	{
		unsigned len=(unsigned)strlen(i_str);

		if (m_capacity<=m_length+len)
			grow(m_length+len+1);

		strcpy(m_buf+m_length,i_str);
		m_length+=len;

		return *this;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE tstring<TSTRING_MIN_CAPACITY>& tstring<TSTRING_MIN_CAPACITY>::operator+=(int i_num)
	{
		char strtmp[16];
		sprintf(strtmp,"%d",i_num);
//		_itoa(i_num,strtmp,10);
		return *this+=strtmp;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE tstring<TSTRING_MIN_CAPACITY>& tstring<TSTRING_MIN_CAPACITY>::operator+=(f32 i_num)
	{
		char strtmp[16];
		sprintf(strtmp,"%f",i_num);
//		_ftoa(i_num,strtmp,10);
		return *this+=strtmp;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE tstring<TSTRING_MIN_CAPACITY> tstring<TSTRING_MIN_CAPACITY>::operator+(const char* i_str) const
	{
		tstring strmp(*this);
		strmp+=i_str;
		return strmp;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE tstring<TSTRING_MIN_CAPACITY> tstring<TSTRING_MIN_CAPACITY>::operator+(int i_num) const
	{
		tstring strmp(*this);
		strmp+=i_num;
		return strmp;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE tstring<TSTRING_MIN_CAPACITY> tstring<TSTRING_MIN_CAPACITY>::operator+(f32 i_num) const
	{
		tstring strmp(*this);
		strmp+=i_num;
		return strmp;
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE void tstring<TSTRING_MIN_CAPACITY>::to_upper()
	{
		for (unsigned n=0; n<m_length; ++n)
			m_buf[n]=toupper(m_buf[n]);
	}

	template <unsigned TSTRING_MIN_CAPACITY>
	MLINLINE void tstring<TSTRING_MIN_CAPACITY>::to_lower()
	{
		for (unsigned n=0; n<m_length; ++n)
			m_buf[n]=tolower(m_buf[n]);
	}

	template<unsigned  TSTRING_MIN_CAPACITY>
	MLINLINE tstring<TSTRING_MIN_CAPACITY> tstring<TSTRING_MIN_CAPACITY>::substring(unsigned i_pos, unsigned i_length) const
	{
		assertion(i_pos+i_length<=m_length,"nem jau");
		tstring res;
		if (i_length+1>res.m_capacity)
			res.grow(i_length);

		for (unsigned n=0; n<i_length; ++n)
		{
			res.m_buf[n]=m_buf[i_pos+n];
		}

		res.m_buf[i_length]=0;

		return res;
	}

	template<unsigned  TSTRING_MIN_CAPACITY>
	MLINLINE unsigned tstring<TSTRING_MIN_CAPACITY>::find(const char* i_substring) const
	{
		char* ret=strstr(m_buf,i_substring);

		if (ret)
			return ret-m_buf;
		else
			return m_length;
	}

	template<unsigned  TSTRING_MIN_CAPACITY>
	MLINLINE int tstring<TSTRING_MIN_CAPACITY>::match(unsigned i_pos, const char* i_str) const
	{
		assertion(i_pos<=m_length);

		int i=0;

		while (m_buf[i_pos+i] && i_str[i] && m_buf[i_pos+i]==i_str[i]) ++i;

		return (!i_str[i]);
	}

	typedef tstring<16> string;
#endif//_string_h_
