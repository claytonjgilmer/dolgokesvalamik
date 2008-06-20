#ifndef _string_h_
#define _string_h_

#define  NULL 0
#include <string.h>
#include "..\base\assert.h"
namespace ctr
{
#define STRING_MIN_CAPACITY 16
	class string
	{
	public:
		string();
		~string();

		string(const char* i_str);
		string(const string& i_str);

		void operator=(const string& i_str);
		void operator=(const char* i_str);
		const char* c_str() const;
		unsigned size() const;

	private:
		char* m_buf;
		unsigned m_capacity;
		unsigned m_length;

		void grow(unsigned i_capacity);
	};

	inline void string::grow(unsigned i_capacity)
	{
		if (m_capacity)
			delete [] m_buf;

		m_buf=new char[i_capacity];
//		m_buf[0]=0;

		m_capacity=i_capacity;
//		m_length=0;
	}

	inline string::string()
	{
		m_buf=new char[STRING_MIN_CAPACITY];
		m_buf[0]=0;
		m_capacity=STRING_MIN_CAPACITY;
		m_length=0;
	}

	inline string::string(const string& i_str)
	{
		m_capacity=0;
		grow(i_str.m_capacity);

		strcpy(m_buf,i_str.m_buf);
		m_length=i_str.m_length;
	}

	inline string::string(const char* i_str)
	{
		base::assertion(i_str!=NULL,"ctr::stringnek nem illik nullpointert atadni");

		unsigned len=(unsigned)strlen(i_str);

		m_capacity=0;
		grow(max(len+1,STRING_MIN_CAPACITY));

		m_length=len;
		strcpy(m_buf,i_str);
	}

	inline string::~string()
	{
		delete [] m_buf;
	}

	inline const char* string::c_str() const
	{
		return m_buf;
	}

	inline void string::operator =(const string& i_str)
	{
		if (m_capacity<i_str.m_length+1)
			grow(i_str.m_length+1);

		strcpy(m_buf,i_str.m_buf);
		m_length=i_str.m_length;
	}

	inline void string::operator =(const char* i_str)
	{
		unsigned len=(unsigned)strlen(i_str);

		if (m_capacity<len+1)
			grow(len+1);

		strcpy(m_buf,i_str);
		m_length=len;
	}

	inline unsigned string::size() const
	{
		return m_length;
	}
}
#endif//_string_h_