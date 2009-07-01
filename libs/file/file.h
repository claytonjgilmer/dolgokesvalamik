#ifndef _file_h_
#define _file_h_

#include "utils/misc.h"
#include "containers/string.h"
#include <stdio.h>
#include <sys/stat.h>

	struct file_t
	{
		file_t();
		file_t(const char* i_name, const char* i_attributes);
		~file_t();
		void open(const char* i_name, const char* i_attributes);
		void close();

		int read_line(string& o_line) const;
		void write_line(const char* i_line) const;
		unsigned read_bytes(char* o_bytes, unsigned i_numbytes) const;
		void write_bytes(const char* i_bytes, unsigned i_numbytes) const;

		void seek_cur(int i_offset);

		unsigned size() const;

		int opened() const;

		file_t(const file_t&);
		void operator=(const file_t&);
		FILE* m_handle;
		string m_name;
		unsigned m_size;
	};


	MLINLINE file_t::file_t()
	{
		m_handle=NULL;
		m_size=0;
	}

	MLINLINE file_t::file_t(const char* i_name,const char* i_attributes):
	m_handle(NULL)
	{
		open(i_name,i_attributes);
	}
//_CRTIMP int __cdecl __MINGW_NOTHROW	_fileno (FILE*);

	MLINLINE void file_t::open(const char* i_name, const char* i_attributes)
	{
		if (m_handle)
			close();

		m_handle=fopen(i_name,i_attributes);
		if (m_handle)
		{
			struct stat statistics;
			fstat(_fileno(m_handle),&statistics);
			m_size=statistics.st_size;
			m_name=i_name;
		}
	}

	MLINLINE void file_t::close()
	{
		if (!m_handle)
			return;

		fclose(m_handle);
		m_handle=NULL;
		m_name="";
		m_size=0;
	}

	MLINLINE file_t::~file_t()
	{
		if (m_handle)
			fclose(m_handle);
	}

	MLINLINE int file_t::read_line(string& o_line) const
	{
		char line[16384];

		if (fgets(line,16383,m_handle))
		{
			int len=(int)strlen(line)-1;

			while (len>=0)
			{
				if (line[len]==13 || line[len]==10)
				{
					line[len--]=0;
				}
				else
				{
					break;
				}
			}
			o_line=line;
			return true;
		}
		else
		{
			return false;
		}
	}

	MLINLINE void file_t::write_line(const char* i_line) const
	{
		fputs(i_line,m_handle);
		fputs("\n",m_handle);
	}

	MLINLINE unsigned file_t::read_bytes(char* o_bytes, unsigned i_numbytes) const
	{
		return (unsigned)fread(o_bytes,1,i_numbytes,m_handle);
	}

	MLINLINE void file_t::write_bytes(const char* i_bytes, unsigned i_numbytes) const
	{
		fwrite(i_bytes,1,i_numbytes,m_handle);
	}

	MLINLINE int file_t::opened() const
	{
		return m_handle!=NULL;
	}

	MLINLINE unsigned file_t::size() const
	{
		return m_size;
	}

	MLINLINE void file_t::seek_cur(int i_offset)
	{
		fseek(m_handle,i_offset,SEEK_CUR);
	}


	MLINLINE string get_extension(const char* i_filename)
	{
		const char* tmp=i_filename;
		unsigned len=(unsigned)strlen(i_filename);

		while (len)
		{
			if (*(tmp+len-1)=='.')
				break;

			--len;
		}

		if (!len)
			return string("");

		return string(tmp+len);
	}

#endif//_file_h_
