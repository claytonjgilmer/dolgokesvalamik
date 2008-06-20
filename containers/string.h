#ifndef _string_h_
#define _string_h_

#include <string.h>
#define  NULL 0
namespace ctr
{
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

	private:
		char* m_buf;
	};

	inline string::string()
	{
		m_buf=NULL;
	}

	inline string::string(const string& i_str)
	{
		if (!i_str.m_buf || !strlen(i_str.m_buf))
		{
			m_buf=NULL;
			return;
		}

		m_buf=new char[strlen(i_str.m_buf)];
		strcpy(m_buf,i_str.m_buf);
	}

	inline string::string(const char* i_str)
	{
		if (!i_str || !strlen(i_str))
			m_buf=NULL;
		else
		{
			m_buf=new char[strlen(i_str)];
			strcpy(m_buf,i_str);
		}
	}

	inline string::~string()
	{
		if (m_buf)
			delete [] m_buf;
	}

	inline const char* string::c_str() const
	{
		return m_buf;
	}

	inline void string::operator =(const string& i_str)
	{
		unsigned len=(unsigned)strlen(i_str.m_buf);

		if (strlen(m_buf)<len)
		{
			delete [] m_buf;
			m_buf=new char[len+1];
		}

		strcpy(m_buf,i_str.m_buf);
	}

	inline void string::operator =(const char* i_str)
	{
		unsigned len=(unsigned)strlen(i_str);

		if (!m_buf || strlen(m_buf)<len)
		{
			if (m_buf)
				delete [] m_buf;

			m_buf=new char[len+1];
		}

		strcpy(m_buf,i_str);
	}
}
#endif//_string_h_