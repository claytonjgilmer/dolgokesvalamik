#ifndef _stringmap_h_
#define _stringmap_h_

#include "base/misc.h"
#include "utils/stringhash.h"

extern int szamlalo;
extern int maxkeynum;

namespace ctr
{
	template <class T, unsigned bufsize=1024>
	class stringmap
	{
	public:
		stringmap();
		unsigned add_data(T* i_data);
		void remove_data(const char* i_name);
		T* get_data(const char* i_name) const; 

	private:
		T* m_buf[bufsize];
	};


#define hashfn utils::BKDRHash

	template<class T, unsigned bufsize>
	MLINLINE stringmap<T,bufsize>::stringmap()
	{
		for (unsigned n=0; n<bufsize; ++n)
			m_buf[n]=NULL;
	}

	template<class T, unsigned bufsize>
	MLINLINE unsigned stringmap<T,bufsize>::add_data(T* i_data)
	{
		unsigned hashkey=hashfn(i_data->Name) & (bufsize-1);
//		unsigned hashkey=hashfn(i_data->Name) % bufsize;
		i_data->Next=m_buf[hashkey];
		if (m_buf[hashkey]!=NULL)
			szamlalo++;
		m_buf[hashkey]=i_data;

		int szam=0;
		T* ptr=m_buf[hashkey];

		while (ptr)
		{
			++szam;
			ptr=ptr->Next;
		}

		if (szam>maxkeynum) maxkeynum=szam;


		return hashkey;
	}

	template<class T, unsigned bufsize>
	MLINLINE void stringmap<T,bufsize>::remove_data(const char* i_name)
	{
		unsigned hashkey=hashfn(ctr::string(i_name)) & (bufsize-1);
//		unsigned hashkey=hashfn(i_name) % bufsize;

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
	MLINLINE T* stringmap<T,bufsize>::get_data(const char* i_name) const
	{
		unsigned hashkey=hashfn(ctr::string(i_name)) & (bufsize-1);
//		unsigned hashkey=hashfn(i_name) % bufsize;

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