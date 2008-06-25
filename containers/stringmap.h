#ifndef _stringmap_h_
#define _stringmap_h_

#include "base/misc.h"

namespace ctr
{
	template <class T, unsigned bufsize=1024>
	class stringmap
	{
	public:
		stringmap();
		unsigned add_data(T* i_data);
		void remove_data(const ctr::string& i_name);
		T* get_data(const ctr::string& i_name);

	private:
		T* m_buf[bufsize];
	};

	MLINLINE unsigned int DJBHash(const ctr::string& str)
	{
		unsigned int hash = 5381;

		for(unsigned i = 0; i < str.size(); i++)
		{
			hash = ((hash << 5) + hash) + str[i];
		}

		return hash;
	}

	template<class T, unsigned bufsize>
	MLINLINE stringmap<T,bufsize>::stringmap()
	{
		for (unsigned n=0; n<bufsize; ++n)
			m_buf[n]=NULL;
	}

	template<class T, unsigned bufsize>
	MLINLINE unsigned stringmap<T,bufsize>::add_data(T* i_data)
	{
		unsigned hashkey=DJBHash(i_data->Name) & (bufsize-1);
		i_data->Next=m_buf[hashkey];
		m_buf[hashkey]=i_data;

		return hashkey;
	}

	template<class T, unsigned bufsize>
	MLINLINE void stringmap<T,bufsize>::remove_data(const ctr::string& i_name)
	{
		unsigned hashkey=DJBHash(i_name) & (bufsize-1);

		T* ptr=m_buf[hashkey];
		T* prevptr=NULL;

		while (ptr)
		{
			if (ptr->Name==i_name)
			{
				if (prevptr)
					prevptr->Next=ptr->Next;
				else
					m_buf[hashkey]=ptr->Next;

				break;

				prevptr=ptr;
			}
			ptr=ptr->Next;
		}
	}

	template<class T, unsigned bufsize>
	MLINLINE T* stringmap<T,bufsize>::get_data(const ctr::string& i_name)
	{
		unsigned hashkey=DJBHash(i_name) & (bufsize-1);

		T* ptr=m_buf[hashkey];

		while (ptr)
		{
			if (ptr->Name==i_name)
			{
				return ptr;
			}
			ptr=ptr->Next;
		}

		return NULL;
	}

}
#endif//_stringmap_h_